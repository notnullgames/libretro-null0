#include "wasm3.h"
#include "m3_env.h"
#include "physfs.h"
#include <libgen.h>

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;

/**
 * Determine if a file exists in the search path.
 *
 * @param fileName Filename in platform-independent notation.
 *
 * @return True if the file exists, false otherwise.
 *
 * @see DirectoryExistsInPhysFS()
 */
bool FileExistsInPhysFS(const char* fileName) {
    PHYSFS_Stat stat;
    if (PHYSFS_stat(fileName, &stat) == 0) {
        return false;
    }
    return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

/**
 * Get filsize in bytes from PhysFS.
 *
 * @param fileName The filename to load from the search paths.
 *
 * @return The bytes size or -1 for error
 */
int FileSizeFromPhysFS(const char* fileName) {
  if (FileExistsInPhysFS(fileName)) {
    void* handle = PHYSFS_openRead(fileName);
    int size = PHYSFS_fileLength(handle);
    return size;
  } else {
   return -1;
  }
}

enum FileType {
   FileTypeInvalid,
   FileTypeDir,
   FileTypeWasm,
   FileTypeZip
};

void strreplace(char *string, const char *find, const char *replaceWith){
  if(strstr(string, find) != NULL){
    char *temporaryString = malloc(strlen(strstr(string, find) + strlen(find)) + 1);
    strcpy(temporaryString, strstr(string, find) + strlen(find));    //Create a string with what's after the replaced part
    *strstr(string, find) = '\0';    //Take away the part to replace and the part after it in the initial string
    strcat(string, replaceWith);    //Concat the first part of the string with the part to replace with
    strcat(string, temporaryString);    //Concat the first part of the string with the part after the replaced part
    free(temporaryString);    //Free the memory to avoid memory leaks
  }
}

// detect rom-type
enum FileType null0_rom_type (const char *d) {
   DIR *dirptr;
   if (access ( d, F_OK ) != -1 ) {
      if ((dirptr = opendir (d)) != NULL) {
         // d exists and is a directory
         closedir (dirptr);
         return FileTypeDir;
      } else {
         // d exists but is not a directory, detect zip
         unsigned char bytes[4];
         FILE* fp=fopen(d, "r");
         fread(&bytes, 4, 1, fp);
         fclose(fp);
         if (bytes[0] == 0x50 && bytes[1] == 0x4b && bytes[2] == 0x03 && bytes[3] == 0x04) {
            return FileTypeZip;
         } else {
            return FileTypeWasm;
         }
      }
   } else {
      // d does not exist
      return FileTypeInvalid;
   }
}

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3 (M3Result result) {
  if (result) {
    M3ErrorInfo info;
    m3_GetErrorInfo(runtime, &info);
    printf("%s - %s", result, info.message);
  }
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok () {
  M3ErrorInfo error;
  m3_GetErrorInfo(runtime, &error);
  if (error.result) {
    printf("%s - %s\n", error.result, error.message);
  }
}

// Log a string
static m3ApiRawFunction (null0_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// load a wasm binary buffer
void null0_load_cart_wasm (u8* wasmBuffer, int byteLength) {
  env = m3_NewEnvironment();
  runtime = m3_NewRuntime (env, 1024, NULL);
  null0_check_wasm3(m3_ParseModule (env, &module, wasmBuffer, byteLength));
  null0_check_wasm3(m3_LoadModule(runtime, module));

  // IMPORTS
  m3_LinkRawFunction(module, "env", "null0_log", "v(i)", &null0_log);

  null0_check_wasm3_is_ok();

  // EXPORTS
  static M3Function* cart_init;
  m3_FindFunction(&cart_init, runtime, "init");

  if (cart_init) {
    null0_check_wasm3(m3_CallV(cart_init));
  } else {
    printf("error in init() in cart.\n");
  }
}

// mount a file/dir location as root and start the wasm engine
bool null0_mount(char* retro_game_path) {
  PHYSFS_init("null0");

  // TODO: is this an ok way to set game-name?
  char* name = basename(retro_game_path);
  strreplace(name, ".", "_");
  PHYSFS_setWriteDir(PHYSFS_getPrefDir("null0", name));

  enum FileType romType = null0_rom_type(retro_game_path);
  if (romType == FileTypeInvalid) {
    return false;
  } else if (romType != FileTypeWasm) {
    // mount zip/dir as read-root
    PHYSFS_mount(retro_game_path, NULL, 0);
  }

  // add the writable dir to search-path
  PHYSFS_mount(PHYSFS_getWriteDir(), NULL, 0);

  if (romType == FileTypeWasm) {
    // wasm files are special, as there is no root-read filesystem, it's just the entry-point
    FILE *wasmFile;
    wasmFile = fopen(retro_game_path, "rb");

    // TODO: read file in to wasmBuffer/wasmLen
    printf("direct wasm-loading not implemented yet.\n");
    return false;

    fclose(wasmFile);
  } else {
    if (!FileExistsInPhysFS("cart.wasm")) {
      printf("no cart.wasm\n");
      return false;
    }

    PHYSFS_File* wasmFile = PHYSFS_openRead("cart.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    u8* wasmBuffer[wasmLen];
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
    if (bytesRead == -1) {
      printf("Error opening cart.wasm: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return false;
    }

    null0_load_cart_wasm ((u8*)wasmBuffer, wasmLen);
  }

  return true;
}
#include "wasm3.h"
#include "m3_env.h"
#include "physfs.h"
#include <libgen.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum Null0FileType {
   Null0FileTypeInvalid,
   Null0FileTypeOther,
   Null0FileTypeDir,
   Null0FileTypeWasm,
   Null0FileTypeZip
};

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;

static M3Function* as_new;

// copy a value into wasm-space
uint32_t lowerBuffer(char* value) {
  uint32_t ptr;
  // currently this supports assmeblyscript, but other languages can also export __new(size, 1): ptr

  return ptr;
}

// copy a value from wasm-space
char* liftBuffer(uint32_t ptr) {
  char* value;
  return value;
}


char* joinpath(char* path1, char* path2) {
    if(path1 == NULL && path2 == NULL) {
        return NULL;
    }

    if(path2 == NULL || strlen(path2) == 0) return path1;
    if(path1 == NULL || strlen(path1) == 0) return path2;

    char* directory_separator = "";
#ifdef WIN32
    directory_separator = "\\";
#else 
    directory_separator = "/";
#endif

    char p1[strlen(path1)];                    // (1)
    strcpy(p1, path1);                         // (2) 
    char *last_char = &p1[strlen(path1) - 1];  // (3)

    char *combined = malloc(strlen(path1) + 1 + strlen(path2));
    int append_directory_separator = 0;
    if(strcmp(last_char, directory_separator) != 0) {
        append_directory_separator = 1;
    }
    strcpy(combined, path1);
    if(append_directory_separator)
        strcat(combined, directory_separator);
    strcat(combined, path2);
    return combined;
}

bool FileExistsInPhysFS(const char* fileName) {
    PHYSFS_Stat stat;
    if (PHYSFS_stat(fileName, &stat) == 0) {
        return false;
    }
    return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

// detect rom-type
enum Null0FileType null0_rom_type (const char *d) {
  DIR *dirptr;
  if (access ( d, F_OK ) != -1 ) {
    if ((dirptr = opendir (d)) != NULL) {
      // d exists and is a directory
      closedir (dirptr);
      return Null0FileTypeDir;
    } else {
      // d exists but is not a directory, detect zip/wasm
      unsigned char bytes[4];
      FILE* fp=fopen(d, "r");
      fread(&bytes, 4, 1, fp);
      fclose(fp);
      if (bytes[0] == 0x50 && bytes[1] == 0x4b && bytes[2] == 0x03 && bytes[3] == 0x04) {
        return Null0FileTypeZip;
      } else if (bytes[0] == 0x00 && bytes[1] == 0x61 && bytes[2] == 0x73 && bytes[3] == 0x6d) {
        return Null0FileTypeWasm;
      } else {
        return Null0FileTypeOther;
      }
    }
  } else {
    // d does not exist
    return Null0FileTypeInvalid;
  }
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok () {
  M3ErrorInfo error;
  m3_GetErrorInfo(runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
  }
}

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3 (M3Result result) {
  if (result) {
    null0_check_wasm3_is_ok();
  }
}

// Fatal error
static m3ApiRawFunction (null0_abort) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  m3ApiSuccess();
}

// Log a string
static m3ApiRawFunction (null0_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// Log a string
static m3ApiRawFunction (null0_http_request_get) {
  printf("null0_http_request_get\n");
  m3ApiSuccess();
}

void null0_load_cart_wasm (u8* wasmBuffer, int byteLength) {
  env = m3_NewEnvironment();
  runtime = m3_NewRuntime (env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule (env, &module, wasmBuffer, byteLength));
  null0_check_wasm3(m3_LoadModule(runtime, module));

  // IMPORTS
  m3_LinkRawFunction(module, "env", "null0_log", "v(*)", &null0_log);
  m3_LinkRawFunction(module, "env", "abort", "v(**ii)", &null0_abort);
  m3_LinkRawFunction(module, "env", "null0_http_request_get", "*(*)", &null0_http_request_get);

  null0_check_wasm3_is_ok();

  // EXPORTS
  // __new(size: usize, id: u32): usize should be implemented in non-as wasm
  m3_FindFunction(&as_new, runtime, "__new");
  if (!as_new) {
    fprintf(stderr, "__new not exported.\n");
    exit(1);
  }
  static M3Function* cart_load;
  m3_FindFunction(&cart_load, runtime, "load");
  static M3Function* cart_update;
  m3_FindFunction(&cart_update, runtime, "update");
  static M3Function* cart_draw;
  m3_FindFunction(&cart_draw, runtime, "draw");

  if (cart_load) {
    null0_check_wasm3(m3_CallV(cart_load));
  }
}

bool null0_run(char* retro_game_path) {
  char* dirname = strdup(retro_game_path);

  enum Null0FileType retro_game_path_type = null0_rom_type(retro_game_path);

  if (retro_game_path_type == Null0FileTypeInvalid || retro_game_path_type == Null0FileTypeOther) {
    fprintf(stderr, "Unknown filetype '%s'. Pass a zip or wasm file, or a directory with main.wasm.\n", retro_game_path);
    return false;
  }

  PHYSFS_init("null0");

  // Bare wasm files do not get read file-root
  if (retro_game_path_type != Null0FileTypeWasm) {
    PHYSFS_mount(retro_game_path, NULL, 0);
    if (!FileExistsInPhysFS("main.wasm")) {
      fprintf(stderr, "No main.wasm in '%s'. Pass a zip or wasm file, or a directory with main.wasm.\n", retro_game_path);
      PHYSFS_deinit();
      return false;
    } else {
      PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
      PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
      u8* wasmBuffer[wasmLen];
      PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
      if (bytesRead == -1) {
        fprintf(stderr, "Error reading main.wasm.\n");
        PHYSFS_deinit();
        return false;
      }
      null0_load_cart_wasm ((u8*)wasmBuffer, wasmLen);
    }
  } else {
    int fd = open(retro_game_path, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "Could not open '%s'.\n", retro_game_path);
      PHYSFS_deinit();
      return false;
    } else {
      struct stat sb;
      stat(retro_game_path, &sb);
      u8* wasmBuffer[sb.st_size];
      read(fd, wasmBuffer, sb.st_size);
      close(fd);
      null0_load_cart_wasm ((u8*)wasmBuffer, sb.st_size);
    }
  }

  PHYSFS_deinit();
  return true;
}
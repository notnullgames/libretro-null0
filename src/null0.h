#include "wasm3.h"
#include "m3_env.h"
#include "physfs.h"
#include <libgen.h>
#include <curl/curl.h>

enum Null0FileType {
   Null0FileTypeInvalid,
   Null0FileTypeDir,
   Null0FileTypeWasm,
   Null0FileTypeZip
};

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;
static M3Function* wmalloc;
static M3Function* cart_init;


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

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t nullo_http_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    fprintf(stderr, "not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
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
         // d exists but is not a directory, detect zip
         unsigned char bytes[4];
         FILE* fp=fopen(d, "r");
         fread(&bytes, 4, 1, fp);
         fclose(fp);
         if (bytes[0] == 0x50 && bytes[1] == 0x4b && bytes[2] == 0x03 && bytes[3] == 0x04) {
            return Null0FileTypeZip;
         } else {
            return Null0FileTypeWasm;
         }
      }
   } else {
      // d does not exist
      return Null0FileTypeInvalid;
   }
}

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3 (M3Result result) {
  if (result) {
    M3ErrorInfo info;
    m3_GetErrorInfo(runtime, &info);
    fprintf(stderr, "%s - %s", result, info.message);
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

// Fatal error
static m3ApiRawFunction (null0_abort) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  m3ApiSuccess();
}

static m3ApiRawFunction (null0_http_request_get) {
  m3ApiGetArgMem(const char*, url);
  m3ApiReturnType (uint32_t);

  printf("Url: %s", url);

  #ifdef NULL0_HTTP
    struct MemoryStruct chunk;
    CURL *curl_handle;
    CURLcode res;

    
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, nullo_http_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    curl_global_cleanup();

    // lowerBuffer
    uint32_t wPointer;
    size_t s = strlen(chunk.memory) + 1;
    null0_check_wasm3(m3_CallV (wmalloc, s));
    m3_GetResultsV(wmalloc, &wPointer);
    char* wBuffer = m3ApiOffsetToPtr(wPointer);
    memcpy(wBuffer, chunk.memory, s);

    m3ApiReturn(wPointer);
  #else
    fprintf(stderr, "Attempting to GET %s but HTTP is disabled.\n", url);
    return
  #endif

  m3ApiSuccess();
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
  runtime = m3_NewRuntime (env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule (env, &module, wasmBuffer, byteLength));
  null0_check_wasm3(m3_LoadModule(runtime, module));

  // IMPORTS
  m3_LinkRawFunction(module, "env", "abort", "v(iiii)", &null0_abort);
  m3_LinkRawFunction(module, "env", "null0_log", "v(i)", &null0_log);
  m3_LinkRawFunction(module, "env", "null0_http_request_get", "*(i)", &null0_http_request_get);

  null0_check_wasm3_is_ok();

  // EXPORTS
  m3_FindFunction(&wmalloc, runtime, "wmalloc");
  m3_FindFunction(&cart_init, runtime, "init");

  null0_check_wasm3_is_ok();

  if (!wmalloc) {
    fprintf(stderr, "no wmalloc in cart\n");
    exit(1);
  }
  

  if (cart_init) {
    null0_check_wasm3(m3_CallV(cart_init));
  } else {
    fprintf(stderr, "error in init() in cart.\n");
    exit(1);
  }
}

// mount a file/dir location as root and start the wasm engine
bool null0_run(char* retro_game_path) {
  PHYSFS_init("null0");

  // TODO: is this an ok way to set game-name?
  char* name = basename(retro_game_path);
  strreplace(name, ".", "_");
  PHYSFS_setWriteDir(PHYSFS_getPrefDir("null0", name));

  enum Null0FileType romType = null0_rom_type(retro_game_path);
  if (romType == Null0FileTypeInvalid) {
    return false;
  } else if (romType != Null0FileTypeWasm) {
    // mount zip/dir as read-root
    PHYSFS_mount(retro_game_path, NULL, 0);
  }

  // add the writable dir to search-path
  PHYSFS_mount(PHYSFS_getWriteDir(), NULL, 0);

  if (romType == Null0FileTypeWasm) {
    // wasm files are special, as there is no root-read filesystem, it's just the entry-point
    FILE *wasmFile;
    wasmFile = fopen(retro_game_path, "rb");

    // TODO: read file in to wasmBuffer/wasmLen
    fprintf(stderr, "direct wasm-loading not implemented yet.\n");
    return false;

    fclose(wasmFile);
  } else {
    if (!FileExistsInPhysFS("main.wasm")) {
      fprintf(stderr, "no main.wasm\n");
      return false;
    }

    PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    u8* wasmBuffer[wasmLen];
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
    if (bytesRead == -1) {
      fprintf(stderr, "Error opening main.wasm: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return false;
    }

    null0_load_cart_wasm ((u8*)wasmBuffer, wasmLen);
  }

  return true;
}
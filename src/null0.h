#ifndef NULL0_CORE_H
#define NULL0_CORE_H

#define NULL0_RESOURCE_MAX 255
#define NULL0_WIDTH 320
#define NULL0_HEIGHT 240

#include <dirent.h>
#include <sys/time.h>
#include <unistd.h>
#include "m3_env.h"
#include "wasm3.h"

struct Null0CartData {
  u8* bytes;
  u32 size;
  char* filename;
};
typedef struct Null0CartData Null0CartData;

#include "null0-fs.h"
#include "null0-graphics.h"
#include "physfs.h"

struct timespec null0_time_start;
struct timespec null0_time_now;

// these will be used for anything that needs to allocate a buffer in wasm-space
M3Function* null0_pin;
M3Function* null0_new;
M3Function* null0_unpin;

M3Function* null0_cart_update;
M3Function* null0_cart_unload;

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3(M3Result result, M3Runtime* runtime) {
  if (result) {
    M3ErrorInfo info;
    m3_GetErrorInfo(runtime, &info);
    fprintf(stderr, "%s - %s\n", result, info.message);
    exit(1);
  }
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok(M3Runtime* runtime) {
  M3ErrorInfo error;
  m3_GetErrorInfo(runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
    exit(1);
  }
}

static m3ApiRawFunction(null0_import_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_fatal) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, filename);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, filename, lineNumber, columnNumber);
  exit(1);
}

static m3ApiRawFunction(null0_import_seed) {
  m3ApiReturnType(double);
  m3ApiReturn(null0_time_start.tv_nsec);
}

bool null0_imports_core(M3Module* module) {
  m3_LinkRawFunction(module, "null0", "log", "v(*)", &null0_import_log);
  m3_LinkRawFunction(module, "null0", "fatal", "v(**ii)", &null0_import_fatal);
  m3_LinkRawFunction(module, "null0", "seed", "F()", &null0_import_seed);

  return true;
}

// given a partial cart, load the complete cart for the wasm
void* null_cart_to_wasm(Null0CartData incart) {
  if (incart.filename == NULL) {
    // you need at least the filename
    return NULL;
  }

  if (access(incart.filename, F_OK) == -1) {
    return NULL;
  }

  static Null0CartData cart;
  bool lookInPhys = false;

  PHYSFS_init(incart.filename);

  DIR* dirptr = opendir(incart.filename);
  if (dirptr == NULL) {
    cart = null0_load_file(incart.filename);
    if (cart.size == 0) {
      return NULL;
    }
    Null0CartData sourceFile = null0_load_file(incart.filename);
    if (null0_fs_is_zip(sourceFile.bytes)) {
      // it's a zip, mount it
      PHYSFS_mountMemory(sourceFile.bytes, sourceFile.size, NULL, incart.filename, NULL, 0);
      lookInPhys = true;
    } else if (null0_fs_is_wasm(sourceFile.bytes)) {
      // it's a wasm file
      cart = sourceFile;
    }
  } else {
    // it's a directory, mount it
    PHYSFS_mount(incart.filename, NULL, 0);
    lookInPhys = true;
  }

  if (lookInPhys) {
    if (!null0_fs_exists("main.wasm")) {
      return NULL;
    }

    PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    u8* wasmBuffer = malloc(wasmLen);
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBuffer, wasmLen);
    PHYSFS_close(wasmFile);
    cart.size = wasmLen;
    cart.bytes = (u8*)wasmBuffer;
  }

  if (!null0_fs_is_wasm(cart.bytes)) {
    return NULL;
  }

  return &cart;
}

// once you've got wasm loaded in cart, hand to this to setup things
bool null0_load(Null0CartData* cart) {
  M3Environment* env;
  M3Runtime* runtime;
  M3Module* module;

  module = NULL;
  env = m3_NewEnvironment();
  runtime = m3_NewRuntime(env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule(env, &module, cart->bytes, cart->size), runtime);
  null0_check_wasm3(m3_LoadModule(runtime, module), runtime);

  bool s;

  // mount the write-dir, so the cart can read from it
  PHYSFS_mount(PHYSFS_getWriteDir(), NULL, 0);

  // imports

  s = null0_imports_core(module);
  if (!s) {
    fprintf(stderr, "null0: could not intiialize core imports.\n");
    exit(1);
  }

  s = null0_imports_fs(module);
  if (!s) {
    fprintf(stderr, "null0: could not intiialize fs imports.\n");
    exit(1);
  }

  s = null0_imports_graphics(module);
  if (!s) {
    fprintf(stderr, "null0: could not intiialize graphics imports.\n");
    exit(1);
  }

  // exports
  M3Function* null0_cart_load;

  m3_FindFunction(&null0_cart_load, runtime, "load");
  m3_FindFunction(&null0_cart_update, runtime, "update");
  m3_FindFunction(&null0_cart_unload, runtime, "unload");
  m3_FindFunction(&null0_pin, runtime, "__pin");
  m3_FindFunction(&null0_new, runtime, "__new");
  m3_FindFunction(&null0_unpin, runtime, "__unpin");

  if (null0_cart_load) {
    null0_check_wasm3(m3_CallV(null0_cart_load), runtime);
  } else {
    fprintf(stderr, "no load() in cart.\n");
  }

  return true;
}

#endif  // NULL0_CORE_H
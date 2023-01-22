// null0 host C header, generated 2023-01-22T04:11:03.002Z

#include "wasm3.h"
#include "m3_env.h"
#include "physfs.h"

#include <sys/time.h>

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;

static M3Function* new_func;
static M3Function* cart_load;
static M3Function* cart_update;
Image* screenBuffer;

struct timeval now, start;

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

// IMPORTS

// Fatal error
// this is an assemblyscript-thing, but you can export one from any language
// message/filename is WTF-16 encoded
static m3ApiRawFunction (null0_abort) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  m3ApiSuccess();
}



// call this in your game-loop
void null0_update_loop() {
  gettimeofday(&now, NULL);
  if (cart_update) {
    null0_check_wasm3(m3_CallV(cart_update, (now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec));
  } else {
    // TODO: some cute embedded "no update" screen here
  }
}

// this is the actual wasm-runner (after you have setup FS and stuff)
bool null0_start(const void* wasmBuffer, size_t byteLength) {
  gettimeofday(&start, NULL);

  env = m3_NewEnvironment();
  runtime = m3_NewRuntime (env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule (env, &module, wasmBuffer, byteLength));
  null0_check_wasm3(m3_LoadModule(runtime, module));

  // IMPORTS
  m3_LinkRawFunction(module, "env", "abort", "v(**ii)", &null0_abort);
  

  null0_check_wasm3_is_ok();

  // EXPORTS
  m3_FindFunction(&new_func, runtime, "__new");
  m3_FindFunction(&cart_load, runtime, "load");
  m3_FindFunction(&cart_update, runtime, "update");

  if (!new_func) {
    // this means no string-returns
    // it should be exported: __new(size: usize, id: u32 (1)): usize
    printf("no __new.\n");
  }

  if (cart_load) {
    null0_check_wasm3(m3_CallV(cart_load));
  } else {
    printf("no load.\n");
  }

  if (!cart_update) {
    printf("no update.\n");
    // do load of any assets for "no update" screen here
  }
  return true;
}

// load a "cart", which is either a directory or a wasm/zip file
bool null0_load(Image* imageBuffer, const void* fileBuffer, size_t byteLength, const char* filePath) {
  screenBuffer = imageBuffer;
}

// call this when you are ready to exit
void null_unload() {}



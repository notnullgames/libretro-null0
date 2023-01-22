// this will generate the C host-header
// WARNING this is not complete, use it as a base!

// TODO: more checks with m3ApiCheckMem
// TODO: look into 

import funcs from './api.json' assert { type: 'json' }

// use as filter to not output some functions
const noFilter = ({name}) => ![
  'GetPixelColor',
  'SetPixelColor',
  'LoadImageRaw',
  'LoadImageFromMemory',
  'LoadImageFromScreen',
  'ExportImageAsCode',
  'UnloadImageColors',
  'UnloadImagePalette',
  'LoadImageColors',
  'LoadImagePalette',
  'log'
].includes(name)

// generate a single arg-handler
function genArgHandler(arg) {
  // TODO: handle all the differnt types of args
  if (!arg[1]) {
    return ''
  }
  if (arg[0].includes('*')) {
    return `m3ApiGetArgMem(${arg[0]}, ${arg[1]});`
  } else {
    return `m3ApiGetArg(${argTypeMap[arg[0]] || arg[0]}, ${arg[1]});`
  }
}

// this will massage the data
function genArgMassager(arg) {
  return arg[1]
}

// return the mapped type
const returnTypeMap = {
  'int': 'uint32_t',
  'float': 'float'
}

// mapped argument types
const argTypeMap = {
  'int': 'uint32_t',
  'float': 'float'
}

// the import-defs for your functions
function getImports() {
  return funcs.filter(noFilter).map(func => {
    return `// ${func.comment}
static m3ApiRawFunction (null0_${func.name}) {
  ${func.returns && func.returns !== 'void' ? (`m3ApiReturnType (${returnTypeMap[func.returns] || func.returns }); // ${func.returns}`): ''}
  ${func.params.map(genArgHandler).join('\n  ')}
  ${func.returns && func.returns !== 'void' ? (
    `m3ApiReturn(${func.name}(${func.params.map(genArgMassager).join(', ')}));`
  ) : ''}
  ${!func.returns || func.returns === 'void' ? (
    `${func.name}(${func.params.map(genArgMassager).join(', ')});\n  m3ApiSuccess();`
  ) : ''}
}
`
  })
}

// the connector that hooks your imports to WASM
function getImportsConnector() {
  return funcs.filter(noFilter).map(func => {
    const o = func.returns && func.returns !== 'void' ? 'i' : 'v'
    const p = func.params.map(() => 'i').join('')
    return `m3_LinkRawFunction(module, "env", "null0_${func.name}", "${o}(${p})", &null0_${func.name});`
  })
}

let out = `// null0 host C header, generated ${(new Date()).toISOString()}

#include <sys/time.h>

#include "wasm3.h"
#include "m3_env.h"
#include "physfs.h"

// put these in the file that imports this
// #define RIMAGE_IMPLEMENTATION
// #include "rimage.h"

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
    fprintf(stderr, "%s - %s\\n", error.result, error.message);
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
// message/filename is WTF-16 encoded, which is a bit annoying
static m3ApiRawFunction (null0_abort) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\\n", message, fileName, lineNumber, columnNumber);
  m3ApiSuccess();
}

// Log a string
static m3ApiRawFunction (null0_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\\n", message);
  m3ApiSuccess();
}

${getImports().join('\n')}

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
  m3_LinkRawFunction(module, "env", "null0_log", "v(*)", &null0_log);
  ${getImportsConnector().join('\n  ')}

  null0_check_wasm3_is_ok();

  // EXPORTS
  m3_FindFunction(&new_func, runtime, "__new");
  m3_FindFunction(&cart_load, runtime, "load");
  m3_FindFunction(&cart_update, runtime, "update");

  null0_check_wasm3_is_ok();

  if (!new_func) {
    // this means no string-returns
    // it should be exported: __new(size: usize, id: u32 (1)): usize
    printf("no __new.\\n");
  }

  if (cart_load) {
    null0_check_wasm3(m3_CallV(cart_load));
  } else {
    printf("no load.\\n");
  }

  if (!cart_update) {
    printf("no update.\\n");
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

`

console.log(out)
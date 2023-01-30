// this is the host-side header for null0

// TODO: should we do argc/argv for carts?

// TODO: add WASI adapters, like log, etc - see https://github.com/wasm3/wasm3/blob/main/docs/Cookbook.md

// TODO: finish sound https://github.com/mackron/miniaudio/blob/master/examples/engine_sdl.c

// TODO: add mod/xm support

// TODO: sf2/midi?

// TODO: check which of these I am actually using
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"

#include "m3_env.h"
#include "physfs.h"
#include "wasm3.h"

#define MA_NO_DEVICE_IO
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

static ma_engine g_engine;
static ma_sound g_sound;

enum Null0CartType {
  Null0CartTypeInvalid,
  Null0CartTypeDir,
  Null0CartTypeZip,
  Null0CartTypeWasm,
};

struct Null0Image {
  uint8_t height;
  uint8_t width;
};
typedef struct Null0Image Null0Image;

static M3Environment* env;
static M3Runtime* runtime;
static M3Module* module;
static M3Function* cart_load;
static M3Function* cart_update;
static M3Function* cart_unload;

struct timespec startTime;
struct timespec nowTime;
pntr_image* null0_screen_image;
u8 currentImage = 0;
pntr_image* allImages[255];

bool FileExistsInPhysFS(const char* fileName) {
  PHYSFS_Stat stat;
  if (PHYSFS_stat(fileName, &stat) == 0) {
    return false;
  }
  return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead) {
  if (!FileExistsInPhysFS(fileName)) {
    fprintf(stderr, "file: Tried to load non-existing file '%s'\n", fileName);
    bytesRead = 0;
    return 0;
  }

  // Open up the file.
  void* handle = PHYSFS_openRead(fileName);
  if (handle == 0) {
    fprintf(stderr, "file: Could not open file '%s'\n", fileName);
    bytesRead = 0;
    return 0;
  }

  // Check to see how large the file is.
  int size = PHYSFS_fileLength(handle);
  if (size == -1) {
    bytesRead = 0;
    PHYSFS_close(handle);
    fprintf(stderr, "file: Cannot determine size of file '%s'\n", fileName);
    return 0;
  }

  // Close safely when it's empty.
  if (size == 0) {
    PHYSFS_close(handle);
    bytesRead = 0;
    return 0;
  }

  // Read the file, return if it's empty.
  void* buffer = malloc(size);
  int read = PHYSFS_readBytes(handle, buffer, size);
  if (read < 0) {
    bytesRead = 0;
    free(buffer);
    PHYSFS_close(handle);
    fprintf(stderr, "file: Cannot read bytes '%s'\n", fileName);
    return 0;
  }

  // Close the file handle, and return the bytes read and the buffer.
  PHYSFS_close(handle);
  *bytesRead = read;
  return buffer;
}

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3(M3Result result) {
  if (result) {
    M3ErrorInfo info;
    m3_GetErrorInfo(runtime, &info);
    fprintf(stderr, "%s - %s\n", result, info.message);
    exit(1);
  }
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok() {
  M3ErrorInfo error;
  m3_GetErrorInfo(runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
    exit(1);
  }
}

// Log a string
static m3ApiRawFunction(null0_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript's abort(), but it's utf8
static m3ApiRawFunction(null0_fatal) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  exit(1);
}

// exposed to wasm to seed random-generator
static m3ApiRawFunction(null0_seed) {
  m3ApiReturnType(double);
  m3ApiReturn(startTime.tv_nsec);
}

static m3ApiRawFunction(null0_clear_screen) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_image* c = pntr_gen_image_color(320, 240, *color);
  pntr_draw_image(allImages[destination], c, 0, 0);
  pntr_unload_image(c);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_gen_image_color) {
  m3ApiReturnType(u8);
  m3ApiGetArg(u8, width);
  m3ApiGetArg(u8, height);
  m3ApiGetArgMem(pntr_color*, color);

  currentImage++;
  allImages[currentImage] = pntr_gen_image_color(width, height, *color);

  m3ApiReturn(currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_draw_image) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(u8, i);
  m3ApiGetArg(i32, x);
  m3ApiGetArg(i32, y);

  pntr_draw_image(allImages[destination], allImages[i], x, y);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_draw_pixel) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_draw_pixel(allImages[destination], x, y, *color);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_draw_rectangle) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, width);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_draw_rectangle(allImages[destination], x, y, height, width, *color);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_draw_circle) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, centerX);
  m3ApiGetArg(int, centerY);
  m3ApiGetArg(int, radius);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_draw_circle(allImages[destination], centerX, centerY, radius, *color);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_load_image) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, fileName);

  unsigned int bytesRead = 0;
  unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);

  if (bytesRead) {
    currentImage++;
    allImages[currentImage] = pntr_load_image_from_memory(fileData, bytesRead);
  }

  m3ApiReturn(currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_file_read) {
  m3ApiReturnType(uint32_t);
  m3ApiGetArgMem(const char*, fileName);

  // example of what you want to return
  char* buffer = "Hello from C host";

  printf("TODO: read %s\n", fileName);

  m3ApiReturn(0);
}

// given a filename and some bytes (at least 4) this will tell you what type the cart is (dir/wasm/zip/invalid)
enum Null0CartType null0_get_cart_type(char* filename, u8* bytes, u32 byteLength) {
  DIR* dirptr;
  if (access(filename, F_OK) != -1) {
    if ((dirptr = opendir(filename)) != NULL) {
      return Null0CartTypeDir;
    } else {
      // printf("%d %d %d %d\n", bytes[0], bytes[1], bytes[2], bytes[3]);

      if (bytes[0] == 80 && bytes[1] == 75 && bytes[2] == 3 && bytes[3] == 4) {
        return Null0CartTypeZip;
      }
      if (bytes[0] == 0 && bytes[1] == 97 && bytes[2] == 115 && bytes[3] == 109) {
        return Null0CartTypeWasm;
      }
      return Null0CartTypeInvalid;
    }
  } else {
    return Null0CartTypeInvalid;
  }
}

// call this to get current sound loaded into buffer
void null0_get_audio(ma_uint8* pBuffer, int bufferSizeInBytes) {
  ma_uint32 bufferSizeInFrames = (ma_uint32)bufferSizeInBytes / ma_get_bytes_per_frame(ma_format_f32, ma_engine_get_channels(&g_engine));
  ma_engine_read_pcm_frames(&g_engine, pBuffer, bufferSizeInFrames, NULL);
}

// call cart's update(): run this in your game-loop
void null0_update() {
  clock_gettime(CLOCK_MONOTONIC_RAW, &nowTime);
  uint64_t delta = ((nowTime.tv_sec - startTime.tv_sec) * 1000000) + ((nowTime.tv_nsec - startTime.tv_nsec) / 1000);
  if (cart_update) {
    null0_check_wasm3(m3_CallV(cart_update, delta));
  }
}

// call this when you close
void null0_unload() {
  if (cart_unload) {
    null0_check_wasm3(m3_CallV(cart_unload));
  }
  pntr_unload_image(null0_screen_image);
}

// given a filename, byte-array and length of cart file, this will load up wasm environment for it
int null0_load_cart_wasm(char* filename, u8* wasmBuffer, u32 byteLength) {
  enum Null0CartType t = null0_get_cart_type(filename, wasmBuffer, byteLength);

  if (t == Null0CartTypeInvalid) {
    fprintf(stderr, "invalid cart.\n");
    return 1;
  }

  // TODO: init neesd a string of "who are you?", not sure if this is right way to do that
  PHYSFS_init(filename);
  env = m3_NewEnvironment();
  runtime = m3_NewRuntime(env, 1024 * 1024, NULL);

  if (t == Null0CartTypeDir || t == Null0CartTypeZip) {
    PHYSFS_mount(filename, NULL, 0);

    if (!FileExistsInPhysFS("main.wasm")) {
      fprintf(stderr, "No main.wasm.\n");
      return 1;
    }

    PHYSFS_File* wasmFile = PHYSFS_openRead("main.wasm");
    PHYSFS_uint64 wasmLen = PHYSFS_fileLength(wasmFile);
    u8* wasmBufferP[wasmLen];
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(wasmFile, wasmBufferP, wasmLen);
    PHYSFS_close(wasmFile);

    if (bytesRead == -1) {
      fprintf(stderr, "Could not read main.wasm.\n");
      return 1;
    }

    null0_check_wasm3(m3_ParseModule(env, &module, (u8*)wasmBufferP, wasmLen));
  } else if (t == Null0CartTypeWasm) {
    null0_check_wasm3(m3_ParseModule(env, &module, wasmBuffer, byteLength));
  }

  null0_check_wasm3(m3_LoadModule(runtime, module));

  // add the writable dir overlay to root
  PHYSFS_mount(PHYSFS_getWriteDir(), NULL, 0);

  // IMPORTS (to wasm)
  m3_LinkRawFunction(module, "env", "null0_log", "v(*)", &null0_log);
  m3_LinkRawFunction(module, "env", "null0_fatal", "v(**ii)", &null0_fatal);
  m3_LinkRawFunction(module, "env", "seed", "F()", &null0_seed);

  m3_LinkRawFunction(module, "env", "null0_clear_screen", "v(i*)", &null0_clear_screen);
  m3_LinkRawFunction(module, "env", "null0_draw_image", "v(i*ii)", &null0_draw_image);
  m3_LinkRawFunction(module, "env", "null0_draw_pixel", "v(iii*)", &null0_draw_pixel);
  m3_LinkRawFunction(module, "env", "null0_draw_rectangle", "v(iiiii*)", &null0_draw_rectangle);
  m3_LinkRawFunction(module, "env", "null0_draw_circle", "v(iiii*)", &null0_draw_circle);
  m3_LinkRawFunction(module, "env", "null0_load_image", "i(*)", &null0_load_image);
  m3_LinkRawFunction(module, "env", "null0_gen_image_color", "i(ii*)", &null0_gen_image_color);
  m3_LinkRawFunction(module, "env", "null0_file_read", "*(*)", &null0_file_read);

  null0_check_wasm3_is_ok();

  // EXPORTS (from wasm)
  m3_FindFunction(&cart_load, runtime, "load");
  m3_FindFunction(&cart_update, runtime, "update");
  m3_FindFunction(&cart_unload, runtime, "unload");

  null0_check_wasm3_is_ok();

  clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);
  null0_screen_image = pntr_gen_image_color(320, 240, PNTR_BLACK);
  allImages[0] = null0_screen_image;

  ma_result result;
  ma_engine_config engineConfig;

  engineConfig = ma_engine_config_init();
  engineConfig.noDevice = MA_TRUE;
  engineConfig.channels = 2;
  engineConfig.sampleRate = 48000;

  result = ma_engine_init(&engineConfig, &g_engine);
  if (result != MA_SUCCESS) {
    fprintf(stderr, "Failed to initialize audio engine.\n");
    return 1;
  }

  if (cart_load) {
    null0_check_wasm3(m3_CallV(cart_load));
  } else {
    fprintf(stderr, "no load() in cart.\n");
  }

  return 0;
}

// given a filename, this will load up wasm environment for it
int null0_load_cart(char* filename) {
  u8* wasm = NULL;
  u32 fsize = 0;

  FILE* f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Cannot open file.\n");
    return 1;
  }
  fseek(f, 0, SEEK_END);
  fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  wasm = (u8*)malloc(fsize);
  if (!wasm) {
    fprintf(stderr, "cannot allocate memory for wasm binary\n");
    return 1;
  }

  if (fread(wasm, 1, fsize, f) != fsize) {
    fprintf(stderr, "cannot read file\n");
    return 1;
  }
  fclose(f);
  f = NULL;

  return null0_load_cart_wasm(filename, wasm, fsize);
}
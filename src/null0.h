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

#define PNTR_PIXELFORMAT_ARGB
#define PNTR_IMPLEMENTATION
#include "pntr.h"

#include "m3_env.h"
#include "physfs.h"
#include "rl_config.h"
#include "rl_sound.h"
#include "wasm3.h"

enum Null0CartType {
  Null0CartTypeInvalid,
  Null0CartTypeDir,
  Null0CartTypeZip,
  Null0CartTypeWasm,
};
typedef enum Null0CartType Null0CartType;

struct Null0State {
  M3Environment* env;
  M3Runtime* runtime;
  M3Module* module;

  M3Function* cart_load;
  M3Function* cart_update;
  M3Function* cart_unload;

  u8 currentImage;
  pntr_image* images[255];

  u8 currentSound;
  rl_sound_t* sounds[255];

  struct timespec startTime;
  struct timespec nowTime;
};
typedef struct Null0State Null0State;

Null0State null0_state;

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3(M3Result result) {
  if (result) {
    M3ErrorInfo info;
    m3_GetErrorInfo(null0_state.runtime, &info);
    fprintf(stderr, "%s - %s\n", result, info.message);
    exit(1);
  }
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok() {
  M3ErrorInfo error;
  m3_GetErrorInfo(null0_state.runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
    exit(1);
  }
}

bool null0_file_exists(const char* fileName) {
  PHYSFS_Stat stat;
  if (PHYSFS_stat(fileName, &stat) == 0) {
    return false;
  }
  return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

unsigned char* null0_file_read(const char* fileName, unsigned int* bytesRead) {
  if (!null0_file_exists(fileName)) {
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

// given a filename and some bytes (at least 4) this will tell you what type the cart is (dir/wasm/zip/invalid)
enum Null0CartType null0_get_cart_type(char* filename, u8* bytes, u32 byteLength) {
  DIR* dirptr;
  if (access(filename, F_OK) != -1) {
    if ((dirptr = opendir(filename)) != NULL) {
      return Null0CartTypeDir;
    } else {
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

// Log a string
static m3ApiRawFunction(null0_import_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript's abort(), but it's utf8
static m3ApiRawFunction(null0_import_fatal) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, fileName);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, fileName, lineNumber, columnNumber);
  exit(1);
}

// exposed to wasm to seed random-generator
static m3ApiRawFunction(null0_import_seed) {
  m3ApiReturnType(double);
  m3ApiReturn(null0_state.startTime.tv_nsec);
}

static m3ApiRawFunction(null0_import_clear_screen) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_image* c = pntr_gen_image_color(320, 240, *color);
  pntr_draw_image(null0_state.images[destination], c, 0, 0);
  pntr_unload_image(c);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_gen_image_color) {
  m3ApiReturnType(u8);
  m3ApiGetArg(u8, width);
  m3ApiGetArg(u8, height);
  m3ApiGetArgMem(pntr_color*, color);

  null0_state.currentImage++;
  null0_state.images[null0_state.currentImage] = pntr_gen_image_color(width, height, *color);

  m3ApiReturn(null0_state.currentImage);
  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_image) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(u8, source);
  m3ApiGetArg(i32, x);
  m3ApiGetArg(i32, y);

  pntr_draw_image(null0_state.images[destination], null0_state.images[source], x, y);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_pixel) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_draw_pixel(null0_state.images[destination], x, y, *color);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_rectangle) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, width);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_draw_rectangle(null0_state.images[destination], x, y, height, width, *color);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_draw_circle) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, centerX);
  m3ApiGetArg(int, centerY);
  m3ApiGetArg(int, radius);
  m3ApiGetArgMem(pntr_color*, color);

  pntr_draw_circle(null0_state.images[destination], centerX, centerY, radius, *color);

  m3ApiSuccess();
}

static m3ApiRawFunction(null0_import_load_image) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, fileName);

  unsigned int bytesRead = 0;
  unsigned char* fileData = null0_file_read(fileName, &bytesRead);

  if (bytesRead) {
    null0_state.currentImage++;
    null0_state.images[null0_state.currentImage] = pntr_load_image_from_memory(fileData, bytesRead);
  }

  m3ApiReturn(null0_state.currentImage);
  m3ApiSuccess();
}

// Load a sound
static m3ApiRawFunction(null0_import_load_sound) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, fileName);

  // rl_sound_load(null0_state.sounds[null0_state.currentSound++], fileName);
  printf("load %s\n", fileName);

  m3ApiReturn(null0_state.currentSound);
  m3ApiSuccess();
}

// Load a sound-effect
static m3ApiRawFunction(null0_import_load_sfx) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, fileName);

  // rl_sound_sfxr_load(null0_state.sounds[null0_state.currentSound++], fileName);
  printf("sfx %s\n", fileName);

  m3ApiReturn(null0_state.currentSound);
  m3ApiSuccess();
}

// Play a sound
static m3ApiRawFunction(null0_import_play_sound) {
  m3ApiGetArg(u8, source);
  m3ApiGetArg(float, volume);
  m3ApiGetArg(int, repeat);

  rl_sound_play(null0_state.sounds[source], volume, repeat);
  printf("play %d %f %d\n", source, volume, repeat);

  m3ApiSuccess();
}

// call cart's update(): run this in your game-loop
void null0_update() {
  clock_gettime(CLOCK_MONOTONIC_RAW, &null0_state.nowTime);
  uint64_t delta = ((null0_state.nowTime.tv_sec - null0_state.startTime.tv_sec) * 1000000) + ((null0_state.nowTime.tv_nsec - null0_state.startTime.tv_nsec) / 1000);
  if (null0_state.cart_update) {
    null0_check_wasm3(m3_CallV(null0_state.cart_update, delta));
  }
}

// called to unload game
void null0_unload() {
  if (null0_state.cart_unload) {
    null0_check_wasm3(m3_CallV(null0_state.cart_unload));
  }
  for (int i = 0; i < 255; i++) {
    if (null0_state.images[i] != NULL) {
      pntr_unload_image(null0_state.images[i]);
    }
    if (null0_state.sounds[i] != NULL) {
      rl_sound_destroy(null0_state.sounds[i]);
    }
  }
}
// called when there is no cart
int null0_load_empty() {
  return 0;
}

// load a cart from bytes
int null0_load_memory(char* filename, u8* wasmBuffer, u32 byteLength) {
  enum Null0CartType t = null0_get_cart_type(filename, wasmBuffer, byteLength);
  if (t == Null0CartTypeInvalid) {
    fprintf(stderr, "invalid cart.\n");
    return 1;
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &null0_state.startTime);
  null0_state.currentImage = 0;
  null0_state.currentSound = 0;

  for (int i = 0; i < 255; i++) {
    null0_state.images[i] = NULL;
    null0_state.sounds[i] = NULL;
  }

  null0_state.images[0] = pntr_gen_image_color(320, 240, PNTR_BLACK);

  PHYSFS_init(filename);
  rl_sound_init();

  null0_state.env = m3_NewEnvironment();
  null0_state.runtime = m3_NewRuntime(null0_state.env, 1024 * 1024, NULL);
  if (t == Null0CartTypeDir || t == Null0CartTypeZip) {
    PHYSFS_mount(filename, NULL, 0);

    if (!null0_file_exists("main.wasm")) {
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

    null0_check_wasm3(m3_ParseModule(null0_state.env, &null0_state.module, (u8*)wasmBufferP, wasmLen));
  } else if (t == Null0CartTypeWasm) {
    null0_check_wasm3(m3_ParseModule(null0_state.env, &null0_state.module, wasmBuffer, byteLength));
  }

  null0_check_wasm3(m3_LoadModule(null0_state.runtime, null0_state.module));

  // add the writable dir overlay to root
  PHYSFS_mount(PHYSFS_getWriteDir(), NULL, 0);

  // IMPORTS (to wasm)
  m3_LinkRawFunction(null0_state.module, "env", "null0_log", "v(*)", &null0_import_log);
  m3_LinkRawFunction(null0_state.module, "env", "null0_fatal", "v(**ii)", &null0_import_fatal);
  m3_LinkRawFunction(null0_state.module, "env", "seed", "F()", &null0_import_seed);
  m3_LinkRawFunction(null0_state.module, "env", "null0_clear_screen", "v(i*)", &null0_import_clear_screen);
  m3_LinkRawFunction(null0_state.module, "env", "null0_draw_image", "v(i*ii)", &null0_import_draw_image);
  m3_LinkRawFunction(null0_state.module, "env", "null0_draw_pixel", "v(iii*)", &null0_import_draw_pixel);
  m3_LinkRawFunction(null0_state.module, "env", "null0_draw_rectangle", "v(iiiii*)", &null0_import_draw_rectangle);
  m3_LinkRawFunction(null0_state.module, "env", "null0_draw_circle", "v(iiii*)", &null0_import_draw_circle);
  m3_LinkRawFunction(null0_state.module, "env", "null0_load_image", "i(*)", &null0_import_load_image);
  m3_LinkRawFunction(null0_state.module, "env", "null0_gen_image_color", "i(ii*)", &null0_import_gen_image_color);
  m3_LinkRawFunction(null0_state.module, "env", "null0_load_sound", "i(*)", &null0_import_load_sound);
  m3_LinkRawFunction(null0_state.module, "env", "null0_load_sfx", "i(*)", &null0_import_load_sfx);
  m3_LinkRawFunction(null0_state.module, "env", "null0_play_sound", "v(ifi)", &null0_import_play_sound);
  // m3_LinkRawFunction(null0_state.module, "env", "null0_file_read", "*(*)", &null0_import_file_read);

  // exports from wasm
  m3_FindFunction(&null0_state.cart_load, null0_state.runtime, "load");
  m3_FindFunction(&null0_state.cart_unload, null0_state.runtime, "unload");
  m3_FindFunction(&null0_state.cart_update, null0_state.runtime, "update");

  null0_check_wasm3_is_ok();

  if (null0_state.cart_load) {
    null0_check_wasm3(m3_CallV(null0_state.cart_load));
  } else {
    fprintf(stderr, "no load() in cart.\n");
  }

  return 0;
}

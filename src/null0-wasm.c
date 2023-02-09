#include "null0-wasm.h"

// all wasm3 functions return same sort of error-pattern, so this wraps that
static void null0_check_wasm3(M3Result result) {
  if (result) {
    M3ErrorInfo info;
    m3_GetErrorInfo(null0.runtime, &info);
    fprintf(stderr, "%s - %s\n", result, info.message);
    exit(1);
  }
}

// this checks the general state of the runtime, to make sure there are no errors lingering
static void null0_check_wasm3_is_ok() {
  M3ErrorInfo error;
  m3_GetErrorInfo(null0.runtime, &error);
  if (error.result) {
    fprintf(stderr, "%s - %s\n", error.result, error.message);
    exit(1);
  }
}

// allocate WASM memory & copy value in
// TODO: not currently working, I think _mem is wrong
// TODO: support other export name-conventions like emscripten
u32 wmcopy(void* pointer, size_t size) {
  M3Memory* _mem = &null0.runtime->memory;

  if (!null0._pin) {
    fprintf(stderr, "__pin does not exist.\n");
    return 0;
  }
  if (!null0._new) {
    fprintf(stderr, "__new does not exist.\n");
    return 0;
  }

  u32 wPointer;
  null0_check_wasm3(m3_CallV(null0._new, size, 1));
  m3_GetResultsV(null0._new, &wPointer);
  null0_check_wasm3(m3_CallV(null0._pin, wPointer));
  void* wBuffer = m3ApiOffsetToPtr(wPointer);
  memcpy(wBuffer, pointer, size);

  return wPointer;
}

// free WASM memory
// TODO: support other export name-conventions like emscripten
void wfree(u32 pointer) {
  if (!null0._unpin) {
    fprintf(stderr, "__unpin does not exist.\n");
    return;
  }
  null0_check_wasm3(m3_CallV(null0._unpin, pointer));
}

// IMPORT: Log a string
static m3ApiRawFunction(null0_import_log) {
  m3ApiGetArgMem(const char*, message);
  printf("%s\n", message);
  m3ApiSuccess();
}

// IMPORT: Fatal error - call this from your code on a fatal runtime error, similar to assemblyscript's abort(), but it's utf8
static m3ApiRawFunction(null0_import_fatal) {
  m3ApiGetArgMem(const char*, message);
  m3ApiGetArgMem(const char*, filename);
  m3ApiGetArg(uint16_t, lineNumber);
  m3ApiGetArg(uint16_t, columnNumber);
  fprintf(stderr, "%s at %s:%d:%d\n", message, filename, lineNumber, columnNumber);
  exit(1);
}

// IMPORT: exposed to wasm to seed random-generator
static m3ApiRawFunction(null0_import_seed) {
  m3ApiReturnType(double);
  m3ApiReturn(null0.startTime.tv_nsec);
}

// IMPORT: clear screen
static m3ApiRawFunction(null0_import_clear_screen) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArgMem(pntr_color*, color);

  if (null0.images[destination] != NULL) {
    pntr_image* c = pntr_gen_image_color(320, 240, *color);
    pntr_draw_image(null0.images[destination], c, 0, 0);
    pntr_unload_image(c);
  }

  m3ApiSuccess();
}

// IMPORT: generate a color image
static m3ApiRawFunction(null0_import_gen_image_color) {
  m3ApiReturnType(u8);
  m3ApiGetArg(u8, width);
  m3ApiGetArg(u8, height);
  m3ApiGetArgMem(pntr_color*, color);

  null0.currentImage++;
  null0.images[null0.currentImage] = pntr_gen_image_color(width, height, *color);

  m3ApiReturn(null0.currentImage);
  m3ApiSuccess();
}

// IMPORT: draw an image on another image
static m3ApiRawFunction(null0_import_draw_image) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(u8, source);
  m3ApiGetArg(i32, x);
  m3ApiGetArg(i32, y);

  if (null0.images[destination] != NULL) {
    pntr_draw_image(null0.images[destination], null0.images[source], x, y);
  }

  m3ApiSuccess();
}

// IMPORT: draw a pixel on an image
static m3ApiRawFunction(null0_import_draw_pixel) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArgMem(pntr_color*, color);

  if (null0.images[destination] != NULL) {
    pntr_draw_pixel(null0.images[destination], x, y, *color);
  }

  m3ApiSuccess();
}

// IMPORT: draw a rectangle on an image
static m3ApiRawFunction(null0_import_draw_rectangle) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, x);
  m3ApiGetArg(int, y);
  m3ApiGetArg(int, height);
  m3ApiGetArg(int, width);
  m3ApiGetArgMem(pntr_color*, color);

  if (null0.images[destination] != NULL) {
    pntr_draw_rectangle(null0.images[destination], x, y, height, width, *color);
  }
  m3ApiSuccess();
}

// IMPORT: draw a circle on an image
static m3ApiRawFunction(null0_import_draw_circle) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, centerX);
  m3ApiGetArg(int, centerY);
  m3ApiGetArg(int, radius);
  m3ApiGetArgMem(pntr_color*, color);

  if (null0.images[destination] != NULL) {
    pntr_draw_circle(null0.images[destination], centerX, centerY, radius, *color);
  }

  m3ApiSuccess();
}

// IMPORT: load an image from physfs
static m3ApiRawFunction(null0_import_load_image) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);

  unsigned int bytesRead = 0;
  unsigned char* fileData = null0_fs_file_read(filename, &bytesRead);

  if (bytesRead) {
    null0.currentImage++;
    null0.images[null0.currentImage] = pntr_load_image_from_memory(fileData, bytesRead);
    m3ApiReturn(null0.currentImage);
  } else {
    m3ApiReturn(0);
  }
  m3ApiSuccess();
}

// IMPORT read a file
static m3ApiRawFunction(null0_import_file_read) {
  m3ApiReturnType(u32);
  m3ApiGetArgMem(const char*, filename);

  unsigned int bytesRead = 0;
  unsigned char* fileData = null0_fs_file_read(filename, &bytesRead);

  if (bytesRead && null0._new && null0._pin) {
    u32 wPointer;
    null0_check_wasm3(m3_CallV(null0._new, bytesRead, 1));
    m3_GetResultsV(null0._new, &wPointer);
    null0_check_wasm3(m3_CallV(null0._pin, wPointer));
    memcpy(m3ApiOffsetToPtr(wPointer), fileData, bytesRead);
    m3ApiReturn(wPointer);
  }

  m3ApiSuccess();
}

// IMPORT write a file
static m3ApiRawFunction(null0_import_file_write) {
  m3ApiGetArgMem(const char*, filename);
  m3ApiGetArgMem(const char*, content);
  m3ApiGetArg(u32, size);

  unsigned int bytesWritten = null0_fs_file_write(filename, content, size);

  m3ApiSuccess();
}

// IMPORT check if file exists
static m3ApiRawFunction(null0_import_file_exists) {
  m3ApiReturnType(int);
  m3ApiGetArgMem(const char*, filename);
  m3ApiReturn(null0_fs_exists(filename) ? 1 : 0);
  m3ApiSuccess();
}

// IMPORT create a speech-player
static m3ApiRawFunction(null0_import_sound_create_speech) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, text);
  null0.currentSound++;
  null0.sounds[null0.currentSound] = Speech_create();
  Speech_setText(null0.sounds[null0.currentSound], text);
  m3ApiReturn(null0.currentSound);
  m3ApiSuccess();
}

// IMPORT create a mod-player
// TODO: not working on libretro, maybe due to dlopen stuff. Try to static link openmpt and make a custom soloud AudioSource
static m3ApiRawFunction(null0_import_sound_create_mod) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);

  unsigned int bytesRead = 0;
  unsigned char* fileData = null0_fs_file_read(filename, &bytesRead);

  if (bytesRead) {
    null0.currentSound++;
    null0.sounds[null0.currentSound] = Openmpt_create();
    int res = Openmpt_loadMem(null0.sounds[null0.currentSound], fileData, bytesRead);
    if (res != 0) {
      printf("%s: %d - %s\n", filename, bytesRead, Soloud_getErrorString(null0.soloud, res));
    }
    m3ApiReturn(null0.currentSound);
  }

  m3ApiSuccess();
}

// IMPORT create a wav-player
static m3ApiRawFunction(null0_import_sound_create_wav) {
  m3ApiReturnType(u8);
  m3ApiGetArgMem(const char*, filename);

  unsigned int bytesRead = 0;
  unsigned char* fileData = null0_fs_file_read(filename, &bytesRead);

  if (bytesRead) {
    null0.currentSound++;
    null0.sounds[null0.currentSound] = Wav_create();
    int res = Wav_loadMem(null0.sounds[null0.currentSound], fileData, bytesRead);
    if (res != 0) {
      printf("%s: %d - %s\n", filename, bytesRead, Soloud_getErrorString(null0.soloud, res));
    }
    m3ApiReturn(null0.currentSound);
  }

  m3ApiSuccess();
}

// IMPORT set text of speech-player
static m3ApiRawFunction(null0_import_sound_speech_settext) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArgMem(const char*, text);
  Speech_setText(null0.sounds[destination], text);
  m3ApiSuccess();
}

// IMPORT play a sound source
static m3ApiRawFunction(null0_import_sound_play) {
  m3ApiGetArg(u8, destination);
  m3ApiGetArg(int, looping);
  int voice = Soloud_play(null0.soloud, null0.sounds[destination]);
  if (looping != 0) {
    Soloud_setLooping(null0.soloud, voice, true);
  }
  m3ApiSuccess();
}

// call this to update runtime (calls cart's update)
void null0_wasm_update() {
  clock_gettime(CLOCK_MONOTONIC_RAW, &null0.nowTime);
  // TODO: is this a good resolution for time? is it safe (in terms of timing attacks?)
  uint64_t delta = ((null0.nowTime.tv_sec - null0.startTime.tv_sec) * 1000000) + ((null0.nowTime.tv_nsec - null0.startTime.tv_nsec) / 1000);
  if (null0.cart_update) {
    null0_check_wasm3(m3_CallV(null0.cart_update, delta));
  }
}

// call this on unload (calls cart's unload)
void null0_wasm_unload() {
  if (null0.cart_unload) {
    null0_check_wasm3(m3_CallV(null0.cart_unload));
  }
  for (int i = 0; i <= null0.currentImage; i++) {
    pntr_unload_image(null0.images[i]);
  }
  // for (int i = 0; i <= null0.currentSound; i++) {
  //   free(null0.sounds[i]);
  // }

  Soloud_deinit(null0.soloud);
  Soloud_destroy(null0.soloud);
}

// load wasm runtime
bool null0_wasm_init(Null0CartData cart) {
  null0.module = NULL;
  null0.env = m3_NewEnvironment();
  null0.runtime = m3_NewRuntime(null0.env, 1024 * 1024, NULL);
  null0_check_wasm3(m3_ParseModule(null0.env, &null0.module, (u8*)cart.bytes, cart.size));
  null0_check_wasm3(m3_LoadModule(null0.runtime, null0.module));

  free(cart.bytes);

  null0.currentImage = 0;
  null0.images[0] = pntr_gen_image_color(320, 240, PNTR_BLACK);
  clock_gettime(CLOCK_MONOTONIC_RAW, &null0.startTime);
  null0.soloud = Soloud_create();

  null0.images[0] = pntr_gen_image_color(320, 240, PNTR_BLACK);

#ifdef NULL0_NULL_SOUND
  Soloud_initEx(
      null0.soloud,
      SOLOUD_CLIP_ROUNDOFF,
      SOLOUD_NULLDRIVER,
      NULL0_SAMPLE_RATE,
      SOLOUD_AUTO,
      2);
#else
  Soloud_initEx(
      null0.soloud,
      SOLOUD_CLIP_ROUNDOFF,
      SOLOUD_AUTO,
      NULL0_SAMPLE_RATE,
      SOLOUD_AUTO,
      2);
#endif

  // IMPORTS (to wasm)
  m3_LinkRawFunction(null0.module, "env", "null0_log", "v(*)", &null0_import_log);
  m3_LinkRawFunction(null0.module, "env", "null0_fatal", "v(**ii)", &null0_import_fatal);
  m3_LinkRawFunction(null0.module, "env", "seed", "F()", &null0_import_seed);

  m3_LinkRawFunction(null0.module, "env", "null0_clear_screen", "v(i*)", &null0_import_clear_screen);
  m3_LinkRawFunction(null0.module, "env", "null0_gen_image_color", "i(ii*)", &null0_import_gen_image_color);
  m3_LinkRawFunction(null0.module, "env", "null0_draw_image", "v(i*ii)", &null0_import_draw_image);
  m3_LinkRawFunction(null0.module, "env", "null0_draw_pixel", "v(iii*)", &null0_import_draw_pixel);
  m3_LinkRawFunction(null0.module, "env", "null0_draw_rectangle", "v(iiiii*)", &null0_import_draw_rectangle);
  m3_LinkRawFunction(null0.module, "env", "null0_draw_circle", "v(iiii*)", &null0_import_draw_circle);
  m3_LinkRawFunction(null0.module, "env", "null0_load_image", "i(*)", &null0_import_load_image);

  m3_LinkRawFunction(null0.module, "env", "null0_file_read", "i(*)", &null0_import_file_read);
  m3_LinkRawFunction(null0.module, "env", "null0_file_write", "v(**i)", &null0_import_file_write);
  m3_LinkRawFunction(null0.module, "env", "null0_file_exists", "i(*)", &null0_import_file_exists);

  m3_LinkRawFunction(null0.module, "env", "null0_create_speech", "i(*)", &null0_import_sound_create_speech);
  m3_LinkRawFunction(null0.module, "env", "null0_speech_settext", "v(i*)", &null0_import_sound_speech_settext);
  m3_LinkRawFunction(null0.module, "env", "null0_create_mod", "i(*)", &null0_import_sound_create_mod);
  m3_LinkRawFunction(null0.module, "env", "null0_create_wav", "i(*)", &null0_import_sound_create_wav);
  m3_LinkRawFunction(null0.module, "env", "null0_sound_play", "v(ii)", &null0_import_sound_play);

  null0_check_wasm3_is_ok();

  // EXPORTS (from wasm)
  m3_FindFunction(&null0.cart_load, null0.runtime, "load");
  m3_FindFunction(&null0.cart_update, null0.runtime, "update");
  m3_FindFunction(&null0.cart_unload, null0.runtime, "unload");

  // these are assemblyscript exports, but we can implement similar for others
  m3_FindFunction(&null0._pin, null0.runtime, "__pin");
  m3_FindFunction(&null0._new, null0.runtime, "__new");
  m3_FindFunction(&null0._unpin, null0.runtime, "__unpin");

  null0_check_wasm3_is_ok();

  if (null0.cart_load) {
    null0_check_wasm3(m3_CallV(null0.cart_load));
  } else {
    fprintf(stderr, "no load() in cart.\n");
  }

  return true;
}
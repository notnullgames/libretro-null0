#define PNTR_IMPLEMENTATION
#include "pntr.h"

#include <time.h>
#include "m3_env.h"
#include "physfs.h"
#include "rl_sound.h"
#include "wasm3.h"

enum Null0CartType {
  Null0CartTypeInvalid,
  Null0CartTypeDir,
  Null0CartTypeZip,
  Null0CartTypeWasm,
};

struct Null0State {
  struct timespec start;
  struct timespec now;

  u8 image;
  pntr_image* images[255];

  u8 sound;
  rl_sound_t* sounds[255];

  M3Environment* env;
  M3Runtime* runtime;
  M3Module* module;
  M3Function* cart_load;
  M3Function* cart_update;
  M3Function* cart_unload;
};
typedef struct Null0State Null0State;

static Null0State* null0_state;
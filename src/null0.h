#ifndef NULL0_H
#define NULL0_H

#define BREAK asm("int $3");

// TODO: check these are all used & split them up into imports for each subsystem
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "m3_env.h"
#include "physfs.h"
#include "pntr.h"
#include "soloud_c.h"
#include "wasm3.h"

/* The number of video frames per second. */
#ifndef NULL0_FRAME_RATE
#define NULL0_FRAME_RATE 60
#endif

/* The frequency of the mixed sound. */
#ifndef NULL0_SAMPLE_RATE
#define NULL0_SAMPLE_RATE 48000
#endif

/* Number of 16-bit stereo samples per frame. DO NOT CHANGE! */
#define NULL0_SAMPLES_PER_FRAME (NULL0_SAMPLE_RATE / NULL0_FRAME_RATE)

struct Null0CartData {
  u8* bytes;
  u32 size;
  char* filename;
};
typedef struct Null0CartData Null0CartData;

/* The number of video frames per second. */
#ifndef NULL0_FRAME_RATE
#define NULL0_FRAME_RATE 60
#endif

/* The frequency of the mixed sound. */
#ifndef NULL0_SAMPLE_RATE
#define NULL0_SAMPLE_RATE 48000
#endif

/* Number of 16-bit stereo samples per frame. DO NOT CHANGE! */
#define NULL0_SAMPLES_PER_FRAME (NULL0_SAMPLE_RATE / NULL0_FRAME_RATE)

struct Null0State {
  M3Environment* env;
  M3Runtime* runtime;
  M3Module* module;

  M3Function* cart_load;
  M3Function* cart_update;
  M3Function* cart_unload;

  M3Function* _pin;
  M3Function* _new;
  M3Function* _unpin;

  u8 currentImage;
  pntr_image* images[255];

  u8 currentSound;
  AudioSource* sounds[255];

  int16_t audio_buffer[NULL0_SAMPLES_PER_FRAME * 2];
  Soloud* soloud;

  struct timespec startTime;
  struct timespec nowTime;

  bool ok;
};
typedef struct Null0State Null0State;

Null0State null0;

#include "null0-fs.h"
#include "null0-sound.h"
#include "null0-wasm.h"

#endif  // NULL0_H

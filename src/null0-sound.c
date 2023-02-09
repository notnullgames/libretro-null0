#import "null0-sound.h"

// call this in host to get current frame sound
const int16_t* null0_get_sound(void) {
  if (sizeof(short) == sizeof(int16_t)) {
    Soloud_mixSigned16(null0.soloud, null0.audio_buffer, NULL0_SAMPLES_PER_FRAME);
  } else if (sizeof(short) < sizeof(int16_t)) {
    short buf[NULL0_SAMPLES_PER_FRAME * 2];
    Soloud_mixSigned16(null0.soloud, buf, NULL0_SAMPLES_PER_FRAME);

    for (int i = 0; i < NULL0_SAMPLES_PER_FRAME * 2; i++) {
      null0.audio_buffer[i] = buf[i];
    }
  } else {
    memset(null0.audio_buffer, 0, sizeof(null0.audio_buffer));
  }

  return null0.audio_buffer;
}

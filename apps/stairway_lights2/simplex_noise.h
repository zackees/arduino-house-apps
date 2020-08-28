

#ifndef _SIMPLEX_NOISE_H_
#define _SIMPLEX_NOISE_H_

#include "FastLED.h"

struct NoiseGenerator {

  NoiseGenerator() {
    iteration_scale = 10;
    time_multiplier = 10;
  }

  NoiseGenerator (int32_t itScale, int32_t timeMul) : iteration_scale(itScale), time_multiplier(timeMul) {
  }
  // i is the position of the LED
  uint8_t Value(int32_t i, unsigned long time_ms) {
    uint32_t input = iteration_scale*i + time_ms * time_multiplier;
     
    uint16_t v1 = inoise16(input);
    return uint8_t(v1 >> 8);
  }

  int LedValue(int32_t i, unsigned long time_ms) {
    int val = Value(i, time_ms);
    return max(0, val - 128) * 2;
  }

  int32_t iteration_scale;
  unsigned long time_multiplier;
};

#endif  // _SIMPLEX_NOISE_H_

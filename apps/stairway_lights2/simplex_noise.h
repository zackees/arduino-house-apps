

#ifndef _SIMPLEX_NOISE_H_
#define _SIMPLEX_NOISE_H_

#include <inttypes.h>

struct NoiseGenerator {
  NoiseGenerator() {
    iteration_scale = 10;
    time_multiplier = 10;
  }
  NoiseGenerator (int32_t itScale, int32_t timeMul) : iteration_scale(itScale), time_multiplier(timeMul) {
  }
  // i is the position of the LED
  uint8_t Value(int32_t i, unsigned long time_ms) const;
  int LedValue(int32_t i, unsigned long time_ms) const;
  int32_t iteration_scale;
  unsigned long time_multiplier;
};

#endif  // _SIMPLEX_NOISE_H_

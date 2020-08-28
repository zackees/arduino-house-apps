

#include "simplex_noise.h"
#include "FastLED.h"

#include <inttypes.h>


// i is the position of the LED
uint8_t NoiseGenerator::Value(int32_t i, unsigned long time_ms) const {
  uint32_t input = iteration_scale*i + time_ms * time_multiplier;
  uint16_t v1 = inoise16(input);
  return uint8_t(v1 >> 8);
}

int NoiseGenerator::LedValue(int32_t i, unsigned long time_ms) const {
  int val = Value(i, time_ms);
  return max(0, val - 128) * 2;
}
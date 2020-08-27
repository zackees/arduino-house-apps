#ifndef __ZACH_UTIL_H__
#define __ZACH_UTIL_H__

#include <Arduino.h>

inline float mapf(float t, float in_min, float in_max, float out_min, float out_max) {
  return (t - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif  // __ZACH_UTIL_H__


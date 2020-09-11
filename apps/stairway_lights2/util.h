
#ifndef _UTIL_H_
#define _UTIL_H_

inline float mapf(float t, float in_min, float in_max, float out_min, float out_max) {
  return (t - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline float clampf(float val, float min, float max) {
    if (val > max) { return max; }
    if (val < min) { return min; }
    return val;
}

#endif  // _UTIL_H_



#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "defs.h"

void sensors_init();

// A timer which is active for the duration input, then turns off.
class DurationTimer {
 public:
  DurationTimer() : mStartTime(0), mDurationMs(0) {}
  DurationTimer(unsigned long duration) :  mStartTime(0), mDurationMs(0) { Restart(duration); }
  void Restart(unsigned long duration_ms);
  bool Active() const;
  unsigned long ElapsedDuration() const;
  unsigned long mStartTime;
  unsigned long mDurationMs;
};

#endif  // _SENSORS_H_

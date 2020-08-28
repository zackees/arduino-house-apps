

#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "defs.h"

void sensors_init();

class DurationTimer {
 public:
  DurationTimer() : mStartTime(0), mDurationMs(0) {}
  void Restart(unsigned long duration_ms);
  bool Active() const;
  unsigned long mStartTime;
  unsigned long mDurationMs;
};

#endif  // _SENSORS_H_



#ifndef _SENSORS_H_
#define _SENSORS_H_

#include "Arduino.h"

#include "defs.h"

inline void sensors_init() {
  pinMode(PIN_EXTERNAL_SIG, INPUT);
  pinMode(PIN_PIR, INPUT);
}

class DurationTimer {
 public:
  DurationTimer() : mStartTime(0), mDurationMs(0) {}
  void Restart(unsigned long duration_ms) {
    mStartTime = millis();
    mDurationMs = duration_ms;
  }
  
  bool Active() const {
    return (mDurationMs > (millis() - mStartTime));
  }
  unsigned long mStartTime;
  unsigned long mDurationMs;
};

#endif  // _SENSORS_H_

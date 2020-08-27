#include "Arduino.h"

class FrameRateLock {
 public:
  FrameRateLock(float fps) {
    mStartTime = 0;
    mWaitDurationUs = (unsigned long)(double(1000 * 1000) / double(fps));
    Reset();
  }
  
  void Wait() {
    while (!Expired()) {}
    Reset();
  }
  
  // Unnecessary to call.
  void Reset() {
    mStartTime = micros();
  }
  
  bool Expired() {
    unsigned long delta_t = micros() - mStartTime;
    return delta_t > mWaitDurationUs;
  }

  unsigned long mStartTime;
  unsigned long mWaitDurationUs;
};

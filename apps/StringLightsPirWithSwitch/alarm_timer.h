

#ifndef _ALARM_TIMER_H_
#define _ALARM_TIMER_H_

#include "Arduino.h"
#include <inttypes.h>

class AlarmTimer {
 public:
  uint32_t start_time_;
  AlarmTimer() {
    start_time_ = millis();
  }

  void Trigger() {
    start_time_ = millis();
  } 
  uint32_t Update(uint32_t time_now, bool alarm) {
    uint32_t now = time_now;
    if (alarm) {
      start_time_ = now;
    }
    uint32_t delta = now - start_time_;
    return delta;
  }
};

#endif  // _ALARM_TIMER_H_

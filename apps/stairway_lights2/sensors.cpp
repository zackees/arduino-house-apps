


#include "Arduino.h"
#include "defs.h"
#include "sensors.h"

void sensors_init() {
  pinMode(PIN_EXTERNAL_SIG, INPUT);
  pinMode(PIN_PIR, INPUT);
}

void DurationTimer::Restart(unsigned long duration_ms) {
    mStartTime = millis();
    mDurationMs = duration_ms;
}
  
bool DurationTimer::Active() const {
  return mDurationMs > ElapsedDuration();
}
unsigned long DurationTimer::ElapsedDuration() const {
    return millis() - mDurationMs;
}
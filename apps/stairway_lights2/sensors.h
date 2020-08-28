



class MotionDetector {
 public:
  MotionDetector(int pin) : mPin(pin), mVal(false) {}

  void Update() {
    mVal = (digitalRead(mPin) == HIGH);
  }
  bool Value() { return mVal; }

  int mPin;
  bool mVal;
  unsigned long time_start;
};

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

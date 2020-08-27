#include <Led.h>
#include <MaxSonarRangeFinder.h>

Led led(9);
MaxSonarRangeFinder range_finder(3);


class AbstractCoroutine {
 public:
  AbstractCoroutine() : time_trigger_ms_(0.0f) {}
  virtual ~AbstractCoroutine() {}
  
  void Update(float delta_time_ms) {
    time_trigger_ms_ -= delta_time_ms;
    if (time_trigger_ms_ < 0.0) {
      time_trigger_ms_ = OnCoroutine();
    }
  }
 protected:
  // Interface function
  // Returns the amount of time to sleep in milliseconds
  // before the next OnCoroutine() is called.
  virtual float OnCoroutine() = 0;
  
 private:
  float time_trigger_ms_;
};

class CoroutineDispatch {
 public:
  CoroutineDispatch(AbstractCoroutine* coroutines, int num_coroutines)
      : coroutines_(coroutines), num_coroutines_(num_coroutines), last_time_ms_(0.0f) {
  }
  void Update() {
    float curr_time_ms = millis();
    float dt = 0.0;
    // Either first run condition OR time wrapped OR called in same millisecond
    // slice.
    if (last_time_ms_ == 0.0f || last_time_ms_ >= curr_time_ms_) {
      // dt stays 0.0f and OnRoutine() is not called.
      last_time_ms_ = curr_time_ms_;
      return;
    }
    dt = curr_time_ms_ - last_time_ms_;
    last_time_ms_ = curr_time_ms_;
  }
  
 private:
  AbstractCoroutine* coroutines_;
  int num_coroutines_;
  float last_time_ms_;
};

void HeartBeatCycle(int delay_speed) {
  while (!led.TransitionBrightness(255, 16)) {
    delay(delay_speed);
  }
  
  while (!led.TransitionBrightness(0, 16)) {
    delay(delay_speed);
  }
  
  while (!led.TransitionBrightness(255, 16)) {
    delay(delay_speed);
  }
  
  while (!led.TransitionBrightness(0, 16)) {
    delay(delay_speed);
  }
  delay(500);
}


// the setup routine runs once when you press reset:
void setup()  { 
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop()  {
  
  int delay_speed = 15;
  int heart_beat_trigger_cm = 45.0f;
//   HeartBeatCycle(delay_speed);

  float cm = range_finder.PingRangeCentimetersUsingPW();
  Serial.print("Range cm: ");
  Serial.println(cm);
  
  if (cm < heart_beat_trigger_cm) {
    HeartBeatCycle(delay_speed);
  }


  
  delay(100);
  

}


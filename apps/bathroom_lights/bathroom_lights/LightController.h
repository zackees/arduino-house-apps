
class LightController : public AbstractCoroutine {
 public:
  LightController() : blue_led_(6), red_led_(9), white_led_(3), el_wire_(5), last_time_(millis()) {
  }

  virtual int OnCoroutine() {
    unsigned long long curr = millis();
    const float dt = static_cast<float>(curr - last_time_) / 1000.0f;
    last_time_ = curr;
    Update(dt);
    return 1;
  }
  
  void Update(float dt) {
    blue_led_.Update(dt);
    red_led_.Update(dt);
    white_led_.Update(dt);
    el_wire_.Update(dt);
  }
  
  void Test() {
    for (int i = 0; i < kNumLights; ++i) {
      for (int ii = 0; ii < kNumLights; ++ii) {
        if (i == ii) {
          get_light(ii).On(); 
        } else {
          get_light(ii).Off(); 
        }
      } 
      delay(1000);
    }
  }
  

 
  enum {
    kBlueLed = 0,
    kRedLed = 1,
    kWhiteLed = 2,
    kElWire = 3,
    kNumLights = 4
  };
  
  struct LedState : public Led {
    LedState(int pin) : Led(pin), rate_(0.0f), power_(0.0f), target_power_(0.0) {}
    void Update(float dt) {
      // If nothing to do then return.
      if (power_ == target_power_) {
        return; 
      }
      // move the power toward target_power_, in position of dt.
      if (power_ < target_power_) {
        power_ = min(power_ + (rate_ * dt), target_power_);
      } else {
        power_ = max(power_ - (rate_ * dt), target_power_); 
      }
      // Then finally converrt the power to the led integer mapping.
      Led::Set(static_cast<int>(power_ * 255.0f));
    }
    void Transition(float target_power, float transition_time_secons) {
      rate_ = 1.0f / transition_time_secons; 
      target_power_ = target_power;
    }
    float rate_;
    float power_;
    float target_power_;
  };
  
  // get_light(...) and count() allow array style access to the underlying
  // lights.
  LedState& get_light(int i) {
     switch (i) {
       case kBlueLed: return blue_led_;
       case kRedLed: return red_led_; 
       case kWhiteLed: return white_led_;
       case kElWire:
       default: return el_wire_;
     }
  }
  
  int count() const { return kNumLights; }
  
 private:

  LedState blue_led_, red_led_, white_led_, el_wire_;
  unsigned long long last_time_;
};

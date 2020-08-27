
class AbstractPriorityAction {
 public:
  virtual void Update(float range_meters, float temperature_f) = 0;
  // Returns true if Executed() invoked changes to the environment.
  virtual bool Execute() = 0;
};

// When nothing else can Execute, then this class will execute.
class NullAction : public AbstractPriorityAction {
 public:
  NullAction(LightController& lc) : light_controller_(lc) {}
  virtual void Update(float range_meters, float temperature_f) {}
  
  // Always executes.
  virtual bool Execute() {
    for (int i = 0; i < light_controller_.count(); ++i) {
      // Transition to off over one second.
      light_controller_.get_light(i).Transition(0.0, 10.0);
    }
    return true;
  }
 private:
  LightController& light_controller_;
};

class CooldownAction : public AbstractPriorityAction {
 public:
  CooldownAction(LightController& lc) : light_controller_(lc), end_time_stamp_(0) {}

  virtual void Update(float range_meters, float temperature_f) {
    if (range_meters < 1.2f) {
      typedef unsigned long ul;
      // 2 mins until end of time.
      end_time_stamp_ = millis();
      end_time_stamp_ += ul(20) * ul(60) * ul(1000);  // + 20 minutes.
    }
  }

  // Always executes.
  virtual bool Execute() {
    // Serial.println(end_time_stamp_);
    
    if (end_time_stamp_ > millis()) {
      // 33% power over .7 seconds.
      light_controller_.get_light(LightController::kWhiteLed).Transition(0, 10.0);
      
      const float sin_t = SinTime(1.0f / 5.0f);
      float red_power = mapf(sin_t, -1, 1, .12, .50);
      light_controller_.get_light(LightController::kRedLed).Transition(red_power, 10.0);
      
      const float cos_t = CosTime(1.0f / 5.0f);
      float blue_power = mapf(cos_t, -1, 1, .12, .50);
      light_controller_.get_light(LightController::kBlueLed).Transition(blue_power, .7);
      return true; 
    }
    // Do nothing.
    return false;
  }
  
 private:
  LightController& light_controller_;
  unsigned long end_time_stamp_;
};

class ProximiyAction : public AbstractPriorityAction {
 public:
  ProximiyAction(LightController& lc) : light_controller_(lc), state_(-1) {}
  virtual void Update(float range_meters, float temperature_f) {
    range_meters_ = range_meters;
    if (range_meters_ > 1.2f) {
      state_ = -1; 
    }
  }
  
  // Always executes.
  virtual bool Execute() {
    

    if (range_meters_ < .4f || (state_ == 0 && range_meters_ < .5f)) {
      state_ = 0;
      // 1st arg - brightness brightness
      // 2nd arg - speed to achieve full brightness.
      light_controller_.get_light(LightController::kWhiteLed).Transition(0.50, 1.0);
      light_controller_.get_light(LightController::kRedLed).Transition(0.08, 4.0);
      light_controller_.get_light(LightController::kBlueLed).Transition(0.0, 10.0);
      return true;
    }
    
    if (range_meters_ < .7f || (state_ == 1 && range_meters_ <1.4f)) {
      state_ = 1;
      // 1st arg - brightness brightness
      // 2nd arg - speed to achieve full brightness.
      light_controller_.get_light(LightController::kWhiteLed).Transition(0.25, 5.0);
      light_controller_.get_light(LightController::kRedLed).Transition(0.08, 4.0);
      light_controller_.get_light(LightController::kBlueLed).Transition(.12, 5.0);
      return true;
    }
    
    if (range_meters_ < 1.4f || (state_ == 2 && range_meters_ < 2.0f)) {
      state_ = 2;
      light_controller_.get_light(LightController::kWhiteLed).Transition(0.0, 10.0);
      
      float red_val = mapf(SinTime(.50), -1, 1, .10, .20);
      float blue_val = mapf(CosTime(.50), -1, 1, .15, .20);
      light_controller_.get_light(LightController::kRedLed).Transition(red_val, 2.0);
      light_controller_.get_light(LightController::kBlueLed).Transition(blue_val, 3.0);
      return true;
    }
    // Do nothing.
    return false;
  }
  
 private:
  LightController& light_controller_;
  float range_meters_;
  int state_;
};


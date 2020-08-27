
#include "Coroutine.h"
#include "Led.h"
#include "MaxSonarRangeFinder.h"
#include "SubsumableAction.h"

#define ENABLE_DEBUG_PRINT
#ifdef ENABLE_DEBUG_PRINT
  #define dprint(x) Serial.print(x);
  #define dprintln(x) Serial.println(x);
#else
  #define dprint(x) ;
  #define dprintln(x) ;
#endif

const int light_sensor_pin = A0;
const int range_finder_pin = 7;  

const int startup_cycles = 1;

const int triggered_cycles = 10;
const int delay_speed = 14;

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class DistanceSampler : public AbstractCoroutine {
 public:
  DistanceSampler()
      : range_finder_(7), proximity_cm_(0.0),
        com_pin_(12), alarm_triggered_(false) {
     pinMode(com_pin_, INPUT);      // sets the digital pin as input
  }
  
  virtual int OnCoroutine() {
    proximity_cm_ = range_finder_.PulsePWCentimeters();
    alarm_triggered_ = (digitalRead(com_pin_) == LOW);
    return 250;
  }

  float proximity_cm() const { return proximity_cm_; }
  bool alarm_triggered() const { return alarm_triggered_; }
 private:
  MaxSonarRangeFinder range_finder_;
  float proximity_cm_;
  int com_pin_;
  bool alarm_triggered_;
};


class LightController : public AbstractCoroutine {
 public:
  LightController() : blue_led_(3),
                      red_led_(9),
                      green_led_(6),
                      last_time_(millis()) {}

  virtual int OnCoroutine() {
    unsigned long long curr = millis();
    const float dt = static_cast<float>(curr - last_time_) / 1000.0f;
    last_time_ = curr;
    Update(dt);
    return 1;
  }
  
  void Update(float dt) {
    for (int i = 0; i < kNumLights; ++i) {
      get_light(i).Update(dt); 
    }
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
    kGreenLed = 2,
    kNumLights = 3
  };
   
  // get_light(...) and count() allow array style access to the underlying
  // lights.
  TimedLed& get_light(int i) {
     switch (i) {
       case kBlueLed: return blue_led_;
       case kRedLed: return red_led_; 
       case kGreenLed: return green_led_;
       default: return green_led_;
     }
  }
  
  int count() const { return kNumLights; }
  
 private:
  TimedLed blue_led_, red_led_, green_led_;
  unsigned long long last_time_;
};

struct SensorEnvironment {
  SensorEnvironment(float proximity_cm, bool top_of_stairs_alarm)
      : proximity_cm_(proximity_cm),
        top_of_stairs_alarm_(top_of_stairs_alarm) {}
  float proximity_cm_;
  bool top_of_stairs_alarm_;
};

LightController light_controller_;
DistanceSampler distance_sampler_;
AbstractCoroutine *tasks_for_dispatch[] = { &light_controller_, &distance_sampler_ };
CoroutineDispatch dispatch(tasks_for_dispatch);


class NullAction : public AbstractSubsumableAction<SensorEnvironment> {
 public:
  virtual void Update(const SensorEnvironment& env) {}
  virtual bool Execute() {
    for (int i = 0; i < light_controller_.count(); ++i) {
      light_controller_.get_light(i).Transition(0, 10);
    }
    return true;
  }
};

class ProximityAction : public AbstractSubsumableAction<SensorEnvironment> {
 public:

  ProximityAction() : expire_time_(0) { ResetExpireTime(); }
  virtual void Update(const SensorEnvironment& env) {
    dprint("env.top_of_stairs_alarm_ = "); dprintln(env.top_of_stairs_alarm_);
    if (env.proximity_cm_ < 70.0f || env.top_of_stairs_alarm_) {
      ResetExpireTime();
    }
  }
  virtual bool Execute() {
    unsigned long time_ms = millis();
    float time_f = static_cast<float>(time_ms);
    
    if (expire_time_ < time_f) {
      return false;  // Didn't execute. 
    }
    
    dprint("time remaining (ms) = "); dprintln(expire_time_ - time_f);
    
    
    float cycle_value = static_cast<float>(time_ms % 6000) / 6000.0f;
    
    int index_brighter = cycle_value <= .33f ? 0 :
                            cycle_value <= .67f ? 1 : 2;
    
   
    for (int i = 0; i < light_controller_.count(); ++i) {
      if (index_brighter == i) {
        light_controller_.get_light(i).Transition(1.0f, .5f);
      } else {
        light_controller_.get_light(i).Transition(0.0f, .5f);
      }
    }
    return true;
  }
  
  void ResetExpireTime() {
    float time_f = static_cast<float>(millis());
    expire_time_ = time_f + (1000.0f * 60.0f * 1.0f);
  }
  
 private:
  float expire_time_;
};


NullAction null_action_;
ProximityAction proximity_action_;

AbstractSubsumableAction<SensorEnvironment>* actions[] = {
  &proximity_action_,
  &null_action_
};
SubsumptionHierarchy<SensorEnvironment> subsumption(actions);


// the setup routine runs once when you press reset:
void setup()  { 
   // Allows debugging.
  Serial.begin(9600);
  
  //delay(500);
  for (int i = 0; i < light_controller_.count(); ++i) {
    light_controller_.get_light(i).Off(); 
  }
}

// the loop routine runs over and over again forever:
void loop()  {
  
  dispatch.Update();
  
#if 0
  light_controller_.Test();
  return;
#endif

  float proximity_cm = distance_sampler_.proximity_cm();
  if (0.0f == proximity_cm) {
    dprintln("Range finder is not working");
    delay(1000);
    return;
  }
  // dprint("proximity_cm: "); dprintln(proximity_cm);
  SensorEnvironment env(proximity_cm, distance_sampler_.alarm_triggered());
  subsumption.UpdateThenExecute(env);
}


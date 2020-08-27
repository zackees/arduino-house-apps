#include <Coroutine.h>
#include <Led.h>
#include <MaxSonarRangeFinder.h>

class ScopedTimer {
 public:
  ScopedTimer(const char* str) : str_(str) {
    time_ = millis();
  }
  ~ScopedTimer() {
    int dt = millis() - time_;
    Serial.print(str_);
    Serial.print(": ");
    Serial.println(dt);
  }
  
  int time_;
  const char* str_;
};

// #define PROFILE_TRACING
// #define DEBUG_PRINTING
#define DISABLE_PROXY_SENSOR

#ifdef DEBUG_PRINTING
# define DPrint Serial.print
# define DPrintln Serial.println
#else
# define DPrint
# define DPrintln
#endif  // DEBUG_PRINT

#ifdef PROFILE_TRACING
#  define TRACE_SCOPE(S) ScopedTimer function_timer(S);
#else
#  define TRACE_SCOPE(S)
#endif

class HeartBeatCoroutine : public AbstractCoroutine {
 public:
  struct State {
    State() : anim_sub_state_(0), range_cm_(0), step_delay_ms_(0) {}
    int anim_sub_state_;
    int range_cm_;
    int step_delay_ms_;
  };
  
  HeartBeatCoroutine(int pin_led, int pin_range_finder)
      : led_(pin_led), range_finder_(pin_range_finder), state_() {}

  virtual int OnCoroutine() {
    TRACE_SCOPE(__FUNCTION__);
    //state_.range_cm_ = GetRangeCM();
    return UpdateAnimation(&state_, &led_);
  }
  
  int GetRangeCM() {
    #if defined(DISABLE_PROXY_SENSOR)
    return 8;  // 8 centimeters.
    #else
    TRACE_SCOPE(__FUNCTION__);
    int signal = range_finder_.PulsePWSignal();
    int cm = range_finder_.Signal2Centimeters(signal);
    
    Serial.print("GetRangeCM sees : ");
    Serial.println(cm);
    return cm;
    #endif
  }
  
  int UpdateAnimation(State* state, Led* led) {
    enum AnimState {
      kPreAnimation = 0,
      k1stBeatIn,
      k1stBeatOut,
      k2ndBeatIn,
      k2ndBeatOut
    };
    
    char* dbg_str[] = {
      "kPreAnimation",
      "k1stBeatIn",
      "k1stBeatOut",
      "k2ndBeatIn",
      "k2ndBeatOut"
    };
    
    DPrint(dbg_str[state->anim_sub_state_]);
    DPrint(",  Led: ");
    DPrint(led->Get());
    DPrint(", delay_speed: ");
    DPrintln(state->step_delay_ms_);
    
    // Play animation.
    const int kTransitionSpeed = 8;
   
    switch (state->anim_sub_state_) {
      case kPreAnimation: {
        state->range_cm_ = GetRangeCM();
        state->step_delay_ms_ = ClassifyProximity(state->range_cm_);
        // Looks like the person is not close enough.
        if (state->step_delay_ms_ < 0) {
          return 50;
        } else {
          state->anim_sub_state_ = k1stBeatIn;
        }
      }
      case k1stBeatIn: {
        // returns true when the brightness has finished transitioning
        if (led->TransitionBrightness(255, kTransitionSpeed)) {
          state->anim_sub_state_ = k1stBeatOut;   
        }
        return state->step_delay_ms_;
      }
      case k1stBeatOut: {
        // returns true when the brightness has finished transitioning
        if (!led->TransitionBrightness(0, kTransitionSpeed)) {
          return state->step_delay_ms_;
        } else {
          state->anim_sub_state_ = k2ndBeatIn;
          return state->step_delay_ms_ * 20;
        }
      }
      
      case k2ndBeatIn: {
        // returns true when the brightness has finished transitioning
        if (led->TransitionBrightness(255, kTransitionSpeed)) {
          state->anim_sub_state_ = k2ndBeatOut;   
        }
        return state->step_delay_ms_;
      }
      
      case k2ndBeatOut: {
        // returns true when the brightness has finished transitioning
        if (!led->TransitionBrightness(0, kTransitionSpeed)) {
          return state->step_delay_ms_;  // Stays in this state.
        } else {
          state->anim_sub_state_ = kPreAnimation;
          return state->step_delay_ms_ * 100;  // Final beat.
        }       
      }      
    }
    // Should not hit this case.
    return 1;
  }
  
 private:
  static int ClassifyProximity(int range_cm) {
    if (range_cm > 80) {
      // too far do nothing.
      return -1;
    } else if (range_cm > 60) {
      return 6;
    } else if (range_cm > 45) {
      return 5;
    } else if (range_cm > 35) {
      return 4;
    } else if (range_cm > 25){
      return 3;
    } else {
      return 2;
    }
  }
 
  Led led_;
  MaxSonarRangeFinder range_finder_;
  State state_;
};

class LedBlink : public AbstractCoroutine {
 public:
  explicit LedBlink(int pin) : led_(13), on_(false) {}
  
  virtual int OnCoroutine() {
    led_.Set(on_ ? 0 : 255);
    on_ = !on_;
    return 250;  // Quarter of a second delay.
  }
 
 private:
  Led led_;
  bool on_;
};


const int heart_beat_output_pin = 9;
const int range_finder_input_pin = 3;

AbstractCoroutine* coroutine_array[] = {
  new HeartBeatCoroutine(heart_beat_output_pin, range_finder_input_pin),
  new LedBlink(13),
};

CoroutineDispatch coroutines(coroutine_array);


// the setup routine runs once when you press reset:
void setup()  { 
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop()  {
  int delay_ms = 0;
  {
    TRACE_SCOPE(__FUNCTION__);
    delay_ms = coroutines.Update();
  }
  delay(delay_ms);
}
















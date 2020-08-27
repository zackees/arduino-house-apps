#include <Arduino.h>
#include "Coroutine.h"
#include "LedRopeTCL.h"
#include "util.h"

//#include <ApproximatingFunction.h>

// Color cycle time.
static const unsigned long kCycleTimeMs = 10 * 1000;
// How long the lights stay lit.
static const float kCoolDownMs = 5. * 60. * 1000.;
//static const float kCoolDownMs = 1 * 1000.;

// Interpolates from one float value to the other.
// Example:
//   InterpAnimator ia(0.0, 1.0, 4.0);  // 0->1 in 4 seconds.
//   while (ia.Value() < 4.0) {;}
class InterpAnimator {
 public:
  InterpAnimator() : start_val_(0), end_val_(0), start_time_(0), end_time_(0), transition_time_(0) {}
  void SetAll(float start_val, float end_val, float transition_time_secs) {
    start_val_ = start_val;
    end_val_ = end_val;
    start_time_ = millis();
    end_time_ = start_time_ + static_cast<unsigned long>(transition_time_secs * 1000);
    transition_time_ = transition_time_secs;
  }
  
  float Value() const {
    unsigned long time_now = millis();
    if (time_now <= start_time_) {
      return start_val_;
    }
    if (time_now >= end_time_) {
      return end_val_;
    }
    
    unsigned long relative_time_now = time_now - start_time_;
    unsigned long duration_time = end_time_ - start_time_;
    
    const float t = static_cast<float>(relative_time_now) /
                    static_cast<float>(duration_time);
              
    return start_val_ * (1.0f - t) + end_val_ * t;
  }
  
  bool Increasing() const { return end_val_ > start_val_; }
  
  bool Finished() const {
    return millis() >= end_time_;
  }
  
 private:
  float start_val_;
  float end_val_;
  unsigned long start_time_;
  unsigned long end_time_;
  float transition_time_;
};

///////////////////////////////////////////////////
class DarknessPainter {
 public:
  DarknessPainter() { Init(); }
  void Start() {
    // go from 1->0 in 10 seconds.
    interp_anim_.SetAll(1.0f, -0.5f, 5.0f);
  }
  
  void Update(int length) {
    float t = interp_anim_.Value();
    mid_point_ = length / 2;
    dist_full_color_ = t * mid_point_;
    dist_full_black_ = (t + .5) * mid_point_;
  }
  
  float Brightness(int i) {
    int dist = abs(mid_point_ - i);
    float b = mapf(dist, dist_full_color_, dist_full_black_, 0.0f, 1.0f);
    b = constrain(b, 0.0f, 1.0f);
    return b;
  }
 private:
  void Init() {
    dist_full_black_ = 0;
    dist_full_color_ = 0;
  }
  InterpAnimator interp_anim_;
  float dist_full_color_;
  float dist_full_black_;
  int mid_point_;
};

class Painter : public AbstractCoroutine {
 public:
  Painter(LedRopeTCL* led_rope) : led_rope_(led_rope), brightness_(0.0f), time_presence_(0) {}
  virtual int OnCoroutine() {
    UpdateColor();
    darkness_painter_.Update(led_rope_->length());
    
    unsigned long start_time = millis();
    ColorHSV c = color_;
    
    for (int i = 0; i < led_rope_->length(); ++i) {

      ColorHSV c2 = c;
      float b = darkness_painter_.Brightness(i);
      c2.v_ *= b;
      led_rope_->Set(i, c2.ToRGB()); 
    }
    unsigned long time_delta = millis() - start_time;
    
    //Serial.print("time_to_paint = "); Serial.println(time_delta);
    led_rope_->Draw(); 
    return 0;
  }
  
  void OnDownStairsPresence() {
    time_presence_ = millis();
    if (brightness_animation_.Increasing()) {
      return; 
    }
    darkness_painter_.Start(); 
    brightness_animation_.SetAll(brightness_, 1.0f, 1.0f);  // Transition from 0->1 in 1 sec
  }

  void UpdateColor() {
    brightness_ = brightness_animation_.Value();
    
    if (brightness_animation_.Finished()) {
      const float time_since_presence = static_cast<float>(millis() - time_presence_);
      
      DPRINT("time_since_presence: "); DPRINT(time_since_presence);  DPRINT(",");
      
      //Serial.print("kCoolDownMs: ");
      //Serial.println(kCoolDownMs);
      
      if (time_since_presence > kCoolDownMs) {  // Have we exceeded cooldown?
        // ... then turn the brightness down.
        //Serial.println("Fade off");
        brightness_animation_.SetAll(brightness_, 0.0f, 1.0f);
      }
    }
  
    const unsigned long cur_cyc_time = millis() % kCycleTimeMs;
    
    
    float hue = float(cur_cyc_time) / float(kCycleTimeMs);
    color_.Set(hue, 1.0f, brightness_);
  }
  
 private:
  LedRopeTCL* led_rope_;
  ColorHSV color_;
  float brightness_;
  InterpAnimator brightness_animation_;
  unsigned long time_presence_;
  
  DarknessPainter darkness_painter_;
};


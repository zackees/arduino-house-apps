#ifndef PAINTER_H_
#define PAINTER_H_

#include "Arduino.h"


class Painter {
 public:
  Painter() { Reset(); }
  void Apply(unsigned long time, LedRopeTCL* led_rope) {
    Update(time);
    ApplyWaterLayer(led_rope, time);
  }
  
  void Reset() {
    user_near_transition_ = 0.0f;
    user_near_time_ = 0;
  }
  
  void OnUserNear(unsigned long time_now) {
    user_near_time_ = time_now;
  }
  
 private:
 
  static float NoiseSin(double x) {
    return FastSin(x * .125);
  }
  
  void Update(unsigned long time) {
    if (IsUserNear(time)) {
      // Speed of fade in.
      user_near_transition_ = min(1.0f, user_near_transition_ + .12);  // TODO: make frame rate invariant
    } else {
      user_near_transition_ = max(0.0f, user_near_transition_ - .04); 
    }
  }
  
  bool IsUserNear(unsigned long time_now) const {
    unsigned long delta_time = time_now - user_near_time_;
    return delta_time < 1000;  // 1 second.
  }

  void ApplyWaterLayer(LedRopeTCL* led_rope, unsigned long time_now) {
    
    // Reset once every day.
    static const unsigned long kCyclePeriod =
        1000ul * 60ul * 60ul * 24ul;
    time_now = time_now % kCyclePeriod;
    
    const static double kSpeed = .5;
    const double t1 = kSpeed * time_now / 90.0;
    const double t2 = kSpeed * time_now / 70.0;
   
    
    for (int i = 0; i < led_rope->length(); ++i) {
      float b_v = NoiseSin(i*6 + t1);
      float g_v = NoiseSin(i*4 + t2);
      
      int b = int((b_v * .5f + 0.5f) * 255.f);
      int g = int((g_v * .5f + 0.5f) * 255.f);
      
      int r = 0;
      
      // Swap blue fades to red color.
      if (user_near_transition_ > 0.0) {
        float b_v = b / 255.f;
        float g_v = g / 255.f;
        
        float r_v = b_v * user_near_transition_;
        b_v = b_v * (1.0f - user_near_transition_);
        g_v = g_v * (1.0f - user_near_transition_);
        
        r = static_cast<int>(255.f * r_v);
        b = static_cast<int>(255.f * b_v);
        g = static_cast<int>(255.f * g_v);
        
        r = max(0, r - int(user_near_transition_ * 128));
      }  
      Color3i c(r, g, b);
      led_rope->Set(i, c);
    }
  }
  
  bool first_run_;
  unsigned long user_near_time_;
  float user_near_transition_; // {0 -> 1}
  
};

#endif PAINTER_H_

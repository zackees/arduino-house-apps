

#ifndef _DARKNESS_H_
#define _DARKNESS_H_


#include "Arduino.h"
#include "util.h"
#include <stdint.h>

// Interpolates from one float value to the other.
// Example:
//   InterpAnimator ia(0.0, 1.0, 4.0);  // 0->1 in 4 seconds.
//   while (ia.Value() < 4.0) {;}
class InterpAnimator {
 public:
  InterpAnimator() : start_val_(0), end_val_(0), start_time_(0), end_time_(0), transition_time_(0) {}
  void SetAll(float start_val, float end_val, float transition_time_secs, uint32_t start_time) {
    start_val_ = start_val;
    end_val_ = end_val;
    start_time_ = start_time;
    end_time_ = start_time_ + static_cast<unsigned long>(transition_time_secs * 1000);
    transition_time_ = transition_time_secs;
  }
  
  float Value(uint32_t time_now) const {
    if (time_now <= start_time_) {
      return start_val_;
    }
    if (time_now >= end_time_) {
      return end_val_;
    }
    
    uint32_t relative_time_now = time_now - start_time_;
    uint32_t duration_time = end_time_ - start_time_;
    
    const float t = static_cast<float>(relative_time_now) /
                    static_cast<float>(duration_time);
              
    return start_val_ * (1.0f - t) + end_val_ * t;
  }
  
  bool Increasing() const { return end_val_ > start_val_; }
  
  bool Finished(uint32_t now) const {
    return now >= end_time_;
  }
  
 private:
  float start_val_;
  float end_val_;
  uint32_t start_time_;
  uint32_t end_time_;
  float transition_time_;
};



#endif  // _DARKNESS_H_

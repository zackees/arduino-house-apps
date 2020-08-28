

inline float mapf(float t, float in_min, float in_max, float out_min, float out_max) {
  return (t - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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
class FxEaseIn {
 public:
  FxEaseIn() { Init(); }
  void Start() {
    // go from 1->0 in 10 seconds (update: values might be updated from comment.)
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



///////////////////////////////////////////////////
// experimental.
class FxEaseOut {
 public:
  FxEaseOut() { Init(); }
  void Start() {
    // go from 1->0 in 10 seconds (update: values might be updated from comment.)
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
    return 1.0 - b;
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


class TransitionOnStateFSM {
  public:
   TransitionOnStateFSM() : activation_time_(0), prev_time_now_(0), value_(0) {
   }
   
   float value_;
   
   
   void Update(unsigned long time_now, bool movement_detected) {
    if (0 == prev_time_now_) {
       prev_time_now_ = time_now;
     }
     if (movement_detected) {
       activation_time_ = time_now;
     }
 
     const unsigned long time_diff = time_now - prev_time_now_;
     const float dt = float(time_diff) / 1000.0f;
     const float d_activation_t = float(time_now - activation_time_) / 1000.0f;
     prev_time_now_ = time_now;

     const float kActivationSpeed = 0.1f;
     const float kOffCooldown = 30.0f;  // Seconds
     
     float velocity = dt * kActivationSpeed;
     
     if (d_activation_t > kOffCooldown) {
       velocity = -velocity;
     }
     value_ = constrain(value_ + velocity, 0.0f, 1.0f);
   }
   
   void Apply(LedRopeTCL* led_rope) {
     if (value_ >= 1.0f) {
       return;  // We know that all brightness values will be unmodified.
     }
     
     const float inv_denom = 1.0f / (led_rope->length() - 1.0f);
     const float double_value = 2*value_;
     const float half_pi = PI/2.0f;
     
     
     // Set the colors in reverse order so that a quick out can be caluclated
     // so to speed up when the rope is fully on.
     for (int i = 0; i < led_rope->length(); ++i) {
       
       float brightness =
         FastCos(half_pi * (1.0f - i * inv_denom))
         - 1
         + double_value;
       brightness = constrain(brightness, 0.0f, 1.0f);
       if (1.0f == brightness) {
         // Optimizes for pixels that don't need to be modified.
         continue;
       }
       if (0.0f == brightness) {
         // Optimizes for pixels that will be set black.
         led_rope->Set(i, Color3i::Black());
         continue; 
       }
       
       // Slow path.
       ColorHSV hsv(led_rope->Get(i));
       hsv.v_ *= brightness;
       led_rope->Set(i, hsv.ToRGB());
     }
   }
    
  private:
   unsigned long activation_time_;
   unsigned long prev_time_now_;
};


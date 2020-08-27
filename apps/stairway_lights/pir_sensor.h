
class PIRSensor_RadioShack {
public:
    PIRSensor_RadioShack(int pin, unsigned long latch_p_ms) : pir_pin_(pin), latch_period_ms_(latch_p_ms), start_time_(0), movement_detected_(0) {}
    bool MovementDetected() {
      Update();
      return movement_detected_;
    }
    
    bool SensorActive() {
      return HIGH == digitalRead(pir_pin_);
    }
    
    void set_latch_period(unsigned long t_ms) { latch_period_ms_ = t_ms; }
private:
    void Update() {
      unsigned long now = millis();
      if (SensorActive()) {
        movement_detected_ = true;
        start_time_ = now;
      } else {
        unsigned long delta_time = now - start_time_;
        movement_detected_ = (delta_time < latch_period_ms_);
      }
    }

    int pir_pin_;
    unsigned long latch_period_ms_;
    unsigned long start_time_;
    bool movement_detected_;
};


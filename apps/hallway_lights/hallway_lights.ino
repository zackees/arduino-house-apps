#define NUM_LEDS 50
#define PIR_PIN 4



// The amount of second that the movement latch is
// is sustained.
#define LATCH_MOVEMENT_DURATION_SEC 5

#include <SPI.h>
#include "TCL.h"
#include "FastLED.h"
#include "LedRopeTCL.h"
//#include "VisSimplexColor.h"
#include "Vis.h"


LedRopeTCL led_rope(NUM_LEDS);

//#include "PIRSensor_RadioShack.h"

class PIRSensor_RadioShack {
public:
    PIRSensor_RadioShack(int pin) : pir_pin_(pin), latch_period_(0), start_time_(0), movement_detected_(0) {}
    bool MovementDetected() {
      Update();
      return movement_detected_;
    }
    
    bool SensorActive() {
      return HIGH == digitalRead(pir_pin_);
    }
    
    void set_latch_period(unsigned long t_ms) { latch_period_ = t_ms; }
private:
    void Update() {
      unsigned long now = millis();
      if (SensorActive()) {
        movement_detected_ = true;
        start_time_ = now;
      } else {
        unsigned long delta_time = now - start_time_;
        movement_detected_ = (delta_time < latch_period_);
      }
    }

    int pir_pin_;
    unsigned long latch_period_;
    unsigned long start_time_;
    bool movement_detected_;
};


PIRSensor_RadioShack proxy_sensor(PIR_PIN);


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  proxy_sensor.set_latch_period(LATCH_MOVEMENT_DURATION_SEC*1000);  // 1 second.
}

void loop() {
  bool movement_detected = proxy_sensor.MovementDetected();
  Serial.print("m:"); Serial.println(movement_detected);
  
  Fire2012(proxy_sensor.MovementDetected(), &led_rope);
}


#include <SPI.h>
#include <TCL.h>

#include <math.h>
#include "FastSin.h"
//#include "StopWatch.h"
#include "LedRopeTCL.h"
#include "RangeFinderSharpIR.h"
#include "TransitionOnStateFSM.h"
#include "Painter.h"



//#define P(X) Serial.println(X)
#define P(X)


class PIR_RadioShack {
 public:
  explicit PIR_RadioShack(int pin) : pir_pin_(pin) {}
  bool MovementDetected() const {
    return HIGH == digitalRead(pir_pin_);
  }
 private:
  int pir_pin_;
};


const int kNumLeds = 58;
LedRopeTCL led_rope(kNumLeds);
PIR_RadioShack pir(4);
RangeFinderSharpIR distance_sensor(A0);

Painter painter;
TransitionOnStateFSM fsm;


void setup() {
  Serial.begin(9600);
  led_rope.FillColor(Color3i::Black());
  led_rope.Draw();
  delay(100);
  led_rope.FillColor(Color3i::Red());
  led_rope.Draw();
  delay(100);
  led_rope.FillColor(Color3i::Green());
  led_rope.Draw();
  delay(100);
  led_rope.FillColor(Color3i::Blue());
  led_rope.Draw();
  delay(100);
}



// the loop routine runs over and over again forever:
void loop() {
  unsigned long time_now = millis();
  
  const float inches = distance_sensor.SampleDistanceInches();
  // Serial.print("inches: "); Serial.println(inches);
  
  unsigned long start_profile = millis();
  
  fsm.Update(time_now, pir.MovementDetected());
  
  
  if (inches < 35) {
    painter.OnUserNear(time_now);
  }
  
  unsigned long start_water_layer = millis();
  painter.Apply(time_now, &led_rope);
  unsigned long time_water_layer = millis() - start_water_layer;
  
  unsigned long start_transition_fsm = millis();
  fsm.Apply(&led_rope);
  unsigned long time_transition_fsm = millis() - start_transition_fsm;
  
  led_rope.Draw();
  
  unsigned long time_profile = millis() - start_profile;
  
  static bool dump_stats = false;
  if (Serial.available()) {
    dump_stats = ('1' == Serial.read());
  }
  
  if (dump_stats) {
    Serial.print("FrameRate: "); Serial.println(1000.0f / float(time_profile));
    Serial.print("  FrameTime: "); Serial.println(time_profile);
    Serial.print("  Time for water: "); Serial.println(time_water_layer);
    Serial.print("  Time for transition fsm: "); Serial.println(time_transition_fsm);
  }
}

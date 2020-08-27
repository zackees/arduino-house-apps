
#include "RangeFinderSharpIR.h"

RangeFinderSharpIR dist_sensor(A0);
const int output_pin(6);

#define ENABLE_DEBUG_PRINT
#ifdef ENABLE_DEBUG_PRINT
  #define dprint(x) Serial.print(x);
  #define dprintln(x) Serial.println(x);
#else
  #define dprint(x) ;
  #define dprintln(x) ;
#endif

void setup() {
  Serial.begin(9600);
  dist_sensor.set_multi_sample_count(1024);
  pinMode(output_pin, OUTPUT);      // sets the digital pin as output
  digitalWrite(output_pin, HIGH);
}

void loop() {
  
#if 0
  // Testing
  delay(5000);
  digitalWrite(output_pin, LOW);
  delay(5000);
  digitalWrite(output_pin, HIGH);
  return;
#else
  
  const float inches = dist_sensor.SampleDistanceInches();
  dprint("Range finder: "); dprintln(inches);
  
  const int output_state = inches < 25.0f ? LOW : HIGH;
  dprint("output_state = "); dprintln(output_state);
  
  digitalWrite(output_pin, output_state);
  if (LOW == output_state) {
    delay(5000);  // Hold the line down low for 5 seconds.
  }
  
#endif
}


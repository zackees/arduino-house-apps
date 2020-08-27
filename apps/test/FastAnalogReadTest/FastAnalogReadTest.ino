
// Tests that the speedup for anlogRead() increases after a call to EnableFastAnalogRead().
#include "FastAnalogRead.h"


void setup() {
  //Serial.begin(115200); // use the serial port
  Serial.begin(9600);
}

// Returns average Hz that analogRead can be performed.
double TimeAnalogRead() {
  unsigned long start_time = millis();
  for (int i = 0; i < 10000; i++) {
    analogRead(A0); 
  }
  unsigned long time_delta = millis() - start_time;
  
  double time_secs = double(time_delta) / 1000.0;
  return 10000.0 / time_secs;
}

void loop() {
  static bool execute_once = false;
  static double hz_slow_analog_read = -1;
  static double hz_fast_analog_read = -1;
  
  if (!execute_once) {
    hz_slow_analog_read = TimeAnalogRead();
    EnableFastAnalogRead();  // Should now be faster.
    hz_fast_analog_read = TimeAnalogRead();
    execute_once = true;
  }
  
  Serial.print("hz_slow_analog_read = "); Serial.println(hz_slow_analog_read);
  Serial.print("hz_fast_analog_read = "); Serial.println(hz_fast_analog_read);
  Serial.print("Speedup = "); Serial.println(hz_fast_analog_read / hz_slow_analog_read);
  delay(1000);
}

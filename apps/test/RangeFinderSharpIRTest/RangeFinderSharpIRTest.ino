
#include <RangeFinderSharpIR.h>

int IRpin = A0;                                    // analog pin for reading the IR sensor

void setup() {
  Serial.begin(9600);                             // start the serial port
}

RangeFinderSharpIR ir_range_finder(0);  // Analguo pin 0

void loop() {
  float distance_meters = ir_range_finder.SampleDistanceMeters();
  Serial.print(distance_meters);
  Serial.println(" meters");
  delay(100);                                     // arbitary wait time.
}

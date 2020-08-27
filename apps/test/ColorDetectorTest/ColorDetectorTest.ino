// Copyleft Zach Vorhies 2014
// No rights reserved.
// Tested on an ArduinoMega. Connect pins to RX and TX.
// Usage:
//  ColorDetector<HardwareSerial> cd(Serial3);
//  void setup() { cd.begin(); }
//  void loop() {
//    if (cd.Update()) {  // True when new readings have occured.
//      cd.PrintValues(Serial);  // print values.
//    }
//  }


#include "ColorDetector.h"

ColorDetector<HardwareSerial> color_detector(Serial3);

void setup() {                                                                //set up the hardware
  Serial.begin(9600);                                                       //set baud rate for the hardware serial port_0 to 38400
  color_detector.begin();
}

void loop() {
  if (color_detector.Update()) {
    color_detector.PrintValues(Serial);
  }
}

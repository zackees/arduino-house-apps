
#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>

LedRopeTCL led_rope(150);

#define MidiSerial Serial1
const int kDelay = 50;


void TestMidiInput() {
  //*************** MIDI IN ***************//
  if (MidiSerial.available() > 0) {
    // read the incoming byte:
    byte incomingByte = MidiSerial.read();
    Serial.println(incomingByte);
  }
}



void setup(void) {
  Serial.begin(9600);
  //start serial with midi baudrate 31250
  Serial1.begin(31250); // Pins 17 & 18
}


void loop(void) {
  TestMidiInput();
}

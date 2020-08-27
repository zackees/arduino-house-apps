
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
    //Serial.print(" Read Byte: ");
    //Serial.println(incomingByte);
    if (incomingByte == 144) {
      Serial.println("WHITE!");
      led_rope.FillColor(LedRopeTCL::Color3i::White());
      led_rope.Draw();
    }
    while (MidiSerial.available()) {
      MidiSerial.read();
    }
    delay(kDelay);
  }
}



void setup(void) {
  Serial.begin(9600);
  //start serial with midi baudrate 31250
  Serial1.begin(31250); // Pins 17 & 18
  led_rope.FillColor(LedRopeTCL::Color3i::Black());
  led_rope.Draw();

}


void loop(void) {
  typedef LedRopeTCL::Color3i Color3i;
  
  Color3i colors[] = {
    Color3i::Red(),
    Color3i::Green(),
    Color3i::Blue()
  };
  
  Color3i fade_to_black(192, 192, 192);
  
  for (int c = 0; c < 3; ++c) {
    Color3i color = colors[c];
    for (int i = 0; i < led_rope.length(); ++i) {
      TestMidiInput();
      led_rope.ApplyBlendMultiply(fade_to_black);
      led_rope.Set(i, color);
      led_rope.Draw();
      delay(kDelay);
    }
  }
  
  // Finally fade everything to black.
  for (int i = 0; i < 8; ++i) {
    TestMidiInput();
    led_rope.ApplyBlendMultiply(fade_to_black);
    led_rope.Draw();
    delay(kDelay);
  }

}

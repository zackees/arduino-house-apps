#include <Arduino.h>
#include <avr/power.h>

#include "Wire.h"  // Still a requirement.
#include "ZackeesFashionGlowJewelry_v1.h"

void setup() {
  ZackeesFashionGlowJewelry_Setup();
}

void UserLedTest() {
  unsigned long t = millis() / 100ul;
  bool on = (t & 0x1);
  debugLed(on);
}

void LedRingTest() {
  // Only update every 20ms.
  static PulseTimer timer(20);
  if (!timer.Update()) {return;}
  
  static int idx = 0;
  const int n = ledRing.numPixels();
  
  idx++;
  if (idx >= ledRing.numPixels()) {
    idx = 0;
  }
  
  int r, g, b;
  r = g = b = 64;  // Max power is 255 each color.
  
  ledRing.setPixelColor(idx % n,     r, 0, 0);
  ledRing.setPixelColor((idx + 1) % n, 0, g, 0);
  ledRing.setPixelColor((idx + 2) % n, 0, 0, b);
  ledRing.show();
  ledRing.setPixelColor(idx % n,     0, 0, 0);
  ledRing.setPixelColor((idx + 1) % n, 0, 0, 0);
  ledRing.setPixelColor((idx + 2) % n, 0, 0, 0);
}

///////////////////

void loop() {
  UserLedTest();  // Red blinking led on the back.
  LedRingTest();  // Rainbow ring around the front.
  
  audioSetGain(1.0);
  int v1 = audioAvgAmplitude();

  audioSetGain(0.25);
  int v2 = audioAvgAmplitude();
  
  
  int8_t x,y,z;
  readAccelerometer(&x, &y, &z);
  SP("Accel(x,y,z): "); SP(x); SP(",\t"); SP(y); SP(",\t"); SP(z); SP("\tAudio(100%,25%): \t"); SP(v1); SP(",\t"); SPLN(v2);
}

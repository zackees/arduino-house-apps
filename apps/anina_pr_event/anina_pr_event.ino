#include <SPI.h>
#include <TCL.h>

#include "Adafruit_NeoPixel.h"
#include "OctoWS2811.h"

#include "FastSin.h"
#include "StopWatch.h"
#include "RangeFinderSharpIR.h"
#include "LedRopeTCL.h"
#include "Painter.h"

const int ledsPerStrip = 480;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(
  ledsPerStrip,
  displayMemory,
  drawingMemory,
  config);
  
Painter painter;

void setup() {
  Serial.begin(9600);
  leds.begin();
  leds.show();  // black out leds
  Serial.println("Done!");
}

void Test() {
  
  
  
  int r, g, b;
  r = g = b = 255;  // max power is 255.
  for (int i = 0; i < ledsPerStrip * 2; ++i) {
     leds.setPixel(i, r, g, b);
     leds.show();
     //delay(100);
     leds.setPixel(i, 0, 0, 0);
  }
  delay(100);
}

void TestSinWave() {
  unsigned long time_now = millis();
  painter.Apply(time_now, &leds, ledsPerStrip * 2);
  leds.show();
}

void TestLastPixel() {
  leds.setPixel(ledsPerStrip - 1, 255, 255, 255);
  leds.show();
  delay(200);
  leds.setPixel(ledsPerStrip -1, 0, 0, 0);
  leds.show();
  delay(200);
}


void loop() {
  //Test();
  TestSinWave();
  //TestLastPixel();
}

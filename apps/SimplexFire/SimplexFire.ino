
#define NUM_LEDS    200


#include "TCL.h"
#include "SPI.h"
#include "LedRopeTCL.h"
#include "TwoWireButton.h"
#include "FastLED.h"
#include "noise.h"

#include "Simplex.h"
#include "Vis.h"

LedRopeTCL led_rope(NUM_LEDS);
TwoWireButton power_button(8);

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  Serial.println("setup()");
}

void loop() {
  bool enabled = power_button.Value();
  if (!enabled) {
    return;
  }
  
  Vis(&led_rope);
  // put your main code here, to run repeatedly:
}

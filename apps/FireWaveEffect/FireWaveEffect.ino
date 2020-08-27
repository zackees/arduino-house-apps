
#define NUM_LEDS    200

// sparking: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 15

// FRAME rate of the simulationl. The Visualizater will
// interpolate between two frames to smooth out color
// transistions.
#define FRAMES_PER_SECOND 15


#include "TCL.h"
#include <SPI.h>
#include "LedRopeTCL.h"

#include "TwoWireButton.h"
#include "FastLED.h"
#include "Vis.h"


/*

#define LED_PIN     5
#define COLOR_ORDER GRB
//#define COLOR_ORDER RGB

#define CHIPSET     P9813
//#define CHIPSET     WS2811
*/






#include "LedRopeTCL.h"

LedRopeTCL led_rope(NUM_LEDS);
TwoWireButton power_button(8);



void setup() {
  Serial.begin(9600);
  Serial.println("setup()");
}

void loop()
{
  bool fire_enabled = power_button.Value();
  int ran = random(~0);
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(ran);

  Fire2012(fire_enabled, &led_rope); // run simulation frame
  
  led_rope.Draw(); // display this frame
}

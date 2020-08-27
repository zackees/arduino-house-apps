#include "Adafruit_NeoPixel.h"
#include "RangeFinderSharpIR.h"

#define LED_PIN 13
#define PIN_RANGE_FINDER A0
#define N_LEDS 60

// How long in milliseconds to play the interrupting animation.
#define MONEY_TIME_MS 1000ul

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
RangeFinderSharpIR range_finder(PIN_RANGE_FINDER);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// Returns true when the device is motion activated.
bool MotionActivated();

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(128);
  strip.show(); // Initialize all pixels to 'off'
  range_finder.set_multi_sample_count(1);  // More noise is ok since this is a binary trigger.
  Serial.println("Booted");
}


// Visualizer plan:
//  1) When no activation the color scheme will play
//  2) When activation a custom scheme will play and then the visualization will cycle back to (1)

void visAlternateYellowGreen() {
  for (int i = 0; i < 10; ++i) {
    setAllColors(strip.Color(0, 255, 0));
    strip.show();
    delay(100);
    setAllColors(strip.Color(255, 255, 0));
    strip.show();
    delay(100);
  }
  
}

void loop() {
  // Some example procedures showing how to display to the pixels:j
  /*
  visColorWipe(strip.Color(255, 0, 0), 4); // Red
  visColorWipe(strip.Color(0, 255, 0), 4); // Green
  visColorWipe(strip.Color(0, 0, 255), 4); // Blue
  // Send a theater pixel chase in...
  visTheaterChase(strip.Color(127, 127, 127), 50); // White
  visTheaterChase(strip.Color(127,   0,   0), 50); // Red
  visTheaterChase(strip.Color(  0,   0, 127), 50); // Blue

  visTheaterChaseRainbow(50);
  visRainbowCycle(4);
  
  */


  visRainbow(22);
}

bool MotionActivated() {
  float meters = range_finder.SampleDistanceMeters();
  static unsigned long alive_t_ms = 0;

  unsigned long now_ms = millis();

  if (meters < .3f) {
    alive_t_ms = now_ms + MONEY_TIME_MS;
    return true;
  }

  bool still_alive = (alive_t_ms > now_ms);
  return still_alive;
}


// Returns whether this colors are still being drawn to. When returing false the
// calling draw routine should break and return.
bool UpdateAndDraw() {

  bool motion_activated = false;
  while (MotionActivated()) {
    motion_activated = true;
    visAlternateYellowGreen();
  }

  if (!motion_activated) {
    strip.show();
  }
  return true;
}

// Fill the dots one after the other with a color
void visColorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      if (!UpdateAndDraw()) { return; }
      delay(wait);
  }
}

void setAllColors(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
}

void visRainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    if (!UpdateAndDraw()) { return; }
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void visRainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*20; j++) { // 20 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    if (!UpdateAndDraw()) { return; }
    delay(wait);
  }
}

//Theatre-style crawling lights.
void visTheaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<20; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      if (!UpdateAndDraw()) { return; }
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void visTheaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        if (!UpdateAndDraw()) { return; }
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


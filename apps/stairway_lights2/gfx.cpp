

#include "Arduino.h"
#include "gfx.h"
#include "defs.h"
#include <FastLED.h>

#define GFX_DATA_PIN 8
#define GFX_CLOCK_PIN 9


CRGB leds[NUM_LEDS];
CRGB display_leds[NUM_LEDS];  // Led's that are displayed.

void gfx_init() {
  FastLED.addLeds<P9813, GFX_DATA_PIN, GFX_CLOCK_PIN, RGB>(display_leds, NUM_LEDS);
}

void gfx_show() {
  FastLED.show();
}

bool sensor_pir_triggered() {
  return digitalRead(PIN_PIR) == HIGH;
}


bool sensor_external_triggered() {
  return digitalRead(PIN_EXTERNAL_SIG) == LOW;
}

void gfx_clear() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
    display_leds[i] = CRGB::Black;
  }
}

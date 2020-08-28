
#ifndef _GFX_H_
#define _GFX_H_

#include "defs.h"

#include <FastLED.h>
extern CRGB leds[NUM_LEDS];
extern CRGB display_leds[NUM_LEDS];  // Led's that are displayed.


void gfx_init();

void gfx_show();

#endif  // _GFX_H_

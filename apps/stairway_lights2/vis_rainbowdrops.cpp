
/*
 * File: basicfadeingamma
 * 
 * By: Andrew Tuline
 * 
 * Date: April, 2019
 * 
 * Based previous work (namely twinklefox) by Mark Kriegsman, this program shows how you can fade-in twinkles by using the fact that a random number generator
 * with the same seed will generate the same numbers every time. Combine that with millis and a sine wave and you have twinkles fading in/out.
 * 
 * The problem is that changes above 50 are much less noticeable to the viewer, so the LED gets bright quickly and then stays bright for way too long before
 * dimming again. This version also includes gamma correction to change LED brightness so that it appears to change evenly for the viewer. 
 * 
 */

                                      // Number of LED's.

#include "defs.h"
#include <FastLED.h>
#include "gfx.h"

int basicfadeingamma_loop(bool clear, bool sensor_active_top, bool sensor_active_bottom) {
  random16_set_seed(535);                                                           // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.
  for (int i = 0; i<NUM_LEDS; i++) {
    uint8_t fader = sin8(millis()/random8(10,20));                                  // The random number for each 'i' will be the same every time.
    //fader = gamma8[fader];                                          // Gamma correction
    fader = qsub8(fader, 64);
    leds[i] = CHSV(i*20,255, fader);                                                // Now, let's assign to CHSV values.
  }
  random16_set_seed(millis());                                                      // Re-randomizing the random number seed for other routines.
  return 0;
} // basicfadeingamma()

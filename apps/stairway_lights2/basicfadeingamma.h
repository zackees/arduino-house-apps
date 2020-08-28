
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


const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void basicfadeingamma();


void basicfadeingamma_loop() {

  basicfadeingamma();
  FastLED.show();
  
} // loop()



void basicfadeingamma() {

  random16_set_seed(535);                                                           // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

  for (int i = 0; i<NUM_LEDS; i++) {
    uint8_t fader = sin8(millis()/random8(10,20));                                  // The random number for each 'i' will be the same every time.
    //fader = gamma8[fader];                                          // Gamma correction
    fader = qsub8(fader, 64);
    leds[i] = CHSV(i*20,255, fader);                                                // Now, let's assign to CHSV values.
  }

  random16_set_seed(millis());                                                      // Re-randomizing the random number seed for other routines.

} // basicfadeingamma()

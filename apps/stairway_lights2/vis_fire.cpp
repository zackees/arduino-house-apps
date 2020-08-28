
#include <Arduino.h>

#include "vis_fire.h"
#include "defs.h"
#include <FastLED.h>
#include "gfx.h"

// Assumed a 4x120 grid for rendering the fire effect.
#define NUM_ROWS NUM_LEDS

// Number of fire pixels.
#define NUM_FIRE_LEDS (NUM_LEDS / 2)

void fire_visualizer_monophonic(bool clear, bool activated);


// Fire2012 with programmable Color Palette
//
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// Four different static color palettes are provided here, plus one dynamic one.
// 
// The three static ones are: 
//   1. the FastLED built-in HeatColors_p -- this is the default, and it looks
//      pretty much exactly like the original Fire2012.
//
//  To use any of the other palettes below, just "uncomment" the corresponding code.
//
//   2. a gradient from black to red to yellow to white, which is
//      visually similar to the HeatColors_p, and helps to illustrate
//      what the 'heat colors' palette is actually doing,
//   3. a similar gradient, but in blue colors rather than red ones,
//      i.e. from black to blue to aqua to white, which results in
//      an "icy blue" fire effect,
//   4. a simplified three-step gradient, from black to red to white, just to show
//      that these gradients need not have four components; two or
//      three are possible, too, even if they don't look quite as nice for fire.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".

// Entry point for running the fire visualizer.
// Returns the number of milliseconds until this visualizer should be run again.
uint32_t fire_loop(bool clear, bool sensor_active_top, bool sensor_active_bottom) {
  // Add entropy to random number generator; we use a lot of it.
  //random16_add_entropy(random(~0xffff));

  // Fourth, the most sophisticated: this one sets up a new palette every
  // time through the loop, based on a hue that changes every time.
  // The palette is a gradient from black, to a dark color based on the hue,
  // to a light color based on the hue, to white.
  //
  //   static uint8_t hue = 0;
  //   hue++;
  //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
  //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
  //   pallete = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);
  fire_visualizer_monophonic(clear, sensor_active_top || sensor_active_bottom);
  return 30;
}


class Fire2012WithPalette {
 public:
  Fire2012WithPalette() : heat(), fire_leds() {
    pallete = HeatColors_p;
    heat_scale = 1.0;
    //pallete = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::Blue, CRGB::White);

    // These are other ways to set up the color palette for the 'fire'.
    // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
    //   pallete = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  
    // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
    //   pallete = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  
    // Third, here's a simpler, three-step gradient, from black to red to white
    //   pallete = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  }

  // Two versions of this function. The enable_sparking lights up the visualizer.
  void Run(bool enable_sparking, int cooling) {
    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_FIRE_LEDS; i++) {
      heat[i] = qsub8(heat[i],  random8(0, ((cooling * 10) / NUM_FIRE_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k= NUM_FIRE_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 1] + heat[k - 1] + heat[k - 2] ) / 4;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if(enable_sparking) {
      int y = random8(7);
      // Static casting to avoid integer overflow.
      uint8_t low_limit =   static_cast<uint8_t>(min(255, static_cast<int>(160 * heat_scale)));
      uint8_t upper_limit = static_cast<uint8_t>(min(255, static_cast<int>(255 * heat_scale)));
      int val = random8(low_limit, upper_limit);
      heat[y] = qadd8(heat[y], val);
    }

    // Step 4.  Map from heat cells to LED colors
    for(int j = 0; j < NUM_FIRE_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8(heat[j], 240);
      CRGB color = ColorFromPalette(pallete, colorindex);
      fire_leds[j] = color;
    }
  }

  void clear() {
    memset(heat, 0, sizeof(heat));
    memset(fire_leds, 0, fire_leds);
  }

 //private:
  byte heat[NUM_FIRE_LEDS];
  CRGB fire_leds[NUM_FIRE_LEDS];
  CRGBPalette16 pallete;
  float heat_scale;  // = 1.0;
};


Fire2012WithPalette fire_simulator[2];


void setup_firevisualizer() {
  // Swap in a new color pallet if there is a most recent key.
  {
    CHSV chsv;
    chsv.setHSV(static_cast<byte>(128),
                static_cast<byte>(255),
                static_cast<byte>(255));
    CRGB rgb;
    hsv2rgb_rainbow(chsv, rgb);
    fire_simulator[0].pallete = CRGBPalette16(CRGB::Black, rgb, CRGB::White);
  }
  {
    CHSV chsv;
    chsv.setHSV(static_cast<byte>(255),
                static_cast<byte>(255),
                static_cast<byte>(255));
    CRGB rgb;
    hsv2rgb_rainbow(chsv, rgb);
    fire_simulator[0].pallete = CRGBPalette16(CRGB::Black, rgb, CRGB::White);
  }
}



void fire_visualizer_monophonic(bool clear, bool activated) {
  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 55, suggested range 20-100 
  int cooling = 70;

  int cooling_factor = cooling;
  static bool most_recent_key = false;  // DEBUG

  float heat_scale = 128.0 / 90.f;  // heat scale could > 1.0.
  if (heat_scale < .15f) {
    heat_scale = .15f;
  }

  if (clear) {
    for (int i = 0; i < 2; ++i) {
      fire_simulator[i].clear();
    }
  }  

#if 0
  if (heat_scale > 1.0) {
    cooling_factor = cooling / heat_scale; 
  }
#endif
  for (int i = 0; i < 2; ++i) {
    fire_simulator[i].heat_scale = heat_scale;
    fire_simulator[i].Run(activated, cooling_factor);
  }

  for (int i = 0; i < NUM_FIRE_LEDS; i++) {
    leds[i] = fire_simulator[0].fire_leds[i];
    leds[NUM_LEDS-i-1] = fire_simulator[1].fire_leds[i];
  }
}

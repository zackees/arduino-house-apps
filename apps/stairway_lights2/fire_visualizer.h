//#include <Arduino.h>


// Assumed a 4x120 grid for rendering the fire effect.
#define NUM_ROWS 1

// Number of fire pixels.
#define NUM_FIRE_LEDS NUM_LEDS


// DEPRECATED.
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
//#define SPARKING 50





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


void setup_firevisualizer() {
}


void fire_visualizer_monophonic();

// Entry point for running the fire visualizer.
// Returns the number of milliseconds until this visualizer should be run again.
uint32_t fire_loop() {
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
  fire_visualizer_monophonic();
  return 4;
}




class Fire2012WithPallete {
 public:
  Fire2012WithPallete() : heat(), fire_leds() {
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
    // Array of temperature readings at each simulation cell
    //static byte heat[NUM_FIRE_LEDS] = {0};

    // Kind of a hack to get the device to cooldown more.
    //if (heat_scale < 1.0) {

    //}

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_FIRE_LEDS; i++) {
      heat[i] = qsub8(heat[i],  random8(0, ((cooling * 10) / NUM_FIRE_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k= NUM_FIRE_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
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

 //private:
  byte heat[NUM_FIRE_LEDS];
  CRGB fire_leds[NUM_FIRE_LEDS];
  CRGBPalette16 pallete;
  float heat_scale;  // = 1.0;
};


void fire_visualizer_monophonic() {
  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 55, suggested range 20-100 
  int cooling = 120;
  static Fire2012WithPallete fire_simulator;
  bool any_on = true;
  int cooling_factor = cooling;
  static bool most_recent_key = false;  // DEBUG

  float heat_scale = 1.0f;
  if (most_recent_key) {
    most_recent_key = !most_recent_key;  // DEBUG
    // Swap in a new color pallet if there is a most recent key.

    CHSV chsv;
    chsv.setHSV(static_cast<byte>(255),
                static_cast<byte>(255),
                static_cast<byte>(255));
    CRGB rgb;
    hsv2rgb_rainbow(chsv, rgb);
    fire_simulator.pallete = CRGBPalette16(CRGB::Black, rgb, CRGB::White);
    float heat_scale = 128.0 / 90.f;  // heat scale could > 1.0.
    if (heat_scale < .15f) {
      heat_scale = .15f;
    }

    if (heat_scale > 1.0) {
      cooling_factor = cooling / heat_scale; 
    }
    fire_simulator.heat_scale = heat_scale;
  }
  fire_simulator.Run(any_on, cooling_factor);
  for (int x = 0; x < NUM_LEDS; x++) {
    leds[x] = fire_simulator.fire_leds[x];
  }
}
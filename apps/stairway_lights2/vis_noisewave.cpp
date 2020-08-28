

#include "Arduino.h"
#include "vis_noisewave.h"
#include "simplex_noise.h"
#include "gfx.h"

//#define VIS_DURATION 1000 * 60 * 10  // 10 minutes
#define VIS_DURATION 1000 * 5   // 5 seconds


void setup_noisewave() {

}

int noisewave_loop(bool clear, bool sensor_active_top, bool sensor_active_bottom) {
  unsigned long start_t = millis();
  unsigned long time_now = start_t;
  NoiseGenerator noiseGeneratorRed (500, 14);
  NoiseGenerator noiseGeneratorBlue (500, 10);
  for (int32_t i = 0; i < NUM_LEDS; ++i) {
    int r = noiseGeneratorRed.LedValue(i, time_now);
    int b = noiseGeneratorBlue.LedValue(i, time_now + 100000) >> 1;
    int g = 0;
    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
  return 0;
}

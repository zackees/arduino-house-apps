
#include <FastLED.h>

#include "hsv2rgb.h"

#define MAX_PIXEL_COUNT 199
#define NUM_LEDS MAX_PIXEL_COUNT  // needed for some legacy code.

#define Color3i CRGB
#define ColorHSV CHSV

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#if 0
inline void Set(CRGB* leds, const CRGB& c, int start_idx, int n) {
  for (int i = 0; i < n; ++i) {
    //leds[start_idx+i] = c;
    int idx = start_idx+i;
    CRGB& l = leds[idx];
    l = c;
  }
}

inline void SetAll(CRGB* leds, const CRGB& c) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = c;
  }
}

#else

void SetAll(CRGB* leds, const CRGB& c);
void Set(CRGB* leds, const CRGB& c, int start_idx, int n);
void ApplyBendMultipley(CRGB* leds, Color3i color_to_blend);

#endif



#if 0
#include <noise.h>
#include <bitswap.h>
#include <fastspi_types.h>
#include <pixelset.h>
#include <fastled_progmem.h>
#include <led_sysdefs.h>
#include <hsv2rgb.h>
#include <fastled_delay.h>
#include <colorpalettes.h>
#include <color.h>
#include <fastspi_ref.h>
#include <fastspi_bitbang.h>
#include <controller.h>
#include <fastled_config.h>
#include <colorutils.h>
#include <chipsets.h>
#include <pixeltypes.h>
#include <fastspi_dma.h>
#include <fastpin.h>
#include <fastspi_nop.h>
#include <platforms.h>
#include <lib8tion.h>
#include <cpp_compat.h>
#include <fastspi.h>
#include <dmx.h>
#include <power_mgt.h>
#endif



// This version is used for Interstellar Transmissions drum kit.



#if 0
#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>
#endif

#include <SpectrumAnalyzer.h>
#include <Coroutine.h>

#include <NewDeleteExt.h>
#include <Vector.h>
#include <CircularArray.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "PersistantMemory.h"
#include "PixelArray.h"
#include "Visualizer.h"
#include "TwoWireButton.h"
#include "simpletimer.h"
#include "LengthSelectionUI.h"
#include "DrumVis.h"


#define USE_POWER_BUTTON 0
#define DEBUG_PRINT_EQUALIZER 1

#define SERIAL_BAUD_RATE 9600
#define NEOPIXEL_PIN_strip 11

CRGB leds[NUM_LEDS];

// Pins for the buttons
CountingToggleTwoWireButton drum_mode_button(7);
//TwoWireButton drum_mode_button(7);
TwoWireButton power_button(8);
TwoWireButton more_pixels_button(9);
TwoWireButton less_pixels_button(10);

SpectrumAnalyzer spectrum_analyser_;

SimpleTimer timer;

// Collection of visualizers.
Visualizer vis1;
Visualizer2 vis2;
Visualizer3 vis3;

// Controls visualations for drum mode.
DrumVis drum_vis;
DrumVis2 drum_vis2;

IVisualizer* rotating_vis[] = {&vis1, &vis2, &vis3};
RotatingVis rot_vis(rotating_vis, 3);

LengthSelectionUI length_selection_ui(&more_pixels_button, &less_pixels_button);

void UpdateLoop();

#if 0

// This is implimented for future pixel arrays that are not Adafruit
static AbstractPixelArray* PixelArray() {
  if (true) {
    // Parameter 1 = number of pixels in strip
    // Parameter 2 = pin number (most are valid)
    // Parameter 3 = pixel type flags, add together as needed:
    //   NEO_RGB     Pixels are wired for RGB bitstream
    //   NEO_GRB     Pixels are wired for GRB bitstream
    //   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
    //   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
    static Adafruit_NeoPixel* strip = new Adafruit_NeoPixel(
       MAX_PIXEL_COUNT,
       NEOPIXEL_PIN_strip,
       NEO_GRB + NEO_KHZ800);
    static NeoPixelArray* neo_p_array = new NeoPixelArray(strip);
    return neo_p_array; 
  } else {
    return NULL; 
  }
}
#endif

#define PIN_DATA 11
#define PIN_CLOCK 13
#define PIN_CLOCK_MHZ 4
#define LED_ORDER RGB
#define GLOBAL_BRIGHTNESS 255

void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("STARTING UP");
  //active_button.Invert(true);
  timer.setInterval(16, UpdateLoop);
  //int n = SavedState::Instance().PixelCount();
  //PixelArray()->SetLength(n);
  //PixelArray()->Begin();
  
  // Graphics setup.
  FastLED.addLeds<P9813, PIN_DATA, PIN_CLOCK, LED_ORDER,
                  DATA_RATE_MHZ(PIN_CLOCK_MHZ)>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  
  Serial.println("STARTUP FINISHED");
}




void UpdateLoop() {
  //AbstractPixelArray* pixel_array = PixelArray();
  // Fades the rope by 50% on each color.
  //pixel_array->ApplyBlendMultiply(Color3i(128, 128, 128));
  ApplyBendMultipley(leds, Color3i(128, 128, 128));

  
  
  
  SpectrumAnalyzer::Output mixed_mono;
  spectrum_analyser_.ProcessAudioMaxChannel(&mixed_mono);
  
  
  #if DEBUG_PRINT_EQUALIZER
  mixed_mono.DebugPrint(Serial);
  #endif  // DEBUG_PRINT_EQUALIZER
  
  IVisualizer* visualizers[] = {
    //&vis1,
    //&vis2,
    //&vis3,
    &rot_vis,
    &drum_vis,
    &drum_vis2,
  };
  
  const int n_visualizers = sizeof(visualizers) / sizeof(*visualizers);
  
  int drum_mode_state = drum_mode_button.Value() % n_visualizers;
  
  
  static const Color3i color_palette[] = {
    Color3i::Red,
    Color3i::Orange,
    Color3i::Yellow,
    Color3i::Green,
    Color3i::Cyan,
    Color3i::Blue,
    Color3i::Purple,
  };
  
  IVisualizer* vis = visualizers[drum_mode_state];
  vis->Update(color_palette, mixed_mono, pixel_array);

  pixel_array->Draw();
}


void ShowBlack() {
  //AbstractPixelArray* array = PixelArray();
  Color3i black(0, 0, 0);
  for (int i = 0; i < NUM_LEDS; ++i) {
    //array->Set(i, black);
    leds[i] = black;
  }
  //array->Draw();
  FastLeds.show();
}


void loop(void) {
  
  // TEST!
  static int s_counter = 0;
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = i == s_counter ? CRGB::Green : CRGB::Black;
  }
  s_counter = (s_counter + 1) % NUM_LEDS;
  FastLED.show();
  //Serial.println("TEST DRAW");
  return;

  #if 0
  AbstractPixelArray* array = PixelArray();
  // Blank out the light strips.
  if (power_button.Value() && USE_POWER_BUTTON) {
    array->PaintBlack();
    array->Draw();
    delay(100);
    return;
  }
  
  if (more_pixels_button.Value() || less_pixels_button.Value()) {
    array->SetRotationalOffset(0);
    length_selection_ui.Run(array);
    return;
  } else {
    // Run the scheduled algorithm.
    timer.run();  // Runs UpdateLoop()
  }
  #endif
}

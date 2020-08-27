

#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>
#include <SpectrumAnalyzer.h>
#include <Coroutine.h>

#include <NewDeleteExt.h>
#include <Vector.h>
#include <CircularArray.h>
#include <Adafruit_NeoPixel.h>

#include "Painter.h"
#include "Visualizer.h"
#include "TwoWireButton.h"
#include "simpletimer.h"

#define USE_BLACKOUT_BUTTON 0


#define SERIAL_BAUD_RATE 57600
#define NEOPIXEL_PIN_STRIP1 11
#define NEOPIXEL_PIN_STRIP2 13
#define N_ROPE_PIXELS 20

#define ACTIVE_BUTTON_PIN 11

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(
   N_ROPE_PIXELS,
   NEOPIXEL_PIN_STRIP1,
   NEO_GRB + NEO_KHZ800);
   
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(
   N_ROPE_PIXELS,
   NEOPIXEL_PIN_STRIP2,
   NEO_GRB + NEO_KHZ800);

TwoWireButton active_button(ACTIVE_BUTTON_PIN);

SpectrumAnalyzer spectrum_analyser_;

SimpleTimer timer;

FrameBuffer led_rope_1(N_ROPE_PIXELS);
FrameBuffer led_rope_2(N_ROPE_PIXELS);

Visualizer vis1;
Visualizer2 vis2;
Visualizer3 vis3;

void UpdateLoop();

void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("STARTING UP");
  strip1.begin();
  strip2.begin();
  active_button.Invert(true);
  timer.setInterval(16, UpdateLoop);
  Serial.println("STARTUP FINISHED");
}


void PaintChannel(const SpectrumAnalyzer::Output& channel,
                  int visualizer_rotation,
                  FrameBufferBase* pixel_strip) {

  //  sound_equalizer.PrintBands(sound_equalizer_output); 
  // Fades the rope by 50% on each color.
  pixel_strip->ApplyBlendMultiply(Color3i(128, 128, 128));

  static Color3i color_palette[] = {
    Color3i::Red(),
    Color3i::Orange(),
    Color3i::Yellow(),
    Color3i::Green(),
    Color3i::Cyan(),
    Color3i::Blue(),
    Color3i(255, 0, 255), // purple
  };


  if (visualizer_rotation < 500) {
    spectrum_analyser_.set_gain(1.0f);
    vis3.Update(color_palette, channel, pixel_strip);
  } else if (visualizer_rotation < 1000) {
    spectrum_analyser_.set_gain(1.0f);
    vis2.Update(color_palette, channel, pixel_strip); 
  } else {
    spectrum_analyser_.set_gain(2.0f);
    vis1.Update(color_palette, channel, pixel_strip); 
  }
}
                  


void UpdateLoop() {
  SpectrumAnalyzer::Output left, right;
  spectrum_analyser_.ProcessAudio(&left, &right);
  
  left.DebugPrint(Serial);
  right.DebugPrint(Serial);
  
  static int counter = 0;
  counter = (counter + 1) % 1500;
  
  static int draw_offset = 0;
  if (counter % 10 == 0) {
    draw_offset = (draw_offset + 1) % N_ROPE_PIXELS;
  }
  
  PaintChannel(left, counter, &led_rope_1);
  PaintChannel(right, counter, &led_rope_2); 

  for (int i = 0; i < N_ROPE_PIXELS; ++i) {
    Color3i c = led_rope_1.Get((i + draw_offset) % N_ROPE_PIXELS);
    strip1.setPixelColor(i, c.r_, c.g_, c.b_);
    c = led_rope_2.Get((i + draw_offset) % N_ROPE_PIXELS);
    strip2.setPixelColor(i, c.r_, c.g_, c.b_);
  }
  strip1.show();
  strip2.show();
}


void ShowBlack() {
  for (int i = 0; i < N_ROPE_PIXELS; ++i) {
    strip1.setPixelColor(i, 0, 0, 0);
    strip2.setPixelColor(i, 0, 0, 0);
  }
  strip1.show();
  strip2.show();
}


void loop(void) {
  // Blank out the light strips.
  if (active_button.Value() && USE_BLACKOUT_BUTTON) {
    ShowBlack();
    delay(100);
    return;
  } else {
    // Run the scheduled algorithm.
    timer.run();
  }
}

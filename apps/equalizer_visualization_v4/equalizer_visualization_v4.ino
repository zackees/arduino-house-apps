// This version is used as the reference for Cool Neons total control lighting

#define MAX_PIXEL_COUNT 300  // Maximum number of pixels that can be written to


#include <SPI.h>
#include <TCL.h>
#include <LedRopeTCL.h>


#include <SpectrumAnalyzer.h>
#include <Coroutine.h>

#include <NewDeleteExt.h>
#include <Vector.h>
#include <CircularArray.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "PersistantMemory.h"
#include "PixelArray.h"
#include "Painter.h"
#include "Visualizer.h"
#include "TwoWireButton.h"
#include "simpletimer.h"
#include "LengthSelectionState.h"


#define USE_POWER_BUTTON 1
#define DEBUG_PRINT_EQUALIZER 0

#define SERIAL_BAUD_RATE 57600
#define NEOPIXEL_PIN_strip 11
#define N_ROPE_PIXELS 150

// Pins for the buttons
TwoWireButton more_pixels_button(9);
TwoWireButton less_pixels_button(10);
TwoWireButton power_button(8);


SpectrumAnalyzer spectrum_analyser_;

SimpleTimer timer;

Visualizer vis1;
Visualizer2 vis2;
Visualizer3 vis3;

LengthSelectionState length_selection_state(&more_pixels_button, &less_pixels_button);

void UpdateLoop();

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
       N_ROPE_PIXELS,
       NEOPIXEL_PIN_strip,
       NEO_GRB + NEO_KHZ800);
       strip->begin();
    static NeoPixelArray* neo_p_array = new NeoPixelArray(strip);
    return neo_p_array; 
  } else {
    return NULL; 
  }
}


void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("STARTING UP");
  //active_button.Invert(true);
  timer.setInterval(16, UpdateLoop);
  int n = SavedState::Instance().PixelCount();
  PixelArray()->SetLength(n);
  Serial.println("STARTUP FINISHED");
}



void PaintChannel(const SpectrumAnalyzer::Output& channel,
                  int visualizer_rotation,
                  AbstractPixelArray* pixel_strip) {

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
  AbstractPixelArray* pixel_array = PixelArray();
  SpectrumAnalyzer::Output mixed_mono;
  spectrum_analyser_.ProcessAudioMaxChannel(&mixed_mono);
  
  
  #if DEBUG_PRINT_EQUALIZER
  left.DebugPrint(Serial);
  right.DebugPrint(Serial);
  #endif  // DEBUG_PRINT_EQUALIZER
  
  static int counter = 0;
  counter = (counter + 1) % 1500;
  
  static int draw_offset = 0;
  if (counter % 10 == 0) {
    draw_offset = (draw_offset + 1) % pixel_array->Length();
  }
  

  pixel_array->SetOffsetRotation(draw_offset);
  
  PaintChannel(mixed_mono, counter, pixel_array);  
  pixel_array->Draw();
}


void ShowBlack() {
  AbstractPixelArray* array = PixelArray();
  Color3i black(0, 0, 0);
  for (int i = 0; i < array->Length(); ++i) {
    array->Set(i, black);
  }
  array->Draw();
}


void loop(void) {

  AbstractPixelArray* array = PixelArray();
  // Blank out the light strips.
  if (power_button.Value() && USE_POWER_BUTTON) {
    array->PaintBlack();
    array->Draw();
    delay(100);
    return;
  }
  
  if (more_pixels_button.Value() || less_pixels_button.Value()) {
    length_selection_state.Run(array);
    return;
  }
  
  
  else {
    // Run the scheduled algorithm.
    timer.run();
  }
}

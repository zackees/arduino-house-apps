


// This version is used for Interstellar Transmissions drum kit.

#define MAX_PIXEL_COUNT 150  // Maximum number of pixels that can be written to


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

#include "PixelArray.h"
#include "Visualizer.h"
#include "TwoWireButton.h"
#include "simpletimer.h"
#include "DrumVis.h"



#define USE_POWER_BUTTON 0
#define DEBUG_PRINT_EQUALIZER 0

#define SERIAL_BAUD_RATE 9600
#define NEOPIXEL_PIN_A 9
#define NEOPIXEL_PIN_B 8

// Pins for the buttons
//CountingToggleTwoWireButton drum_mode_button(7);
//TwoWireButton drum_mode_button(7);
//TwoWireButton power_button(8);

CountingToggleTwoWireButton mode_button(7);



SpectrumAnalyzer spectrum_analyser_;

SimpleTimer timer;

// Collection of visualizers.
Visualizer vis1;
Visualizer2 vis2;
Visualizer3 vis3;
OffVis offVis;

// Controls visualations for drum mode.
DrumVis drum_vis;
DrumVis2 drum_vis2;

IVisualizer* rotating_vis[] = {&vis1, &vis2, &vis3};
RotatingVis rot_vis(rotating_vis, 3);

void UpdateLoop();

// This is implimented for future pixel arrays that are not Adafruit
static AbstractPixelArray* PixelArray() {
    static Adafruit_NeoPixel* stripA = new Adafruit_NeoPixel(
       MAX_PIXEL_COUNT,
       NEOPIXEL_PIN_A,
       NEO_GRB + NEO_KHZ800);
    static Adafruit_NeoPixel* stripB = new Adafruit_NeoPixel(
       MAX_PIXEL_COUNT,
       NEOPIXEL_PIN_B,
       NEO_GRB + NEO_KHZ800);
    static MultiNeoPixelArray* pixel_array = new MultiNeoPixelArray(stripA, stripB);
    return pixel_array; 
}


void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("STARTING UP");
  //active_button.Invert(true);
  timer.setInterval(8, UpdateLoop);
  PixelArray()->Begin();
  Serial.println("STARTUP FINISHED");
}

     


void UpdateLoop() {
  AbstractPixelArray* pixel_array = PixelArray();
  

  
  // Fades the rope by 50% on each color.
  pixel_array->ApplyBlendMultiply(Color3i(128, 128, 128));
  

  
  
  SpectrumAnalyzer::Output left, right;
  spectrum_analyser_.ProcessAudioMaxChannel(&left);
  //delay(1);
  

  
  #if DEBUG_PRINT_EQUALIZER
  left.DebugPrint(Serial);
  right.DebugPrint(Serial);
  #endif  // DEBUG_PRINT_EQUALIZER
  
  IVisualizer* visualizers[] = {
    &offVis,
    &vis1,
    &vis2,
    &vis3,
    &rot_vis,
    //&drum_vis,
   // &drum_vis2,
  };
  
  const int n_visualizers = sizeof(visualizers) / sizeof(*visualizers);
  
  int mode_select = mode_button.Value() % n_visualizers;
  
  
  static const Color3i color_palette[] = {
    Color3i::Red(),
    Color3i::Orange(),
    Color3i::Yellow(),
    Color3i::Green(),
    Color3i::Cyan(),
    Color3i::Blue(),
    Color3i::Purple(),
  };
  


  IVisualizer* vis = visualizers[mode_select];
  vis->Update(color_palette, left, pixel_array);

  pixel_array->Draw();
}


void ShowColor(const Color3i& c) {
  AbstractPixelArray* array = PixelArray();
  for (int i = 0; i < array->Length(); ++i) {
    array->Set(i, c);
  }
  array->Draw();
}

void ShowBlack() {
  ShowColor(Color3i(0,0,0));
}



void loop(void) {  
  AbstractPixelArray* array = PixelArray();
  

  // Run the scheduled algorithm.
  timer.run();  // Runs UpdateLoop()

}

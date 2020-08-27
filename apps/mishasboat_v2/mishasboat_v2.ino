

#include "Viz.h"

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
#include "TwoWireButton.h"
#include "simpletimer.h"

#define MAX_PIXEL_COUNT 200  // Maximum number of pixels that can be written to
#define DEBUG_PRINT_EQUALIZER 0

#define SERIAL_BAUD_RATE 9600
#define NEOPIXEL_PIN_A 9
#define NEOPIXEL_PIN_B 8

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

void setup() {
  // put your setup code here, to run once:
  PixelArray()->Begin();
  Serial.begin(9600);
}



void loop() {


  unsigned long now = millis() / 5ul;

  int n = PixelArray()->Length();
  for (int i = 0; i < n; ++i) {
    int r = 0;
    r = inoise8(now + i*20);
    
    static int min = 0;
    static int max = 255;
    static int avg = 128;

    if (r < avg) { r = 0; }
    else { r -= avg; }
    
    
    r = map(r, 0, 127, 0, 255);
    //Serial.print(r); Serial.print(",");
    int g = 0;
    int b = inoise8(now + -i*10 - 2000);
    
    if (b < avg) { b = 0; }
    else { b -= avg; }
    
    
    b = map(b, 0, 127, 0, 255);
    
    
    Color3i c(r, 0 , b/*b>>4*/);
    PixelArray()->Set(i, c);
    //PixelArray()->PaintBlack();
  }
  //Serial.println("");
  
  PixelArray()->Draw();
  
  
}

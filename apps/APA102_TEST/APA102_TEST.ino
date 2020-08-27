


//#include "SPI.h"

    #include <stdint.h>
#include "Softspi.h"

#define NUM_LEDS 1000

#define MOSI_PIN 12 //7
#define SCK_PIN 13 //14

#include "FastLED.h"

#define DATA_PIN 7
#define CLOCK_PIN 14

CRGB leds[NUM_LEDS];

// template<uint8_t MisoPin, uint8_t MosiPin, uint8_t SckPin, uint8_t Mode = 0>
// class SoftSPI {
  
SoftSPI<23, MOSI_PIN, SCK_PIN> softSpi;


void setup() {
  Serial.begin(9600);
  softSpi.begin();
}


void SendSPI(byte ctr, byte r, byte b, byte g) {
  //SPI.transfer(ctr);
  //SPI.transfer(g);
  //SPI.transfer(b);
  //SPI.transfer(r); 
}

void SetAllBlack() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
}

void Draw() {
  #if 1
  static bool initialized = false;
  
  if (!initialized) {
    initialized = true;
    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(4)>(leds, NUM_LEDS);
    FastLED.show();
  }
  FastLED.show();
  FastLED.show();
  
  #else
  
  #define SH(X) softSpi.send(X);
  #define SH4(A,B,C,D) SH(A); SH(B); SH(C); SH(D);
  
  for (int i = 0; i < NUM_LEDS; ++i) {
    CRGB datum = leds[i];
    SH4(0xff, datum.g, datum.b, datum.r);
  }
  
  SH4(0, 0, 0, 0);
  
  #endif
}

void loop() {
  SetAllBlack();

  leds[0] = CRGB::White;
  leds[NUM_LEDS>>2] = CRGB::White;
  leds[NUM_LEDS-1] = CRGB::White;
  Draw();
  delay(1000);
  Draw();
  delay(1000);
  
  static int whichColor = 0;

  
  for (int i = 0; i < NUM_LEDS; ++i) {
    
    SetAllBlack();
    CRGB p = CRGB::Blue;
    
    switch (whichColor) {
      case 0: { p = CRGB::Red;   break; }
      case 1: { p = CRGB::Green; break; }
      case 2: { p = CRGB::Blue;  break; }
    }
    leds[i] = p;  
    
    // End Frame
    Draw();
    Draw();
    Serial.println("DrawFrame");
    delay(10);

  }
  
  
  whichColor = (whichColor + 1) % 3;


}

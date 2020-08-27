
#define DBG_TIMER 0  // enables performance metrics dump to the console.
#define DBG_PRINT_AUDIO 0
#define DBG_PRINT_BUTTON_ST 1
#define DBG_RAINBOW_VIS 0

#define ARRAYSIZE(X) (sizeof(X) / sizeof(*X))

#include "Dbg.h"

#include "Audio.h"
#include "Input.h"

#include "OctoWS2811.h"
#include "Gfx.h"

#include <SPI.h>
#include <SdFat.h>
// ****
// * Sd card needs to be declared before the following headers.
SdFat SD;
#include "Movie.h"

#include "FastLED.h"
#include "Vis.h"
#include "Control.h"




void setup() {
  Serial.begin(115200);
  SPLN("setup begin...");

  // Delay startup allows the serial port to connect to see startup errors.
  delay(1000);

  
  // put your setup code here, to run once:

  Audio_Setup();
  //Input_Setup();

  Gfx_Setup();
  Movie_Setup();
  SP("setup finished!");
}


struct DeltaTimer {
  DeltaTimer() {
    mPrevTime = millis();
  }

  unsigned long UpdateTimeMs() {
    unsigned long now = millis();
    unsigned long time_delta = now - mPrevTime;
    mPrevTime = now;
    return time_delta;
  }

  float UpdateTimeSeconds() {
    unsigned long dtime = UpdateTimeMs();
    if (dtime == 0ul) { return 0.0f; }
    
    return float(dtime) / 1000.f;
  }

  unsigned long mPrevTime;
};




void loop() {
  PERF_TIMER;


  if (false) { TestVis(); return;           }
  
  Update_ButtonState();
  AudioData data = ReadAudio();

  
  if (DBG_PRINT_AUDIO)     { PrintAudio(data);   }
  if (DBG_PRINT_BUTTON_ST) { PrintButtonState(); }
  if (DBG_RAINBOW_VIS)     { TestRainbowVis();   }

/*
  if (ButtonChanged(kButtonA)) {
    SP("A "); SPLN(ButtonPressed(kButtonA) ? "pressed" : "released");
  }

  if (ButtonChanged(kButtonB)) {
    SP("B "); SPLN(ButtonPressed(kButtonB) ? "pressed" : "released");
  }
  */

  if (ButtonChanged(kButtonA) && ButtonPressed(kButtonA)) {
    static int s_mode = 0;
    switch (s_mode) {
      case 0: { Control_SetVis(MOVIE);       break; }
      case 1: { Control_SetVis(RAINBOW_VOL); break; }

    }
    s_mode = (s_mode + 1) % 2;
  }

  if (ButtonChanged(kButtonB) && ButtonPressed(kButtonB)) {
    static int s_mode = 0;
    switch (s_mode) {
      case 0: { Control_SetVis(WHITE);            break; }
      case 1: { Control_SetVis(RED);              break; }
      case 2: { Control_SetVis(GREEN);            break; }
      case 3: { Control_SetVis(BLUE);             break; }
      case 4: { Control_SetVis(RAINBOW);          break; }
      case 5: { Control_SetVis(NOISE_RED_BLUE);   break; }
      case 6: { Control_SetVis(NOISE_GREEN_BLUE); break; }
      case 7: { Control_SetVis(NOISE_RAINBOW);    break; }
    }
    
    s_mode = (s_mode + 1) % 8;
  }


  if (ButtonChanged(kButtonC) && ButtonPressed(kButtonC)) {
    SPLN("Increase brightness");
    Gfx_SetBrightness(Gfx_GetBrightness() + 32);
  }

  if (ButtonChanged(kButtonD) && ButtonPressed(kButtonD)) {
    SPLN("Decrease brightness");
    Gfx_SetBrightness(Gfx_GetBrightness() - 32);
  }

  
  //TestRainbowVis();
  Control_Draw(data);
}


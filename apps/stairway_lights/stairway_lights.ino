


//#define PIR_PIN 2
#define ENABLE_TOP_OF_STAIRS_SENSOR

#define COM_PIN 2
#define USE_IR_RANGE_FINDER
#define N_PIXELS 150
//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define DPRINT(X) Serial.print(X)
#define DPRINTLN(X) Serial.println(X)
#else
#define DPRINT(X)
#define DPRINTLN(X)
#endif

#include <SPI.h>
#include "TCL.h"
#include "LedRopeTCL.h"
#include "RangeFinderSharpIR.h"
#include "pir_sensor.h"

#include "./Coroutine.h"
#include "Sensors.h"
#include "Painter.h"
#include "Particles.h"

// #include "NewDeleteExt.h"

#include "Vector.h"




LedRopeTCL led_rope(N_PIXELS);


Sensors sensors;
Painter painter(&led_rope);
AbstractCoroutine* system_components[] = {
  &sensors,
  &painter,
};

CoroutineDispatch co_system(system_components);

void setup(void) {
  Serial.begin(9600);
  pinMode(COM_PIN, INPUT);
  //pinMode(PIR_PIN, INPUT);
  led_rope.FillColor(Color3i::Black());
  led_rope.Draw();
  delay(250);
  led_rope.FillColor(Color3i::Red());
  led_rope.Draw();
  delay(250);
  led_rope.FillColor(Color3i::Green());
  led_rope.Draw();
  delay(250);
  led_rope.FillColor(Color3i::Blue());
  led_rope.Draw();
  delay(250);  
}

void loop(void) {
  co_system.Update();
  
  const bool top_of_stairs_presence = (digitalRead(COM_PIN) == LOW);
  const bool downstairs_presence =  sensors.DownStairsPresence();
  
  bool sensed = downstairs_presence;
  
  #ifdef ENABLE_TOP_OF_STAIRS_SENSOR
  sensed = sensed || top_of_stairs_presence;
  #endif
  
  
  DPRINT("sensed: "); DPRINT(sensed);
  if (sensed) {
    painter.OnDownStairsPresence();
  }
  
  DPRINTLN("");
}

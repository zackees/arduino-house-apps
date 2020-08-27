

#include <Adafruit_NeoPixel.h>
#include "Accelerometer.h"
#include "Physics.h"

#define PIN 4
#define N_LEDS 7

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);


void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
   strip.begin();

}

float fabs(float a) {
  if (a < 0.0) return -a;
  return a;
}



void loop()
{
  
  static bool s_initialized = false;
  if (!s_initialized) {
    Serial.println("Initializing");
    delay(2000);
    s_initialized = true;
    for (int i = 0; i < N_LEDS; ++i) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
  }
  
  
  float dt = DeltaTime();

  float x = 0;
  float y = 0;
  float z = 0;
  
  ReadAccelerometer(&x, &y, &z);
  
  if (fabs(x) < .6f) {
    ResetSpring();
    x = 0;
  }
  
  if (fabs(x) > 1) {
    x = x > 0 ? 1.0 : -1.0;
  }
  
  
  float spring_position = CalcSpringPosition(x * 10, dt);
  
  Serial.print("spring_position: "); Serial.println(spring_position);
 
  bool on = fabs(spring_position) > .4f;
  
  Serial.print("is "); Serial.println(on);
  
  strip.setPixelColor(0, on ? 255 : 0, 0, 0);
    
  strip.show();
}

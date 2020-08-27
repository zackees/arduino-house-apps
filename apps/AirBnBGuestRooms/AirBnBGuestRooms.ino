#define NUM_LEDS 1000
#define LED_DATA_PIN 11
#define LED_CLOCK_PIN 13

#define MOTION_PIN 2





#include <FastLED.h>

#include "SimplexNoise.h"

CRGB leds[NUM_LEDS];



void setup() {

  pinMode(MOTION_PIN, INPUT);

  FastLED.addLeds<APA102,  LED_DATA_PIN, LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(6)>(leds, NUM_LEDS);

  // Enables Serial.print(...) and
  // Serial.println(...) to work.
  Serial.begin(115200);  
  
  // This will setup the hardware for the strip and paint a black frame.
  //strip.begin();
  FastLED.show();
}


void FillBlack(CRGB* strip) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    strip[i] = CRGB::Black;
  }
}

class MotionDetector {
 public:
  MotionDetector(int pin) : mPin(pin), mVal(false) {}

  void Update() {
    mVal = (digitalRead(mPin) == HIGH);
  }
  bool Value() { return mVal; }

  int mPin;
  bool mVal;
  unsigned long time_start;
};

class DurationTimer {
 public:
  DurationTimer() : mStartTime(0), mDurationMs(0) {}

  void Restart(unsigned long duration_ms) {
    mStartTime = millis();
    mDurationMs = duration_ms;
  }

  
  bool Active() const {
    return (mDurationMs > (millis() - mStartTime));
  }

  unsigned long mStartTime;
  unsigned long mDurationMs;
};

static int global_brightness = 0;



MotionDetector motionDetector(MOTION_PIN);
DurationTimer activeTimer;



void loop() {


  static bool has_override = false;
  static bool override_value = false;
  if (Serial.available()) {
    has_override = true;
    override_value = Serial.read() == '1';
  }

  // test fucnction for the pir motion sensor.
  motionDetector.Update();
  
  if (motionDetector.Value()) {
    activeTimer.Restart(1000 * 60 * 10);
    //Serial.print(millis()); Serial.println(": triggered");
  } else {
    //Serial.print(millis()); Serial.println(": not triggered");
  }

  

  unsigned long time_now = millis();

  bool increase_brightness = false;
  if (has_override) { increase_brightness = override_value; }
  else if (activeTimer.Active()) { increase_brightness = true; }

  if (increase_brightness) {
    global_brightness += 4;
  } else {
    global_brightness--;
  }


  if (global_brightness < -255) {
    global_brightness = -255;
  } else if (global_brightness > 0) {
    global_brightness = 0;
  }



  {
    unsigned long start_t = millis();
    unsigned long time_now = start_t;


    NoiseGenerator noiseGeneratorRed (500, 14);
    NoiseGenerator noiseGeneratorBlue (500, 10);
  

/*
    for (int i = 0; i < 20; ++i) {
      uint8_t val = noiseGen.Value(i, start_t);
      Serial.print(val); Serial.print(",");
    }
    Serial.println();
    */

/*
    fill_noise16(leds, NUM_LEDS,
                 8,  // octaves
                 0,  // x
                 2,  // scale
                 4, // hue_octaves
                 0, // hue_x
                 4, //hue_scale,
                 millis() >> 4);


      */



      for (int32_t i = 0; i < NUM_LEDS; ++i) {
       
        int r = noiseGeneratorRed.LedValue(i, time_now);
        int b = noiseGeneratorBlue.LedValue(i, time_now + 100000) >> 1;
        int g = 0;
    
        if (global_brightness < 0) {
          r = max(0, r + global_brightness);
          g = max(0, g + global_brightness);
          b = max(0, b + global_brightness);
        }
   
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
      }


     
      unsigned long delta_t = millis() - start_t;
      Serial.print("strip population took: "); Serial.print(delta_t); Serial.println("ms");
  }


  {
    unsigned long start_t = millis();
    FastLED.show();
    unsigned long delta_t = millis() - start_t;
    Serial.print("strip.show() took: "); Serial.print(delta_t); Serial.println("ms");
  }
  //delay(100);
}

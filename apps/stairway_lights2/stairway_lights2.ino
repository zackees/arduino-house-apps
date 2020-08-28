


#define PIN_STATUS_LED 13
#define PIN_EXTERNAL_SIG 32
#define PIN_PIR 27
#define NUM_LEDS 151

#include "FastLED.h"
CRGB leds[NUM_LEDS];
CRGB display_leds[NUM_LEDS];  // Led's that are displayed.

#define DATA_PIN 8
#define CLOCK_PIN 9


#include "simplex_noise.h"
#include "fire_visualizer.h"
#include "basicfadeingamma.h"
#include "darkness.h"



//#define VIS_DURATION 1000 * 60 * 10  // 10 minutes
#define VIS_DURATION 1000 * 5   // 5 seconds


DarknessPainter darkness_painter;


// P9813

void setup() {
  Serial.begin(9600);  // Doesn't matter for teensy.
  delay(500);
  //FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(display_leds, NUM_LEDS);
  // put your setup code here, to run once:

  Serial.println("Init ok");
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_EXTERNAL_SIG, INPUT);
  pinMode(PIN_PIR, INPUT);
  setup_firevisualizer();
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




void update_status_led() {
  uint32_t time = millis();
  int st = (time % uint32_t(100));
  Serial.println(st);
  digitalWrite(PIN_STATUS_LED, (st < 50) ? HIGH : LOW);
}

void set_status_led(int val) {
  digitalWrite(PIN_STATUS_LED, val);
}



void do_draw_test() {
  // Turn the LED on, then pause
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Red;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(2);
  }
  FastLED.show();
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Green;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(2);
  }
  FastLED.show();
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Blue;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(2);
  }
}



DurationTimer activeTimer;




void vis_loop() {

  // test fucnction for the pir motion sensor.

  bool internal_sig = digitalRead(PIN_PIR) == HIGH;
  int external_sig = digitalRead(PIN_EXTERNAL_SIG) == LOW;

  

  if (external_sig || internal_sig) {
    activeTimer.Restart(VIS_DURATION);
  }

  unsigned long time_now = millis();

  bool increase_brightness = false;
  if (activeTimer.Active()) { increase_brightness = true; }

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
}


void loop() {
  darkness_painter.Update(NUM_LEDS);
  
  bool internal_sig = digitalRead(PIN_PIR) == HIGH;
  int external_sig = digitalRead(PIN_EXTERNAL_SIG) == LOW;

  bool active = internal_sig || external_sig;
  set_status_led(active);
  

  if (active) {
    static uint32_t s_last_time = 0;

    if (s_last_time == 0 || (millis() - s_last_time > 1000 * 10)) {
      darkness_painter.Start();
      s_last_time = millis();
    }
  }

  #if 1
  active = (millis() % 4000ul) < 2000;
  #endif

  
  #if 1
  basicfadeingamma_loop();
  delay(0);
  #elif 0
  //FillBlack(leds);
  int delay_factor = fire_loop(active);
  delay(delay_factor);
  
  #elif 0
  vis_loop();
  #else
  //update_status_led();
  //do_draw_test();

  if (active) {
    do_draw_test();
  }
  #endif


  for (int i = 0; i < NUM_LEDS; ++i) {
    char data[100];
    float b = darkness_painter.Brightness(i);
    //sprintf(data, "[%d]: %f\n", i, b);
    //Serial.print(data);
    //Serial.print('['); Serial.print(i); Serial.print(']: '); Serial.println("");
    if (b <= 0.0f) {
      display_leds[i] = CRGB::Black;
    }
    if (b < 1.0f) {
      display_leds[i] = CRGB(leds[i].r * b, leds[i].g * b, leds[i].b * b);
    } else {
      display_leds[i] = leds[i];
    }
  }

  FastLED.show();
}

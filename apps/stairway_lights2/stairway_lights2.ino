


#include "defs.h"

#include "gfx.h"

#include "simplex_noise.h"
#include "fire_visualizer.h"
#include "basicfadeingamma.h"
#include "darkness.h"
#include "sensors.h"
#include "noisewave_visualizer.h"

DarknessPainter darkness_painter;
// P9813

void setup() {
  Serial.begin(9600);  // Doesn't matter for teensy.
  Serial.println("Init ok");
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_EXTERNAL_SIG, INPUT);
  pinMode(PIN_PIR, INPUT);
  setup_firevisualizer();
  gfx_init();
}


void FillBlack(CRGB* strip) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    strip[i] = CRGB::Black;
  }
}


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
    gfx_show();
    leds[i] = CRGB::Black;
    delay(2);
  }
  gfx_show();
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Green;
    gfx_show();
    leds[i] = CRGB::Black;
    delay(2);
  }
  FastLED.show();
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Blue;
    gfx_show();
    leds[i] = CRGB::Black;
    delay(2);
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

  gfx_show();
}

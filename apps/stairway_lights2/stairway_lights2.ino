

#include "FastLED.h"

#define PIN_STATUS_LED 13
#define PIN_EXTERNAL_SIG 32

#define NUM_LEDS 300
#define DATA_PIN 8
#define CLOCK_PIN 9

CRGB leds[NUM_LEDS];

// P9813

void setup() {
  Serial.begin(9600);  // Doesn't matter for teensy.
  delay(500);
  //FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  // put your setup code here, to run once:

  Serial.println("Init ok");
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_EXTERNAL_SIG, INPUT);                                                                                                                          
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





void loop() {
  //update_status_led();
  //do_draw_test();

  int external_sig = digitalRead(PIN_EXTERNAL_SIG);

  Serial.print("EXTERNAL_SIG: "); Serial.println(external_sig);
  set_status_led(external_sig == HIGH);

  if (!external_sig) {
    do_draw_test();
  }
}

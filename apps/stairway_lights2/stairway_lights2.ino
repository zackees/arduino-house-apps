

#include "FastLED.h"


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
  pinMode(13, OUTPUT);
}

void update_status_led() {
  uint32_t time = millis();
  int st = (time % uint32_t(100));
  Serial.println(st);
  digitalWrite(13, (st < 50) ? HIGH : LOW);
}

void loop() {
  update_status_led();

  // Turn the LED on, then pause
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Red;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(2);
    update_status_led();
  }
  FastLED.show();
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Green;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(2);
    update_status_led();
  }
  FastLED.show();
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Blue;
    FastLED.show();
    leds[i] = CRGB::Black;
    delay(2);
    update_status_led();
  }
}

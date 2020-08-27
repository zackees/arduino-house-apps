/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

 #include "PIRSensor_RadioShack.h"
 
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int led = 13;
int pir_sensor = 12;
uint32_t on_duration = 1000ul * 60ul * 3ul;  // 3 min timer
//uint32_t on_duration = 1000ul * 10ul * 1ul;  // 10 sec timer
int user_button = 8;
int led_strip = 3;
int16_t led_value = 0;
int pir_pwr_pin_on = 2;

#define PRINTOUT_SENSORS

PIRSensor_RadioShack pir(pir_sensor);

class LedController {
 public:
  uint32_t start_time_;
  LedController() {
    start_time_ = millis();
  }

  void Trigger() {
    start_time_ = millis();
  } 
  bool Update() {
    uint32_t now = millis();
    if (pir.MovementDetected()) {
#ifdef PRINTOUT_SENSORS
      Serial.println("Y");
#endif
      static uint32_t i = 0;
      start_time_ = now;
    } else {
#ifdef PRINTOUT_SENSORS
      Serial.println("N");
#endif
    }
    uint32_t delta = now - start_time_;
    return delta < on_duration;
  }
};

LedController led_controller;

bool UserSwitchOn() {
  pinMode(user_button, OUTPUT);
  digitalWrite(user_button, LOW);
  delay(1);
  digitalWrite(user_button, HIGH);
  delay(1);
  pinMode(user_button, INPUT);
  bool on = digitalRead(user_button) == LOW;
  digitalWrite(user_button, LOW);
  return on;
}

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);

  pinMode(pir_pwr_pin_on, OUTPUT);
  digitalWrite(pir_pwr_pin_on, HIGH);
  pinMode(pir_sensor, INPUT);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  //Serial.println("Hi");
  //Serial.println(".");

  static bool s_prev_switch_on = UserSwitchOn();
  bool curr_switch_on = UserSwitchOn();
  Serial.print(curr_switch_on ? "1": "0");
  if (curr_switch_on && (curr_switch_on != s_prev_switch_on)) {
    led_controller.Trigger();
  }
  s_prev_switch_on = curr_switch_on;

  bool updated = led_controller.Update();
  bool triggered = updated && curr_switch_on;
  //debug
  //led_value = UserSwitchOn() ? 255 : 0;

  //Serial.println(triggered ? "Triggered" : "Not");

  if (triggered) {
    led_value += 1;
    if (led_value > 255) {
      led_value = 255;
    }
  } else {
    led_value -= 1;
    if (led_value < 0) {
      led_value = 0;
    }
  }
  
  //Serial.println(led_value);
  digitalWrite(led, led_value == 0 ? LOW : HIGH);
  analogWrite(led_strip, led_value);
}

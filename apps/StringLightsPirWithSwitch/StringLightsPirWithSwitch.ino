/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

 
// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:

#define PIN_STATUS_LED 13
#define PIN_PIR 4
#define PIN_PIR_PWR 2
#define PIN_LED_STRIP 6
#define PIN_USER_BTN 8

#define SERIAL_PORT_BAUD 57600

#define FRAME_TIME_MS 8

//#define DEBUG

#ifdef DEBUG
const uint32_t on_duration = 1000ul * 10ul * 1ul;  // 10 sec timer
#else
const uint32_t on_duration = 1000ul * 60ul * 3ul;  // 3 min timer
#endif

// Let the linker find it.
void(* resetFunc) (void) = 0;

int16_t led_value = 0;

class LedController {
 public:
  uint32_t start_time_;
  LedController() {
    start_time_ = millis();
  }

  void Trigger() {
    start_time_ = millis();
  } 
  bool Update(uint32_t time_now, bool has_movement) {
    uint32_t now = time_now;
    if (has_movement) {
      static uint32_t i = 0;
      start_time_ = now;
    }
    uint32_t delta = now - start_time_;
    return delta < on_duration;
  }
};

LedController led_controller;

bool UserSwitchOn() {
  pinMode(PIN_USER_BTN, OUTPUT);
  digitalWrite(PIN_USER_BTN, LOW);
  delay(0);
  digitalWrite(PIN_USER_BTN, HIGH);
  delay(0);
  pinMode(PIN_USER_BTN, INPUT);
  bool on = digitalRead(PIN_USER_BTN) == LOW;
  digitalWrite(PIN_USER_BTN, LOW);
  return on;
}

void setup() {                
  // initialize the digital pin as an output.
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_PIR_PWR, OUTPUT);
  pinMode(PIN_PIR, INPUT);
  digitalWrite(PIN_PIR_PWR, HIGH);
  pinMode(PIN_LED_STRIP, OUTPUT);
  
  Serial.begin(SERIAL_PORT_BAUD);
}


// the loop routine runs over and over again forever:
void loop() {

  const uint32_t time_now = millis();

  if (false) {
    delay(4000);
    Serial.println("About to reset");
    delay(1000);
    resetFunc();
  }
  
  
  bool curr_switch_on = UserSwitchOn();
  static bool s_prev_switch_on = curr_switch_on;
  if (curr_switch_on && (curr_switch_on != s_prev_switch_on)) {
    led_controller.Trigger();
  }
  s_prev_switch_on = curr_switch_on;

  bool movement_activated = (HIGH == digitalRead(PIN_PIR));
  digitalWrite(PIN_STATUS_LED, movement_activated ? HIGH : LOW);


  bool updated = led_controller.Update(time_now, movement_activated);
  bool triggered = updated && curr_switch_on;

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
  analogWrite(PIN_LED_STRIP, led_value);

#ifdef DEBUG
  char buff[256] = {0};
  sprintf(buff, "movement_activated: %d, curr_switch_on: %d, led_value: %d\n",
          movement_activated, curr_switch_on, led_value);
  Serial.print(buff);
#endif

  while (millis() - time_now < FRAME_TIME_MS) {;}
  
}

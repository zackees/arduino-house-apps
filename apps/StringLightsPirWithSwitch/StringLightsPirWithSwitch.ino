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
#define PIN_LED_STRIP 6
#define PIN_USER_BTN 8

#define SERIAL_PORT_BAUD 57600

#define FRAME_TIME_MS 8

// Prints out the state each frame
#define PRINT_STATE

#define DEBUG

#ifdef DEBUG
const uint32_t on_duration = 1000ul * 10ul * 1ul;  // 10 sec timer
#else
const uint32_t on_duration = 1000ul * 60ul * 3ul;  // 3 min timer
#endif

int16_t led_value = 0;

class AlarmTimer {
 public:
  uint32_t start_time_;
  AlarmTimer() {
    start_time_ = millis();
  }

  void Trigger() {
    start_time_ = millis();
  } 
  bool Update(uint32_t time_now, bool alarm) {
    uint32_t now = time_now;
    if (alarm) {
      static uint32_t i = 0;
      start_time_ = now;
    }
    uint32_t delta = now - start_time_;
    return delta < on_duration;
  }
};

AlarmTimer alarm_timer;

bool probe_user_switch_on() {
  pinMode(PIN_USER_BTN, OUTPUT);
  digitalWrite(PIN_USER_BTN, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_USER_BTN, HIGH);
  delayMicroseconds(5);
  pinMode(PIN_USER_BTN, INPUT);
  bool on = digitalRead(PIN_USER_BTN) == LOW;
  digitalWrite(PIN_USER_BTN, LOW);
  return on;
}

void setup() {                
  // initialize the digital pin as an output.
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_STRIP, OUTPUT);
  Serial.begin(SERIAL_PORT_BAUD);
}

// the loop routine runs over and over again forever:
void loop() {
  const uint32_t time_now = millis();
  bool pwr_btn_on = probe_user_switch_on();
  static bool s_prev_pwr_btn_on = pwr_btn_on;
  if (pwr_btn_on && (pwr_btn_on != s_prev_pwr_btn_on)) {
    alarm_timer.Trigger();
  }
  s_prev_pwr_btn_on = pwr_btn_on;

  bool pir_active = (HIGH == digitalRead(PIN_PIR));
  digitalWrite(PIN_STATUS_LED, pir_active ? HIGH : LOW);

  bool triggered = alarm_timer.Update(time_now, pir_active);

  if (triggered && pwr_btn_on) {
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

#ifdef PRINT_STATE
  char buff[256] = {0};
  sprintf(buff,
         "pwr_btn_on: %d, pir_active: %d, triggered: %d, led: %d\n",
          pwr_btn_on, pir_active, triggered, led_value);
  Serial.print(buff);
#endif

  while (millis() - time_now < FRAME_TIME_MS) {;}
  
}

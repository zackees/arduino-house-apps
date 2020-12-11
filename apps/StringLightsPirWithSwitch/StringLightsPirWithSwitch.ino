/* Hardware hookup guide:
 *  PIR Sensor -
 *    Rails -> Rails
 *    Signal -> Pin 4
 *  Pwr Switch -
 *    Pin 1 -> Ground
 *    Pin 2 -> 8
 *  Led Strip -
 *    Pin 6 -> (Through a n-channel mosfet)
 *    
 *  In the future we can remove the mosfet and use a bunch of pins linked
 *  together.
 */


#include "defs.h"
#include "alarm_timer.h"

int16_t g_led_value = 0;
bool g_first_run = true;
bool g_prev_pwr_btn_on = false;


AlarmTimer alarm_timer;

uint32_t g_earliest_time = millis();
uint32_t g_ts_finish_setup = 0;  // timestamp
uint32_t g_ts_start_loop = 0;

void write_led(int value) {
  analogWrite(PIN_LED_STRIP, value);
}

void setup() {
#ifdef PRINT_STATE
  Serial.begin(SERIAL_PORT_BAUD);
  delay(200);
  Serial.println("Starting up");
#endif
  // initialize the digital pin as an output.
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_STRIP, OUTPUT);
  pinMode(PIN_PWR_SWITCH, INPUT_PULLUP);
  io_setup();
  g_ts_finish_setup = millis();
}

// the loop routine runs over and over again forever:
void loop() {
  const uint32_t time_now = millis();
  const bool first_run = g_first_run;

  if (first_run) {
#ifdef PRINT_STATE
    g_ts_start_loop = millis();
    uint32_t delta = g_ts_start_loop - g_ts_finish_setup;
    char buff[128] = {0};
    sprintf(buff, "g_earliest_time: %"PRIu32", g_ts_finish_setup: %"PRIu32", g_ts_start_loop: %"PRIu32"\n",
            g_earliest_time, g_ts_finish_setup, g_ts_start_loop);
    Serial.println("Sprintf finished");
    Serial.print(buff);
    Serial.println("Finished buff");
    Serial.flush();
    Serial.println("Finished flush()");
    Serial.flush();
    delay(1000);
    Serial.println("Finished!");
#endif
  }
  
  g_first_run = false;
  const bool pwr_btn_on = return digitalRead(PIN_PWR_SWITCH) == LOW;
  if (first_run || (pwr_btn_on && (pwr_btn_on != g_prev_pwr_btn_on))) {
    alarm_timer.Trigger();
  }
  g_prev_pwr_btn_on = pwr_btn_on;

  const bool pir_active = (HIGH == digitalRead(PIN_PIR));
  digitalWrite(PIN_STATUS_LED, pir_active ? HIGH : LOW);

  const uint32_t alarm_time = alarm_timer.Update(time_now, pir_active);
  const uint32_t alarm_remaining = ON_DURATION > alarm_time ? ON_DURATION - alarm_time : 0;
  
  if (!pwr_btn_on) {
    g_led_value = 0;
  } else {
    if (alarm_remaining > 0) {
      g_led_value += 1;
      if (g_led_value > 255) {
        g_led_value = 255;
      }
    } else {
      g_led_value -= 1;
      if (g_led_value < 0) {
        g_led_value = 0;
      }
    } 
  }
  write_led(g_led_value);

#ifdef PRINT_STATE
  char buff[128] = {0};
  sprintf(buff,
         "pwr_btn_on: %d, pir_active: %d, led: %d, alarm_remaining: %"PRIu32"\n",
          pwr_btn_on, pir_active, g_led_value, alarm_remaining);
  Serial.print(buff);
  Serial.flush();
#endif

  while (millis() - time_now < FRAME_TIME_MS) {;}
  
}

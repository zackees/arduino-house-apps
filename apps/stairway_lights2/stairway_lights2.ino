


#include "defs.h"
#include "gfx.h"

#include "simplex_noise.h"
#include "fire_visualizer.h"
#include "basicfadeingamma.h"
#include "darkness.h"
#include "sensors.h"
#include "vis_noisewave.h"

DarknessPainter darkness_painter;
// P9813

void setup() {
  Serial.begin(9600);  // Doesn't matter for teensy.
  Serial.println("Init ok");
  pinMode(PIN_STATUS_LED, OUTPUT);
  setup_firevisualizer();
  gfx_init();
  sensors_init();
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

void loop() {
  darkness_painter.Update(NUM_LEDS);
  bool sensor_active_top = sensor_external_triggered();
  bool sensor_active_bottom = sensor_pir_triggered();
  bool active = sensor_active_top || sensor_active_bottom;
  set_status_led(active);
  int delay_ms = 0;
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


  uint32_t idx = (millis() % 9000ul) / 3001ul;  // 0->2

  switch (idx) {
    case 0: { delay_ms = basicfadeingamma_loop(sensor_active_top, sensor_active_bottom); break; }
    case 1: { delay_ms = fire_loop(sensor_active_top, sensor_active_bottom);             break; }
    case 2: { delay_ms = vis_loop(sensor_active_top, sensor_active_bottom);              break; }
  }

  delay(delay_ms);

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

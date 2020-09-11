


#include "defs.h"
#include "gfx.h"

#include "fx.h"

#include "simplex_noise.h"
#include "vis_fire.h"
#include "vis_rainbowdrops.h"
#include "darkness.h"
#include "sensors.h"
#include "vis_noisewave.h"

#ifdef _DEBUG_TESTING
# define FORCE_ACTIVATION_CYCLE
# define VISUALIZATION_TRANSITION_TIME 1000 * 10
#else
# define VISUALIZATION_TRANSITION_TIME 1000 * 60 * 60 * 1  // 1 hour then transition.
#endif  // _DEBUG_TESTING

//#define FORCED_VIS_INDEX 1  // Fire vis
//#define DBG_FORCE_CLEAR

void setup() {
  Serial.begin(9600);  // Doesn't matter for teensy.
  delay(1000);
  Serial.println("Init ok");
  pinMode(PIN_STATUS_LED, OUTPUT);
  setup_firevisualizer();
  setup_noisewave();
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

uint32_t timed_index(uint32_t now) {
  static uint32_t idx = 0;
  static uint32_t prev = now;
  uint32_t time_elapsed = now - prev;
  if (time_elapsed > VISUALIZATION_TRANSITION_TIME) {
    prev = now;
    // do event here
    idx++;
  }
  return idx;
}

class Timer {
  public:
   Timer() { Reset(0); }
   explicit Timer(uint32_t now) { Reset(now); }
   void Reset(uint32_t start_time) { mStartTime = start_time; }
   uint32_t ElapsedTime(uint32_t now) const { return now - mStartTime; }
   uint32_t mStartTime;
};

void dbg_force_clear(bool* dst) {
  #ifdef DBG_FORCE_CLEAR
  uint32_t now = millis();
  static Timer timer(now);
  uint32_t elapsed_time = timer.ElapsedTime(now);
  Serial.println(elapsed_time);
  if (elapsed_time > 1000) {
    timer.Reset(now);
    *dst = true;
  } else {
  }
  #endif
}


uint32_t not_active_duration(uint32_t now, bool active) {
  static bool s_prev_active = 0;
  static uint32_t s_active_time = 0;

  if ((s_prev_active != active) || (s_prev_active == 0 && active)) {
    // Something changed.
    if (active) {
      s_active_time = now;
    } else {
      s_active_time = 0;  // off
    }
  }
  return now - s_active_time;
}


void loop() {
  uint32_t now = millis();
  bool sensor_active_top = sensor_external_triggered();
  bool sensor_active_bottom = sensor_pir_triggered();
  bool active = sensor_active_top || sensor_active_bottom;
  uint32_t not_active_time = not_active_duration(now, active);

  #ifdef FORCE_ACTIVATION_CYCLE
  active = (now % 8000ul) < 2000;
  sensor_active_top = sensor_active_bottom = active;
  #endif

  set_status_led(active);
  int delay_ms = 0;

  const int kNumVis = 3;

  //uint32_t idx = (millis() % 9000ul) / 3001ul;  // 0->2
  static uint32_t idx = timed_index(now) % kNumVis;
  // After a certain amount of time allow the index to be changed.
  if (not_active_time > 1000 * 60 * 10) {
    idx = timed_index(now) % kNumVis;
  }

  #ifdef FORCED_VIS_INDEX
  idx = FORCED_VIS_INDEX;
  #endif

  static uint32_t prev_idx = idx;
  bool clear = (idx != prev_idx);

  dbg_force_clear(&clear);
  if (clear) {
    gfx_clear();
  }

  prev_idx = idx;
  switch (idx) {
    case 0:  { delay_ms = basicfadeingamma_loop(clear, sensor_active_top, sensor_active_bottom); break; }
    case 1:  { delay_ms = fire_loop(clear, sensor_active_top, sensor_active_bottom);             break; }
    case 2:  { delay_ms = noisewave_loop(clear, sensor_active_top, sensor_active_bottom);        break; }
  }
  delay(delay_ms);

  // Copy to display leds.
  memcpy(display_leds, leds, sizeof(leds));
  fx_update(now, active, display_leds, NUM_LEDS);
  gfx_show();

}

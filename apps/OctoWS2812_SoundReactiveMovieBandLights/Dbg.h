
#include "Arduino.h"

#ifndef DBG_TIMER
#define DBG_TIMER 0
#endif

#if(DBG_TIMER)
 #define PERF_TIMER PerfTimer __perfTimer(__FUNCTION__)
#else
 #define PERF_TIMER
#endif  // DBG_TIMER


#define SP(X) Serial.print(X)
#define SPLN(X) Serial.println(X)

#define SP_VAR(X) SP(#X); SP(": "); SP(X);
#define SPLN_VAR(X) SP_VAR(X); SPLN("");

// TODO: have a conditional on the DPRINT
#define DPRINT(X) SP(X)
#define DPRINTLN(X) SPLN(X)

struct PerfTimer {

  unsigned long start_t;
  bool stopped;
  const char* label;
  static int s_num_active;
  
  PerfTimer(const char* l) : start_t(millis()), stopped(false), label(l) {
    PrintIndent(); Serial.print(label); Serial.println(" start...");
    s_num_active++;
  }

  void stop() {
    if (stopped == true) { return; }
    stopped = true;
    --s_num_active;
    unsigned long delta_t = millis() - start_t;
    PrintIndent();
    Serial.print(label); Serial.print(" took "); Serial.print(delta_t); Serial.println(" ms");
  }

  void PrintIndent() {
    for (int i = 0; i < s_num_active; ++i) {
      Serial.print("  ");
    }
  }

  ~PerfTimer() { stop(); }
};

int PerfTimer::s_num_active = 0;


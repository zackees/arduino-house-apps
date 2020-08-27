

#ifndef _HEARTBEAT_H_
#define _HEARTBEAT_H_

#include "ApproximatingFunction.h"

float OneHeartBeat(unsigned long time_now, unsigned long duration_ms) {
   const unsigned long cyc = duration_ms;
  
  time_now = time_now % cyc;
  float time_cyc = float(time_now) / float(cyc);
  
  //Serial.print("time_cyc: "); Serial.println(time_cyc);
  
  typedef InterpData<float, float> Data;
  
  static Data d[] = {
    Data(0.0f, 0.0f),
    Data(0.5f, 0.0f),
    Data(0.8f, 1.f),  // End of first heartbeat contraction
    Data(0.9f, .8f),
    Data(1.f, 0.0f)
  };
  

  const int N = sizeof(d) / sizeof(*d);
  
  
  float val = Interp<float, float>(time_cyc, d, N);
  return pow(val, 2.8f);
}


float TwoHeartBeat(unsigned long time_now, unsigned long duration_ms) {
   const unsigned long cyc = duration_ms;
  
  time_now = time_now % cyc;
  float time_cyc = float(time_now) / float(cyc);
  
  //Serial.print("time_cyc: "); Serial.println(time_cyc);
  
  typedef InterpData<float, float> Data;
  
  static Data d[] = {
    Data(0.0f, 0.0f),
    Data(0.2f, 0.0f),
   // Data(0.1f, 1.0f),
    
    Data(0.8f, 1.f),  // End of first heartbeat contraction
    Data(0.9f, .8f),
    Data(1.f, 0.0f)
  };

  const int N = sizeof(d) / sizeof(*d);
  
  
  float val = Interp<float, float>(time_cyc, d, N);
  return pow(val, 2.8f);
}


#endif // _HEARTBEAT_H_


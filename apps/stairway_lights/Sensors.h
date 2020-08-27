#include <Arduino.h>
#include "Coroutine.h"
#include "RangeFinderSharpIR.h"

#ifndef USE_IR_RANGE_FINDER
#errorDPRINT
#endif

#ifdef USE_IR_RANGE_FINDER
RangeFinderSharpIR ir_range_finder(A5);  // Analguo pin A5

#else


// Latch time is 10 mins.
//const unsigned long kLatchTime = 10 *60 * 1000;
const unsigned long kLatchTime = 10 * 1000;  // 10 seconds.
//PIRSensor_RadioShack proxy_sensor(PIR_PIN, kLatchTime);
#endif

class Sensors : public AbstractCoroutine {
 public:
  Sensors() : down_stairs_presence_(false) {}
  virtual int OnCoroutine() {     
    #ifdef USE_IR_RANGE_FINDER
    down_stairs_presence_ = ir_range_finder.SampleDistanceMeters() < .5;
    DPRINT(","); DPRINT(ir_range_finder.SampleDistanceMeters() );
    #else
    bool is_high = HIGH == digitalRead(PIR_PIN);
    Serial.print("is_high: "); Serial.print(1); Serial.print(" ");
    //down_stairs_presence_ = proxy_sensor.MovementDetected();
    #endif
    return 0;
  }
  
  bool DownStairsPresence() const { return down_stairs_presence_; }
  
  bool down_stairs_presence_;
}; 

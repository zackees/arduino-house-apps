
#define STAT_USE_STDEV  // For Statistic.h
#include <NewDeleteExt.h>

#include <Led.h>
#include <RangeFinderSharpIR.h>
#include <Coroutine.h>
#include <CircularArray.h>
#include <Vector.h>
#include "TempSensor_DHT11.h"


#define PRINT_VAR(X)   Serial.print(#X  " = "); Serial.println(X);

#include "util.h"
#include "LightController.h"
#include "LightBehavior.h"


// Sensors
RangeFinderSharpIR ir_sensor(A0);
TempSensor_DHT11 temp_sensor(12);  // Pin 12.



// Timed rou
LightController light_controller_;
AbstractCoroutine *tasks_for_dispatch[] = { &light_controller_ };
CoroutineDispatch dispatch(tasks_for_dispatch);

// Subsummable actions.
NullAction null_action_(light_controller_);
CooldownAction cooldown_action_(light_controller_);
ProximiyAction proximity_action_(light_controller_);

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {

#if 0
  light_controller_.Test();
  Serial.print("Dist inches: "); Serial.println(ir_sensor.SampleDistanceInches());
  return;
#else  
  int sleep_time = dispatch.Update();
  
  static const int kInvalidTemp = -999999999.0f;
  float temp_f = kInvalidTemp;
  
  #if 0
  TempSensor_DHT11::Status sts = temp_sensor.Update();
  if (sts != TempSensor_DHT11::DHTLIB_OK) {
    Serial.print("temp sensor error: "); Serial.println(sts);
  } else {
    temp_f = temp_sensor.temperature_fahrenheit();
  }
  #endif

  const float dist_meters = ir_sensor.SampleDistanceMeters();
  
  AbstractPriorityAction* subsumable_actions[] = {
    &proximity_action_,
    &cooldown_action_,
    &null_action_
  };
  
  static const int n = sizeof(subsumable_actions) / sizeof(*subsumable_actions);
  
  
  // Update all the subsummable actions.
  for (int i = 0; i < n; ++i) {
    subsumable_actions[i]->Update(dist_meters, temp_f);
  }
  
  // Execute all of the subsummable actions from the most specific
  // to the most general.
  for (int i = 0; i < n; ++i) {
    if (subsumable_actions[i]->Execute()) {
      break;  // This action was able to Execute.
    }
  };

  delay(sleep_time);
#endif
}




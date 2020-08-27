#include "TempSensor_DHT11.h"
#include "Led.h"
#include "Coroutine.h"

class FanController : public AbstractCoroutine {
 public:
  FanController(TempSensor_DHT11* temp_sensor, Led* fan)
    : temp_sensor_(temp_sensor), fan_(fan), prev_fan_power_(0) {}
  
  int OnCoroutine() {
    TempSensor_DHT11::Status sts = temp_sensor_->Update();
    if (sts != 0) {
      prev_fan_power_ = 0;
      return PerformErrorCondition();
    }
    const int temp_f = temp_sensor_->temperature_fahrenheit();
    int fan_power = CalculateFanPower(temp_f);
    
    Serial.print("temperature_fahrenheit = "); Serial.println(temp_f);
    
    if (fan_power == 0) {
      Serial.print("temperature too low, turning off fan");
      fan_->Off();
      prev_fan_power_ = 0;
      return 1000;  // 1 second delay before the temp is checked again.
    }
    
    if (prev_fan_power_ == 0) {
      prev_fan_power_ = fan_power; 
    }

    fan_power = Step(prev_fan_power_, fan_power, 5);
    prev_fan_power_ = fan_power;
    
    Serial.print("fan_power = "); Serial.println(fan_power);
    
    fan_->Set(fan_power);
    return 3000;  // 3 second delay before next update.
  }
  
  static int Step(int old_value, int new_value, int delta_step) {
    if (abs(new_value - old_value) <= delta_step)
      return new_value;
    if (new_value > old_value) {
      return old_value + delta_step; 
    } else {
      return old_value - delta_step;
    }
  }
  
  int PerformErrorCondition() {
    TempSensor_DHT11::Status sts = temp_sensor_->Update();
    if (sts != 0) {
      Serial.println("Status code was error\nTurning fan on/off");
      if (fan_->Get() == 0) {
        fan_->On();
      } else {
        fan_->Off();
      }
      return 1000;
    }
  }
  
 
  byte CalculateFanPower(float temp_f) {
    static const int kMaxPower = 255;
    static const int kMinPower = 128;
    
    if (temp_f > 140) {
      return 255;  // Fully on.
    }
    if (temp_f > 120) {
      // fan_power is a range of [0-255]
      int fan_power = map(temp_f, 120, 140,
                          kMinPower, kMaxPower);
      return fan_power;
    } else if ((fan_->Get() > 0) && (temp_f > 90)) {
      // Prevent the fan from shutting on/off when the sensor is around 100.
      // Instead keep the fan on until the temperature drops below 90. The
      // fan will not trigger again until the temperature is much greater than
      // that.
      // If this was not done then the fan would oscillate between on/off.
      return kMinPower; 
    } else {
      return 0;  // off. 
    }
  }
  
 private:
  TempSensor_DHT11* temp_sensor_;
  Led* fan_;
  int prev_fan_power_;
};

TempSensor_DHT11 temp_sensor(7);  // Pin 2.
Led fan(10);

FanController fan_controller(&temp_sensor, &fan);

AbstractCoroutine* routines[] = { &fan_controller };
CoroutineDispatch routine_dispatcher(routines);

void setup() {
  Serial.begin(9600);
  
  // Run test.
  for (int i = 0; i < 2; ++i) {
    fan.On();
    delay(750);
    fan.Off();
    delay(750);
  }
}

void loop() {
  
  int delay_time = routine_dispatcher.Update();
  delay(delay_time);
}

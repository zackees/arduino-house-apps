#include <OneWire.h>  // This has to be included first
#include <TempSensor_DS18S20.h>

TempSensor_DS18S20 temp_sensor(4);

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  Serial.print("Degrees Fahrenheit: ");
  Serial.println(temp_sensor.TempFahrenheit());
}

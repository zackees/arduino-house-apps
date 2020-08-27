
#ifndef _READ_VCC_H_
#define _READ_VCC_H_

#include <Arduino.h>

long readVccMillivolts() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}


float readVcc() {
  long mV = readVccMillivolts();
  double v = double(mV) / 1000.0;
  return v;
}


bool IsConnectedUsbPower() {
  #ifdef __AVR_ATmega32U4__
  
  static bool s_initialized = false;
  if (!s_initialized) {
    USBCON = USBCON | B00010000;
    delay(150);  // Wait at least 150ms or else the UDINT always reads TRUE

    s_initialized = true;
  }
  
  return !(UDINT & B00000001);
  
  #else
    #error IsConnectedUsbPower only implimented for Arduino Simple USB
  #endif
}

class BatteryMonitor {
 public:
  enum PowerState { kFullPower, kFalling, KNearDead, kShutoff };
  
  BatteryMonitor() : _ps(kFullPower), _vcc_readings(16) {
  }
  
  PowerState Read() {
    if (IsConnectedUsbPower()) {
      _ps = kFullPower;
      _vcc_readings.clear();
      return _ps;
    }

    SampleVcc();
    float vcc = AvgVcc();
    
    Serial.print("vcc: "); Serial.println(vcc);
    
    switch (_ps) {
      case kFullPower: if (vcc < 3.25f) { _ps = kFalling; }
      case kFalling:   if (vcc < 3.12f) { _ps = KNearDead; }      
      case KNearDead:  if (vcc < 3.07f) { _ps = kShutoff; }
      case kShutoff: break;
    }  
    return _ps;
  }
  
 private:  
  void SampleVcc() {
    // Always fill up. Happens on first run.
    do {
      _vcc_readings.push_back(readVcc());
    } while (!_vcc_readings.is_full());
  }
  
  
  float AvgVcc() {
    if (_vcc_readings.size() == 0) {
      return 0.0f;
    }
    float avg = 0.0;
    for (size_t i = 0; i < _vcc_readings.size(); ++i) {
      avg += _vcc_readings[i];
    }
    
    return avg / float(_vcc_readings.size());
  }
  
  CircularArray<float> _vcc_readings;
  PowerState _ps;
};


#endif   // _READ_VCC_H_



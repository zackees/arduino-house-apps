#include "Adafruit_NeoPixel.h"
#include "NewDeleteExt.h"
#include "Vector.h"
#include "CircularArray.h"
#include "TwoWireButton.h"
#include "enum.h"
#include "ClassifyPower.h"


/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
const int led = 2;
const int analogInPin = A0;
TwoWireButton button(8);  // pin 8

const int stress_test_cooldown_ms = 4*75;
const int stress_pulse_duration_ms = 4*25;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1,  // num lights
                                            7,  // Pin
                                            NEO_GRB + NEO_KHZ800);

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  // initialize serial communications at 9600 bps:
  Serial.begin(57600); 
  
  strip.begin();
  strip.show();
}

float ReadVoltage() {
  float voltage = 5.0f * analogRead(analogInPin) / 1023.0f;   
  return voltage; 
}


void BatteryStressTest(float cutoff_voltage) {
  const int kMsgDelay_us = 100;
  Serial.println("BatteryStressTest");
  while (true) {
    Serial.println("BatteryStressTest-cycle");
    delay(stress_test_cooldown_ms);
    
        // DEBUG
    unsigned long start_time = millis();
    
    unsigned long start = millis();
    while ((millis() - start) < 1) {
      Serial.print("ST:"); Serial.println(ReadVoltage());
      delayMicroseconds(kMsgDelay_us);
    }
    
    float voltage = ReadVoltage();
    
    Serial.print("ST:");
    Serial.println(voltage);
    if (voltage < cutoff_voltage) {
      return;
    }
    
    
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    
    start = millis();
    while ((millis() - start) < stress_pulse_duration_ms) {
      delayMicroseconds(kMsgDelay_us);
      Serial.print("ST:"); Serial.println(ReadVoltage()); 
    }
  
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    start = millis();
    
    while ((millis() - start) < 100) {
      Serial.print("ST:"); Serial.println(ReadVoltage());
      delayMicroseconds(kMsgDelay_us);
    }
    
    delay(1000);

    
    Serial.print("ST:");
    Serial.println(ReadVoltage());
  }
  
  Serial.println("BatteryStressTest-finished");
}

void BatteryDischarge(float target_voltage) {

  if (target_voltage > ReadVoltage()) { return; }
  
  double average_voltage = 0.0;
  unsigned long countdown_start = 0;
  long finishing_discharge_time = 60 * 60 * 1000; // 1 hr discharge time.
  
  while (true) {

    
    double voltage_accumulator = 0.0;
    unsigned long start = millis();
    unsigned long count = 0;
    const unsigned int discharge_time = 250;

    
    while (true) {
      float voltage = ReadVoltage();
      voltage_accumulator += voltage;
      int led_state = (voltage > target_voltage) ?
                      HIGH : LOW;
      digitalWrite(led, led_state);
      
      const unsigned long same_time_ms = 1;
      delay(same_time_ms);
      
      Serial.print("BatteryDischarge:");
      Serial.println(voltage);
    }
    average_voltage = voltage_accumulator / double(count);
    
    digitalWrite(led, LOW);
    
    delay(1000);  // 100ms on, 1 second off.

    

    
    if (average_voltage > target_voltage + .01) {
      continue;
    }
    
    // If the average_voltage has dropped below the target_voltage then
    // count down toward zero.
    if (average_voltage < target_voltage) {
      
      finishing_discharge_time -= discharge_time;
      bool finished_discharging = finishing_discharge_time < 0;
      
      if (finished_discharging) {
        return;
      }
    }
  }
}

void HighResBatteryProfile() {
  int s[100];
  int sz = 0;
  int delay_us = 1000;
  
  s[sz++] = analogRead(analogInPin);
  digitalWrite(led, HIGH);
  unsigned long start_time = millis();
  unsigned long stop_time = millis() + 20;
  while (millis() < stop_time) {
    if (sz < 20) {
      s[sz++] = analogRead(analogInPin);
      delayMicroseconds(delay_us);
    }
  }
  
  while(millis() < start_time + 100) {;}
  

  
  digitalWrite(led, LOW);
  
  int n = sizeof(s) / sizeof(*s);
  while (sz < n) {
    s[sz++] = analogRead(analogInPin);
    delayMicroseconds(delay_us);
  }
  
  for (int i = 0; i < n; ++i) {
    float v = 5.0f * s[i] / 1023.f;
    Serial.print("T:"); Serial.println(v);
  }
}


PowerType ClassifyPower_DepletionCurve(float open_circuit_v) {
    // Turn the light panel on for 3ms.
    digitalWrite(led, HIGH);
    delay(1);
    
    float load_v = ReadVoltage();
    digitalWrite(led, LOW);
    
    float v_diff = open_circuit_v - load_v;
    
    // Tolerance depends on voltage. CR2032's
    // have larger voltage differences when they
    // are fresh. As LR2032 drop below 3.2 volts
    // their depletion curve flattens so the
    // tolerance can tighten.
    float v_diff_tolerance =
        (open_circuit_v > 3.2) ? .6f : .4f;
    
    if (v_diff < v_diff_tolerance) {
      return kLR2032; 
    } else {
      return kCR2032; 
    }
}

PowerType ClassifyPower_RecoveryCurve() {
    // Use recovery curve. 
    // Turn the light panel on for 3ms.
    digitalWrite(led, HIGH);
    delay(100);
    
    float depleted_v = ReadVoltage();
    digitalWrite(led, LOW);
    delay(2);
    float recovery_v = ReadVoltage();
    
    if (recovery_v - depleted_v < .2f) {
      return kLR2032; 
    } else {
      return kCR2032;
    }
}

// Returns true if battery is rechargeable LIR2032, otherwise false for CR2032
PowerType ClassifyPower() {
  digitalWrite(led, LOW);
  delay(600);
  
  float open_circuit_v = ReadVoltage();
  
  if (open_circuit_v >= 3.45f) {
    return kLR2032;
  }
  
  // Use discharge curve
  if (open_circuit_v > 3.1) {
    return ClassifyPower_DepletionCurve(open_circuit_v);
  } else {
    return ClassifyPower_RecoveryCurve();
  }
}


void BatteryTest() {
  strip.setPixelColor(0, 0, 0, 0);
  strip.show();
  delay(100);
  switch (ClassifyPower()) {
    case kLR2032: {
      // LR2032
      strip.setPixelColor(0, 255, 128, 0);
      Serial.println("LR2032");
      break; 
    }
    case kCR2032: {
      // CR2032
      Serial.println("CR2032");
      strip.setPixelColor(0, 0, 0, 255);
      break;
    }
  }
  strip.show();
  delay(1000);
}



// Repeatedly tests that the CR2032 does not classify as LR2032
void CR2032_TestLoop() {
  while (true) {
    float v = ReadVoltage();
    if (v < 2.5f) {
      strip.setPixelColor(0, 0, 255, 0);
      strip.show();
      while (true) { if (button.Value()) return; }
    }
    
    Serial.print("V:"); Serial.println(v);
    PowerType pt = ClassifyPower();
    
    if (kCR2032 != pt) {
      strip.setPixelColor(0, 255, 0, 0);
      strip.show();
      while (true) {
        Serial.print("Failed at v = "); Serial.println(v);
        HighResBatteryProfile();
        if (button.Value()) return;
        delay(2000);
      }
    } else {
      strip.setPixelColor(0, 0, 0, 255);
      strip.show();
    }
    
    // Turn the LED on full and drain the battery for 100ms.
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(600);  // Recovery time.
  }
}

// Repeatedly tests that the LR2032 does not classify as CR2032
void LR2032_TestLoop() {
  while (true) {
    float v = ReadVoltage();
    if (v < 2.8f) {
      strip.setPixelColor(0, 0, 255, 0);
      strip.show();
      while (true) { if (button.Value()) return; }
    }
    
    Serial.print("V:"); Serial.println(v);
    PowerType pt = ClassifyPower();
    
    if (kLR2032 != pt) {
      strip.setPixelColor(0, 255, 0, 0);
      strip.show();
      while (true) {
        Serial.print("Failed at v = "); Serial.print(v);
        if (button.Value()) return;
        delay(250);
      }
    } else {
      strip.setPixelColor(0, 255, 127, 0);
      strip.show();
    }

    // Turn the LED on full and drain the battery for 100ms.
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(600);  // Recovery time.
  }
}


void AlgorithVerificationTest() {
  PowerType pt = ClassifyPower();
  
  switch (pt) {
    case kCR2032: { CR2032_TestLoop(); return; }
    case kLR2032: { LR2032_TestLoop(); return; }
  }
}

// the loop routine runs over and over again forever:
void loop() {  
  if (!button.Value()) {
    Serial.println("Ready");
    delay(250);
    return; 
  }
  
  //BatteryDischarge(3.3f);  // Discharged down to 3.6v
  //BatteryStressTest(2.5);  // Stress test until input voltage is reached.
  //BatteryTest();
  //LR2032_TestLoop();
  //CR2032_TestLoop();
  AlgorithVerificationTest();
  //HighResBatteryProfile();
  
 //BatteryDischargeCurrent();
  
  Serial.println("Battery Test Finished");
}

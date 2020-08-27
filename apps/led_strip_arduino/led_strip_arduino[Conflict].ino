/*
 Fade
 
 This example shows how to fade an led0 on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 */

/*
#include <avr/interrupt.h>
#include <avr/io.h>

const int LEDPIN = 13;

ISR(TIMER1_OVF_vect) {
  digitalWrite(LEDPIN, !digitalRead(LEDPIN));
}

bool something_is_close = false;

void InterruptTimerSetup() {
  pinMode(LEDPIN, OUTPUT);
 
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
 
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();
}
*/

#include "RangeFinderSharpIR.h"

const int led0 = 11;           // the pin that the led0 is attached to
const int led1 = 9;
const int led2 = 10;

const int light_sensor_pin = A0;
const int range_finder_pin = 3;  

const int delay_speed = 15;

const int startup_cycles = 10;
const int triggered_cycles = 10;

RangeFinderSharpIR ir_range_finder(A5);

int Clamp(int val, int min_val, int max_val) {
  if (val < min_val) {
    return min_val;
  }
  if (val > max_val) {
    return max_val;
  }
  return val;
}

int MapLedBrightness(int val) {
  float f = static_cast<float>(val) / 255.0f;
  // f = f * f * f * f;  // f^4, seems to give a better fade in.
  // f = sin(f * 3.124 / 2.0);
  //f = sqrt(f);
  int rtn = static_cast<int>(f * 255.0);
  return rtn;
}

void StepLed(int amount, int led, int* led_val) {
  *led_val = Clamp(*led_val + amount, 0, 255);
  
  int led_final_val = MapLedBrightness(*led_val);

  analogWrite(led, led_final_val);
}

void Increment(int led, int* led_val) {
  StepLed(1, led, led_val);
}

void Decrement(int led, int* led_val) {
  StepLed(-1, led, led_val);
}

void ledOff(int led) {
  analogWrite(led, 0);
}



void CycleLights(int count) {
  int led0_val = 0;
  int led1_val = 0;
  int led2_val = 0;
  
  ledOff(led0);
  ledOff(led1);
  ledOff(led2);
  
  
  for (int outer_i = 0; outer_i < count; ++outer_i) {
    for (int i = 0; i < 128; i++) {
      Increment(led0, &led0_val);
      Decrement(led1, &led1_val);
      Decrement(led2, &led2_val);
      delay(delay_speed);
    }
    
    for (int i = 0; i < 128; i++) {
      Increment(led0, &led0_val);
      Increment(led1, &led1_val);
      Decrement(led2, &led2_val);
      delay(delay_speed);
    }
    
    for (int i = 0; i < 128; i++) {
      Decrement(led0, &led0_val);
      Increment(led1, &led1_val);
      Increment(led2, &led2_val);
      delay(delay_speed);
    }
    
    for (int i = 0; i < 128; i++) {
      Decrement(led0, &led0_val);
      Decrement(led1, &led1_val);
      Increment(led2, &led2_val);
      delay(delay_speed);
    }
    
    for (int i = 0; i < 128; i++) {
      Increment(led0, &led0_val);
      Decrement(led1, &led1_val);
      Decrement(led2, &led2_val);
      delay(delay_speed);
    }
    
    for (int i = 0; i < 128; i++) {
      Increment(led0, &led0_val);
      Decrement(led1, &led1_val);
      Decrement(led2, &led2_val);
      delay(delay_speed);
    }
  }
  
  while (true) {
    if ((led0_val <= 0) && (led1_val <= 0) && (led2_val <= 0)) {
      break;
    }
    Decrement(led0, &led0_val);
    Decrement(led1, &led1_val);
    Decrement(led2, &led2_val);
    delay(delay_speed * 2);
  }
}

float SearchRangeInches(int pin) {
  int pulse = pulseIn(pin, HIGH);
  //147uS per inch
  float inches = static_cast<float>(pulse)/147.0f;
  //change inches to centimetres
  return inches;
}

// the setup routine runs once when you press reset:
void setup()  { 
  // InterruptTimerSetup();
  // declare pin 9 to be an output:
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(light_sensor_pin, INPUT);
  // LED indicater from timer interrupt.
  pinMode(13, OUTPUT);
  
  // Allows debugging.
  Serial.begin(9600);
  
  ledOff(led0);
  ledOff(led1);
  ledOff(led2);
  
  delay(500);
  CycleLights(startup_cycles);
}


// the loop routine runs over and over again forever:
void loop()  {

  ledOff(led0);
  ledOff(led1);
  ledOff(led2);
  
  delay(500);
  
  // First test, see if the light_sensor_pin is on.
  int sensor_value = analogRead(light_sensor_pin);
  Serial.print("Light Sensor: ");
  Serial.println(sensor_value);
  if (sensor_value > 200) {
    // Don't cycle lights.
    return;
  }
  
  float sensed_inches_bottom = SearchRangeInches(range_finder_pin);
  float sensed_meters_top = ir_range_finder.SampleDistanceMeters();
  
  Serial.print("sensed_inches_bottom: ");
  Serial.println(sensed_inches_bottom);
  
  Serial.print("sensed_meters_top: ");
  Serial.println(sensed_meters_top);
  
  const bool something_there =
    sensed_inches_bottom < 24.0f /*||
    sensed_meters_top < 3.0f*/;
    
  
  if (!something_there) {
   // Don't cycle lights.
   return; 
  }
  
  // Cycle the lights ten times.
  CycleLights(triggered_cycles);
}


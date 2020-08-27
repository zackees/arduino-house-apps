#ifndef _TWO_WIRE_BUTTON_
#define _TWO_WIRE_BUTTON_

#include "Arduino.h"

// TwoWireButton is a momentary button which uses Arduino's built in pullup resistor
// to simplify wiring. One end of the terminal is connected to ground and then the other
// terminal is connected to a digital input pin on the Arduino. When the button is
// pressed the input pin is held low.
class TwoWireButton {
 public:
  explicit TwoWireButton(int sensor_pin)
    : sensor_pin_(sensor_pin), curr_val_(0), invert_(false), initialized_(false) {
  }
  
  int Value() {
    Update();
    UpdateDebounce();
    if (invert_) {
      return curr_val_;
    } else {
      return !curr_val_;
    }
  }
  
  void Update() {
    InitializeOnce();
    UpdateDebounce();
  }
  
  void Invert(bool yes_no) { invert_ = yes_no; }
  
 private:

  
  void UpdateDebounce() {
    int val = digitalRead(sensor_pin_);
    if (val != curr_val_) {  // Has the toggle switch changed?
      curr_val_ = val;       // Set the new toggle switch value.
      
      unsigned long time_now = millis();
      // Protect against debouncing artifacts by putting a 200ms delay from the
      // last time the value changed.
      if ((time_now - debounce_timestamp_) > 150) {
        ++curr_val_;     // ... and increment the value.
        debounce_timestamp_ = time_now;
      }
    }
  }
  
  void InitializeOnce() {
    if (initialized_) {
      return;
    }
    initialized_ = true;
    pinMode(sensor_pin_, INPUT_PULLUP);
    debounce_timestamp_ = millis();
    delay(1);
    curr_val_ = digitalRead(sensor_pin_);
  }
 
  bool initialized_;
  bool invert_;
  int curr_val_;
  int sensor_pin_;
  unsigned long debounce_timestamp_;
};

class CountingToggleTwoWireButton {
 public:
  CountingToggleTwoWireButton(int pin,
                              int toggle_cooldown_ms = 600)
    : b_(pin), toggle_cooldown_ms_(toggle_cooldown_ms),
      last_toggle_time_(0),
      counter_(0),
      last_button_state_(0) {
  }
    
  int Value()   {
    Update();
    // The counter is incremented for up and down action.
    // A complete button toggle will there always be an
    // even number. Shift down 1-bit to ignore the least
    // significant bit.
    return counter_ >> 1;
  }
  void Update() {
    const unsigned long now = millis();
    const bool in_cooldown =
        (now < (last_toggle_time_ + toggle_cooldown_ms_));
    
    // If in cooldown then the button is not to be modified.
    if (in_cooldown) {
      return;
    }
      
    int curr_button_state = b_.Value();
      
    if (last_button_state_ != curr_button_state) {
      counter_++;
      last_button_state_ = curr_button_state;
    }
  }
    
 private:
  TwoWireButton b_;
  unsigned long toggle_cooldown_ms_;
  unsigned long last_toggle_time_;
  int last_button_state_;
  int counter_;
  
};


#endif  // _TWO_WIRE_BUTTON_

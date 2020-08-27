#include "Arduino.h"

enum Button {
  kButtonA = 1 << 0,
  kButtonB = 1 << 1,
  kButtonC = 1 << 2,
  kButtonD = 1 << 3
};

// Call to update internal button state. After this call the state
// of ButtonPressed() and ButtonChanged() maybe changed.
void Update_ButtonState();
bool ButtonPressed(Button b);
bool ButtonChanged(Button b);
void PrintButtonState();

//// IMPL


// uint8_t represents the OR'd state of the button
uint8_t Input_ReadButtonState();
void Input_PrintRawState();
void Input_PrintButton(uint8_t in);

uint8_t s_prevButtonState = 0;
uint8_t s_currButtonState = 0;

void Update_ButtonState() {
  s_prevButtonState = s_currButtonState;
  s_currButtonState = Input_ReadButtonState();
}

bool ButtonPressed(Button b) {
  return (s_currButtonState & b) == b;
}

bool ButtonChanged(Button b) {
  return ((s_currButtonState ^ s_prevButtonState) & b) == b;
}

void PrintButtonState() {
  if (ButtonChanged(kButtonA)) {
    SP("A "); SPLN(ButtonPressed(kButtonA) ? "pressed" : "released");
  }

  if (ButtonChanged(kButtonB)) {
    SP("B "); SPLN(ButtonPressed(kButtonB) ? "pressed" : "released");
  }

  if (ButtonChanged(kButtonC)) {
    SP("C "); SPLN(ButtonPressed(kButtonC) ? "pressed" : "released");
  }

  if (ButtonChanged(kButtonD)) {
    SP("D "); SPLN(ButtonPressed(kButtonD) ? "pressed" : "released");
  }
}

bool IsAnalogPin(int pin) {
  switch (pin) {
    case A0:  return true;
    case A1:  return true;
    case A2:  return true;
    case A3:  return true;
    case A4:  return true;
    case A5:  return true;
    case A6:  return true;
    case A7:  return true;
    case A8:  return true;
    case A9:  return true;
    case A10: return true;
    case A11: return true;
    case A12: return true;
    case A13: return true;
    case A14: return true;
  }
  return false;
}


//// --- IMPL --- ////
const int inputPins[4] = {A14, 0, 1, 17};  // TEENSY 3.1 pins which don't interfere with OctoWS2811


uint8_t Input_ReadButtonState() {
  for (int i = 0; i < ARRAYSIZE(inputPins); ++i) {
    pinMode(inputPins[i], INPUT);
  }

  uint8_t states = 0;
  
  for (int i = 0; i < 4; ++i) {
    uint8_t bit = 0;
    int pin = inputPins[i];
    if (IsAnalogPin(pin)) {
      bit = (analogRead(pin) > 512) ? 1 : 0;
    } else {
      bit = (digitalRead(pin) == HIGH) ? 1 : 0;
    }
    states |= bit << i;
  }

  return states;
}

void Input_PrintButton(uint8_t butState) {
  if (butState == 0)  { SPLN("kNoButtons");    return; }
  if (butState & kButtonA) { SPLN("kButtonA"); }
  if (butState & kButtonB) { SPLN("kButtonB"); }
  if (butState & kButtonC) { SPLN("kButtonC"); }
  if (butState & kButtonD) { SPLN("kButtonD"); }
}

void Input_PrintRawState() {
  uint8_t buttonState = Input_ReadButtonState();
  for (int i = 0; i < 4; ++i) {
    Serial.print("B"); Serial.print(i); Serial.print(": "); Serial.print((buttonState >> i) & 0x1); Serial.print("\t");
  }
  Serial.println();
}



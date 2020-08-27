#define ARRAYSIZE(X) (sizeof(X)/sizeof(*X))
#define SPLN(X) Serial.println(X)
#define SP(X) Serial.print(X)

#include "Input.h"

void setup() {
  Serial.begin(9600);
}

void loop() {

  Update_ButtonState();
  PrintButtonState();
/*
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
  */

  /*
  if (false) {
    uint8_t bState = Input_ReadButtonState();
    Input_PrintButton(bState);
  } else {
    Input_PrintRawState();
  }
  */
}

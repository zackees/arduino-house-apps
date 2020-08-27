#include "TwoWireButton.h"

void setup() {
  Serial.begin(9600);
}

TwoWireButton button(7);
  
  
void loop() {
  delay(50);
  int on = (HIGH == button.Value());
  if (on) {
    Serial.println("HIGH");
  } else {
    Serial.println("LOW");
  }
}

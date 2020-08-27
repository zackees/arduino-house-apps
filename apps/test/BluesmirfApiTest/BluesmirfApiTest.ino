
// Serial1 is supported on Teensy.
#define BT_SERIAL Serial1
#include "BluesmirfApi.h"

void setup() {
  // put your setup code here, to run once:

  BT.begin();

}

void loop() {
  // put your main code here, to run repeatedly:


    BT.serialConfigMode();
  while (1) {}
}

/*****************************************************************************
 * LedCurtainTest
 *
 * Copyright 2014 Zachary Vorhies (Zackees)
 * Pattern is a red/blue/green point lights in motion.
 ****************************************************************************/


#include <SPI.h>

void setup() {
  Serial.begin(9600);
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(4);  // Glitches out when set to 2.
  SendEmptyFrame();
}

// Resets and synchronzies the chipset state.
// After this we can send a color.
void SendEmptyFrame() {
  SPI.transfer(0x0);
  SPI.transfer(0x0);
}

// High order bit must be 1, then 5 bits of red, green, blue.
void SendColor(byte r, byte g, byte b) {
  if (r > 31) { Serial.println("r must be < 32"); }
  if (g > 31) { Serial.println("g must be < 32"); }
  if (b > 31) { Serial.println("b must be < 32"); }
  r = constrain(r, 0, 31);
  g = constrain(g, 0, 31);
  b = constrain(b, 0, 31);
  
  static const short kCtrlBit = 1<<15;
  
  short data = kCtrlBit;  // Specifies that this is a color value.
  data |= r<<10;
  data |= g<<5;
  data |= b;
  
  SPI.transfer(data >> 8);      // Send off high order bits first.
  SPI.transfer(data & 0x00FF);  // Finishes color payload.
}


void PaintColorDot(byte r, byte g, byte b) {
  for (int i = 0; i < 22 * 22 * 4; ++i) {
    for (int j = 0; j < 22 * 22 * 4; ++j) {
      if ((i % 32) == (j % 32)) {
        SendColor(r, g, b);
      } else {
        SendColor(0, 0, 0);
      }
    }
    SendEmptyFrame();
  }
}

void loop() {
  PaintColorDot(31, 0, 0);
  PaintColorDot(0, 31, 0);
  PaintColorDot(0, 0, 31);
}

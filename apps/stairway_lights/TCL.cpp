/*****************************************************************************
 * Tcl.cpp
 *
 * Copyright 2011-2012 Christpher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt
 ****************************************************************************/

#include "TCL.h"
#include <SPI.h>

TclClass TCL;

bool TclClass::use_new_led_chipset(false);

enum {
  kLightClockDivisor = 12,
  kNewLightClockDivisor = 16,
};

#ifdef TCL_DIO
uint8_t TclClass::datapinmask, TclClass::clkpinmask;
volatile uint8_t *TclClass::dataport, *TclClass::clkport;
#endif

void TclClass::setNewLedChipset(bool val) {
  use_new_led_chipset = val;
}


void TclClass::begin() {
#ifdef TCL_SPI
  // Clock diviser is different if using new led chipset.
  int clock_divisor = use_new_led_chipset ? kNewLightClockDivisor : kLightClockDivisor;

  // Set the SPI parameters
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(clock_divisor);
#endif
#ifdef TCL_DIO
  pinMode(TCL_CLOCKPIN, OUTPUT);
  pinMode(TCL_DATAPIN, OUTPUT);
  clkport     = portOutputRegister(digitalPinToPort(TCL_CLOCKPIN));
  clkpinmask  = digitalPinToBitMask(TCL_CLOCKPIN);
  dataport    = portOutputRegister(digitalPinToPort(TCL_DATAPIN));
  datapinmask = digitalPinToBitMask(TCL_DATAPIN);
  *clkport   &= ~clkpinmask;
  *dataport  &= ~datapinmask;
#endif
}

void TclClass::setupDeveloperShield() {
  pinMode(TCL_MOMENTARY1, INPUT);
  pinMode(TCL_MOMENTARY2, INPUT);
  pinMode(TCL_SWITCH1, INPUT);
  pinMode(TCL_SWITCH2, INPUT);

  digitalWrite(TCL_MOMENTARY1, HIGH);
  digitalWrite(TCL_MOMENTARY2, HIGH);
  digitalWrite(TCL_SWITCH1, HIGH);
  digitalWrite(TCL_SWITCH2, HIGH);
}

void TclClass::end() {
#ifdef TCL_SPI
  SPI.end();
#endif
}

byte TclClass::makeFlag(byte red, byte green, byte blue) {
  byte flag = 0;

  flag = (red&0xC0)>>6;
  flag |= ((green&0xC0)>>4);
  flag |= ((blue&0xC0)>>2);
  return ~flag;
}

#ifdef TCL_DIO
void TclClass::dioWrite(byte c) {
  for(byte bit = 0x80; bit; bit >>= 1) {
    if(c & bit) {
      *dataport |=  datapinmask;
    } else {
      *dataport &= ~datapinmask;
    }
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
  }

}
#endif

void TclClass::sendFrame(byte flag, byte red, byte green, byte blue) {
#ifdef TCL_SPI
  SPI.transfer(flag);
  SPI.transfer(blue);
  SPI.transfer(green);
  SPI.transfer(red);
#endif
#ifdef TCL_DIO
  dioWrite(flag);
  dioWrite(blue);
  dioWrite(green);
  dioWrite(red);
#endif
}

void TclClass::sendColor(byte red, byte green, byte blue) {
  if (!use_new_led_chipset) {
    byte flag;
    flag = makeFlag(red,green,blue);
    sendFrame(flag,red,green,blue);
  } else {
    red >>= 3;
    green >>= 3;
    blue >>= 3;
    
    static const short kCtrlBit = 1<<15;
    short data = kCtrlBit;  // Specifies that this is a color value.
    data |= red<<10;
    data |= green<<5;
    data |= blue;
    
    SPI.transfer(data >> 8);      // Send off high order bits first.
    SPI.transfer(data & 0x00FF);  // Finishes color payload
  }
}

void TclClass::sendEmptyFrame() {
  if (!use_new_led_chipset) {
    sendFrame(0x00,0x00,0x00,0x00);
  } else {
    SPI.transfer(0);      // Send off high order bits first.
    SPI.transfer(0);  // Finishes color payload
  }
}

void TclClass::setAll(int num_leds, byte red, byte green, byte blue) {
  sendEmptyFrame();
  for (int i=0; i<num_leds; i++) {
    sendColor(red, green, blue);
  }
  sendEmptyFrame();
}


#include "XBee.h"
#include "Led.h"
#include "SoftwareSerial.h"

Led led(13);
Led error_led(9);
SoftwareSerial xbee_serial(2, 4);

XBee xbee;
XBeeResponse response;
// create reusable response objects for responses we expect to handle 
Rx64Response rx64;

typedef char CharArray64[64];
CharArray64 char_array_64;


void FlashLed(Led* led, int loop_count, int loop_wait) {
  led->Off();
  for (int i = 0; i < loop_count; ++i) {
    led->On();
    delay(loop_wait / 2);
    led->Off();
    delay(loop_wait / 2);
  }
}

void setup() {
  Serial.begin(9600);
  xbee_serial.begin(9600);
  xbee.setSerial(xbee_serial);
  
  for (int i = 0; i < 10; ++i) {
    led.On();
    error_led.On();
    delay(100);
    led.Off();
    error_led.Off();
    delay(100);
  }
}

void loop() {
  led.Off();
  xbee.readPacket(1000);
    
  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
      // got a rx packet
      
      FlashLed(&led, 3, 250);
        
      xbee.getResponse().getRx64Response(rx64);
        
      /*
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
              xbee.getResponse().getRx16Response(rx16);
      	option = rx16.getOption();
      	data = rx16.getData(0);
      } else {
              xbee.getResponse().getRx64Response(rx64);
      	option = rx64.getOption();
      	data = rx64.getData(0);
      }
      
      // TODO check option, rssi bytes    
      flashLed(statusLed, 1, 10);
      
      // set dataLed PWM to value of the first byte in the data
      analogWrite(dataLed, data);
      */
    } else {
    	// not something we were expecting
      //flashLed(errorLed, 1, 25);
      
      FlashLed(&error_led, 2, 100);
      
    }
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
    // or flash error led
    
    FlashLed(&error_led, 4, 250);
    Serial.print("Error reading packet.  Error code: ");
    const char* err_msg = "";
    switch (xbee.getResponse().getErrorCode()) {
      case 1: err_msg = "CHECKSUM_FAILURE"; break;
      case 2: err_msg = "PACKET_EXCEEDS_BYTE_ARRAY_LENGTH"; break;
      case 3: err_msg = "UNEXPECTED_START_BYTE"; break;
      default: err_msg = "UNEXPECTED_ERROR"; break;
    }
    Serial.println(err_msg);
  } else {
    FlashLed(&error_led, 3, 250);
    Serial.println("Cannot get xbee response packet");
  }
  
  delay(500);
}


#include "XBee.h"
#include "Led.h"

#include "XbeeBroadcastMessage.h"

Led error_led(12);
Led success_led(11);

XbeeBroadcastMessage xbee_broadcast(Serial);

void FlashLed(Led* led, int times, int wait) {
  for (int i = 0; i < times; i++) {
    led->On();
    delay(wait / 2);
    led->Off();
    delay(wait / 2);
  }
  delay(500);
}

void setup() {
  Serial.begin(57600);
  
  FlashLed(&error_led, 10, 50);
  FlashLed(&success_led, 10, 50);
}

// continuously reads packets, looking for RX16 or RX64
void loop() {
  char input_msg[32];
  bool recieved = xbee_broadcast.ReceiveStringCmp("Hello World!");
  
  if (!recieved) {
    FlashLed(&success_led, 1, 250);
    delay(100);
    return; 
  }
}

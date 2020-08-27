 
#include "XBee.h"
#include "Led.h"
#include "SoftwareSerial.h"
#include "XbeeBroadcastMessage.h"
#include "RangeFinderSharpIR.h"

XbeeBroadcastMessage xbee_broadcast(Serial);
RangeFinderSharpIR range_sensor(A0);

Led error_led(12);
Led success_led(11);

void FlashLed(Led* led, int times, int wait) {
  for (int i = 0; i < times; i++) {
    led->On();
    delay(wait / 2);
    led->Off();
    delay(wait / 2);
  }
  delay(500);
}

bool Notify_LocSensStairsOn(XbeeBroadcastMessage* broadcaster,
                            Led* success_led,
                            Led* error_led) {
  for (int i = 0; i < 5; ++i) {
    const int err = broadcaster->SendString("LocSensStairsOn");
    Led* led = (err == 0) ? success_led : error_led;
    if (led) {
      FlashLed(led, 1, 250);
    }
    delay(700);
  }
}

void setup() {
  Serial.begin(57600);
  
  FlashLed(&error_led, 10, 50);
  FlashLed(&success_led, 10, 50);
}

void loop() {
  float inches = range_sensor.SampleDistanceInches();
  
  if (inches < 40) {
    Notify_LocSensStairsOn(&xbee_broadcast, &success_led, &error_led);
  } else {
    delay(8);
  }
}

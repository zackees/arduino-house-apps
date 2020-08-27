 
#include "XBee.h"
#include "Led.h"

XBee xbee;

// allocate two bytes for to hold a 10-bit analog reading
uint8_t payload[8] = { 0 };

// with Series 1 you can use either 16-bit or 64-bit addressing

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0xFFFF, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();

int pin5 = 0;

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

void setup() {
  Serial.begin(9600);
  xbee.setSerial(Serial);
  
  FlashLed(&error_led, 10, 50);
  FlashLed(&success_led, 10, 50);
}

void loop() { 
    xbee.send(tx);

    // flash TX indicator
    // FlashLed(&success_led, 1, 100);
    
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
        // got a response!

        // should be a znet tx status             
      if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
          xbee.getResponse().getZBTxStatusResponse(txStatus);
        
         // get the delivery status, the fifth byte
         if (txStatus.getStatus() == SUCCESS) {
           // success.  time to celebrate
           FlashLed(&success_led, 5, 250);
         } else {
           // the remote XBee did not receive our packet. is it powered on?
           FlashLed(&error_led, 3, 250);
           }
        } else {
          FlashLed(&error_led, 2, 500);
        }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
      FlashLed(&error_led, 3, 50);
    } else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      FlashLed(&error_led, 2, 50);
    }
    
    delay(1000);
}

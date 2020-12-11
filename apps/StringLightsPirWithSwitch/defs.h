#ifndef _DEFS_H_
#define _DEFS_H_

//#define DEBUG


#define PIN_STATUS_LED 13
#define PIN_PIR 4
#define PIN_LED_STRIP 6
#define PIN_PWR_SWITCH 8

#define SERIAL_PORT_BAUD 57600

#define FRAME_TIME_MS 8


#ifdef DEBUG
#define ON_DURATION (1000ul * 10ul * 1ul)  // 10 sec timer
#define PRINT_STATE
#else
#define ON_DURATION (1000ul * 60ul * 3ul)  // 3 min timer
#endif

#endif  // _DEFS_H_

#include "Arduino.h"

#ifndef	MEMORY_FREE_H
#define MEMORY_FREE_H

#ifdef __cplusplus
extern "C" {
#endif

int freeMemory();

#ifdef  __cplusplus
}
#endif

#endif

/**************************************************************************/
/*!
@file     BlueSMiRF.h
@author   Thomas Ricci
This is a library for the Sparkfun BlueSMiRF bluetooth breakout board
https://www.sparkfun.com/products/12577
This chip uses Serial to communicate, 2 pins (RX and TX) are required to interface
*/
/**************************************************************************/

#ifndef BlueSMiRF_included
#define BlueSMiRF_included

#define BLUESMiRF_DEBUG

#define DEFAULT_BOUD 115200
#define MAX_DISCOVERABLE 3 //9 MAX for RN-41 Bluetooth module

#define BLUESMiRF_DEFAULT_TIMEOUT_MS 500

enum BlueSMiRF_Status{ CONFIG, NORMAL };

// NOTE that BT is the name of the global version of this class.


class BlueSMiRF
{
private:
	uint16_t _baud;
	BlueSMiRF_Status _status;
	unsigned long _lastInquiry;
	bool _inquiring;
	char _replybuffer[32];
	void startConfigMode();
	void endConfigMode();
	void flushInput();
	void sendCmd(const char * cmd, bool newLineEnding = true);
	uint8_t readline(uint16_t timeout = BLUESMiRF_DEFAULT_TIMEOUT_MS, bool multiline = false);
public:
        BlueSMiRF();
 	char discovered[MAX_DISCOVERABLE][12];
	void begin(uint16_t baud = 115200);
	int8_t discovery(uint8_t scanSeconds = 10);
	bool detect(char * id);
	void linkQuality();
	void disableDiscoverability();
	void enableDiscoverability();
	void help();
	void reboot();
	void setName(char * name);
	void settings();
	void extendedSettings();
	void serialConfigMode();
};

#endif

/**************************************************************************/
/*!
@file     BlueSMiRF.cpp
@author   Thomas Ricci
This is a library for the Sparkfun BlueSMiRF bluetooth breakout board
https://www.sparkfun.com/products/12577
This chip uses Serial to communicate, 2 pins (RX and TX) are required to interface
*/
/**************************************************************************/



const char Exit_DebugMode = '!';
const char * BEGINCONFIG_CMD = "$$$";
const char * ENDCONFIG_CMD = "---";
const char * COMMA = ",";
const char * NL = "\n";
const char * CR = "\r";
const char * CRNL = "\r\n";
const char * INFO_CMD = "D";
const char * EXTINFO_CMD = "E";
const char * INQUIRY_CMD = "I,%d";
const char * INQUIRYN_CMD = "IN,%d";
const char * BOUD_CMD = "U,%d,N";
const char * SETNAME_CMD ="SN,%s";
const char * REBOOT_CMD ="R,1";
const char * HELP_CMD = "H";
const char * LINKQLT_CMD = "L";
const char * QUIET_CMD = "Q";
const char * WAKE_CMD = "W";

BlueSMiRF::BlueSMiRF() {
	_status = NORMAL;
	_inquiring = false;
}

void BlueSMiRF::flushInput(){
	while(BT_SERIAL.available())
		BT_SERIAL.read();
}

void BlueSMiRF::sendCmd(const char* cmd, bool newLineEnding){
	if(newLineEnding)
		BT_SERIAL.println(cmd);
	else
		BT_SERIAL.print(cmd);
		
	delay(100);
	flushInput();
}

void BlueSMiRF::begin(uint16_t baud){
	_baud = baud;
	startConfigMode();
	char cmd[16];
	sprintf(cmd, BOUD_CMD, _baud);
  	BT_SERIAL.println(cmd);
  	BT_SERIAL.begin(_baud);
	endConfigMode();
	_status = NORMAL;	
}

void BlueSMiRF::settings(){
	if (_status == NORMAL)
		startConfigMode();
	
	sendCmd(INFO_CMD);
}

void BlueSMiRF::extendedSettings(){
	if (_status == NORMAL)
		startConfigMode();
	
	sendCmd(EXTINFO_CMD);
}

void BlueSMiRF::setName(char * name){
	startConfigMode();
	char cmd [strlen(name) + 3];
	sprintf(cmd, SETNAME_CMD, name);
	sendCmd(cmd);
	endConfigMode();
}

int8_t BlueSMiRF::discovery(uint8_t scanSeconds){

	unsigned long currentTime = millis();
	// Check if its already time for our process
	if(!_inquiring && ((currentTime - _lastInquiry) > scanSeconds * 1000)) {
		if(_status == NORMAL)
			startConfigMode();
		
		char cmd[8];
		sprintf(cmd, INQUIRYN_CMD, scanSeconds - 4);
		
		flushInput();
		
		// Start the inquiry scan
		BT_SERIAL.println(cmd);
		_inquiring = true;
		_lastInquiry = currentTime;
		
		#ifdef BLUESMiRF_DEBUG
		Serial.println(F("DISCOVERY session started"));
		Serial.print(F("Free Memory: "));
		Serial.println(freeMemory());
		#endif
	}
		
	//delay(polling - 1000);
	else if(_inquiring && (currentTime - _lastInquiry > (scanSeconds - 4) * 1000)){
		
		uint8_t len;
		uint8_t n = 0;
		bool nl = false;
		
		//Reads first line: Inquiry,T=7,COD=0
		readline(); 
		//Reads line containing discovered device: Inquiry,T=7,COD=0
		do{
			len = readline();
			
			if(len <= 0)
				continue;
			
			char * temp;
			char* mac = strtok_r(_replybuffer, COMMA, &temp);
			
			if(mac == NULL || n > MAX_DISCOVERABLE)
				continue;
			
			strcpy(discovered[n], mac);
			
			#ifdef BLUESMiRF_DEBUG
			Serial.print(F("Found: "));
			Serial.println(discovered[n]);
			Serial.print(F("Memory free: "));
			Serial.println(freeMemory());
			#endif
			
			n++;
		} while(len > 0);
		
		_inquiring = false;
		
		return n;
	}
	
	return -1;
}

bool BlueSMiRF::detect(char* id){
	int8_t n  = discovery();

	for(int8_t i=0; i<n; i++)		
		if(strncmp(discovered[i], id, 12) == 0)
			return true;
			
	return false;
}

void BlueSMiRF::startConfigMode(){
	if(_status == CONFIG) 
		return;
	
	sendCmd(BEGINCONFIG_CMD, false);
	_status = CONFIG;
}

void BlueSMiRF::endConfigMode(){
	sendCmd(ENDCONFIG_CMD);
	_status = NORMAL;
}

void BlueSMiRF::reboot(){
	sendCmd(REBOOT_CMD);
	_status = NORMAL;
}

void BlueSMiRF::help(){
	sendCmd(HELP_CMD);
}

void BlueSMiRF::disableDiscoverability(){
	sendCmd(QUIET_CMD);
}

void BlueSMiRF::enableDiscoverability(){
	sendCmd(WAKE_CMD);
}

void BlueSMiRF::linkQuality(){
	sendCmd(LINKQLT_CMD);
}

uint8_t BlueSMiRF::readline(uint16_t timeout, bool multiline) {
  uint16_t replyidx = 0;
  
  while (timeout--) {
    if (replyidx >= 65)
      break;

    while(BT_SERIAL.available()) {
      char c =  BT_SERIAL.read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)  
          continue;
        
        if (!multiline) {
          timeout = 0;        
          break;
        }
      }
      _replybuffer[replyidx] = c;
      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);
      replyidx++;
    }
    
    if (timeout == 0) 
      break;
    
    delay(1);
  }
  _replybuffer[replyidx] = 0;  // null term
  return replyidx;
}

void BlueSMiRF::serialConfigMode(){
	 Serial.println(F("## +++ BLUESMiRF CONFIGURATION MODE ##"));
	 startConfigMode();
	 char inSerial = 0;
	 do{
		 if (BT_SERIAL.available())
		 {
			 Serial.print((char)BT_SERIAL.read());
		 }
		 if (Serial.available())
		 {
			 inSerial = (char)Serial.read();

			 if (inSerial == Exit_DebugMode){
				 endConfigMode();
				 continue;
			 }
			 BT_SERIAL.print(inSerial);
		 }

	 } while (inSerial != Exit_DebugMode);
	 Serial.println(F("## --- BLUESMiRF CONFIGURATION MODE ##"));
}



extern unsigned int __heap_start;
extern void *__brkval;

/*
 * The free list structure as maintained by the
 * avr-libc memory allocation routines.
 */
struct __freelist
{
  size_t sz;
  struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;

/* Calculates the size of the free list */
int freeListSize()
{
  struct __freelist* current;
  int total = 0;
  for (current = __flp; current; current = current->nx)
  {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }

  return total;
}

int freeMemory()
{
  int free_memory;
  if ((int)__brkval == 0)
  {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  }
  else
  {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}

BlueSMiRF BT = BlueSMiRF();

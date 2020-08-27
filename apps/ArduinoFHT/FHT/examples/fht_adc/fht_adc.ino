/*
fht_adc.pde
guest openmusiclabs.com 9.5.12
example sketch for testing the fht library.
it takes in data on ADC0 (Analog0) and processes them
with the fht. the data is sent out over the serial
port at 115.2kb.  there is a pure data patch for
visualizing the data.
*/

#define LOG_OUT 1 // use the log output function
#define FHT_N 16 // set to 256 point fht

#include "FHT.h" // include the library

// #define DISABLE_TIMER0
// #define DISABLE_GLOBAL_INTERUPTS

void setup() {
  //Serial.begin(115200); // use the serial port
  Serial.begin(9600);
  
  #ifdef DISABLE_TIMER0
  TIMSK0 = 0; // turn off timer0 for lower jitter
  #endif
  
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void loop() {
  while(1) { // reduces jitter
  
    #ifdef DISABLE_GLOBAL_INTERUPTS
    cli();  // UDRE interrupt slows this way down on arduino1.0
    #endif
    
    int dbg_val = -1;
    byte save_m;
    byte save_j;
    
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
      
      if (i == 5) {
        dbg_val = k;
        save_m = m;
        save_j = j;
      }
      //Serial.println(k);
    }
    

    
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    
    #ifdef DISABLE_GLOBAL_INTERUPTS
    sei();
    #endif
    
    Serial.write(255); // send a start byte
    Serial.write(fht_log_out, FHT_N/2); // send out the data

    Serial.println(dbg_val);
    Serial.println(save_m);
    Serial.println(save_j);
  }
}

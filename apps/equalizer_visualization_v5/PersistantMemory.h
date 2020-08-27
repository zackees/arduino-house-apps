#ifndef _PERSISTANT_MEMORY_H_
#define _PERSISTANT_MEMORY_H_


#include <Arduino.h>  // for type definitions


template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}



class SavedState {
  public:

   static SavedState& Instance() {
     static SavedState* state = new SavedState();
     return *state; 
   }
   
   SavedState() {
     EEPROM_readAnything(0, s_);
     if (s_.magic_key != kMagicKey) {
       s_.magic_key = kMagicKey;
       s_.pixel_count = 150;
       EEPROM_writeAnything(0, s_);
     }
   }
   
   int PixelCount() {
     return s_.pixel_count; 
   }
   
   void SetPixelCount(int n) {
     if (n > MAX_PIXEL_COUNT) { n = MAX_PIXEL_COUNT; }
     if (s_.pixel_count != n) {
       s_.pixel_count = n;
       EEPROM_writeAnything(0, s_);
     }
   }
   
 private:
  enum { kMagicKey = 122 };
  struct PersistantState {
    int magic_key;
    int pixel_count;
  };
  PersistantState s_;
};


#endif  // _PERSISTANT_MEMORY_H_


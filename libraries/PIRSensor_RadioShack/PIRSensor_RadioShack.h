//
//  PIRSensor_RadioShack.h
//  
//
//  Created by Zachary Vorhies on 2/22/15.
//
//


#include "Arduino.h"

class PIRSensor_RadioShack {
public:
    explicit PIRSensor_RadioShack(int pin) : pir_pin_(pin) {}
    bool MovementDetected() const {
        return HIGH == digitalRead(pir_pin_);
    }
private:
    int pir_pin_;
};

#pragma once

#include <stdint.h>
#include "utils/bit.h"
#include "arduino_hal/arduinoHelpers.h"
#include "shared/dht_protocol_interface.hpp"



/** hardware specific implementation of the dht protocol */
class DHTSensor : public DHT_PROTOCOL_INTERFACE
{
    private:
        uint8_t dataPin;	// change to pin type
        
    public:
        DHTSensor(const uint8_t dataPin)
        {
            this->dataPin = dataPin;
            temperature = -273.15;    // absolute zero
            humidity = -1;
        }

        void init(void) override
        {
            ; 	// implement
        }

    private:
        void performHandshake(void) override
        {
            ;	// implement
        }

        void read40Bits(void) override
        {   
 			;	// implement
        }

};


#pragma once

#include <stdint.h>
#include "../utils/bit.h"
#include "arduinoHelpers.h"
#include "../shared/dht_protocol_interface.hpp"



/** hardware specific implementation of the dht protocol */
class DHTSensor : public DHT_PROTOCOL_INTERFACE
{
    private:
        uint8_t dataPin;
        
    public:
        DHTSensor(const uint8_t dataPin)
        {
            this->dataPin = dataPin;
            temperature = -273.15;    // absolute zero
            humidity = -1;
        }

        void init(void) override
        {
            pinMode(dataPin, INPUT);
            delay(1000);
        }

    private:
        void performHandshake(void) override
        {
            // SEND START SIGNAL: pull the line down to GND for >18ms
            pinMode(dataPin, OUTPUT);
            digitalWrite(dataPin, LOW);
            delay(20);
            digitalWrite(dataPin, HIGH);

            // READ RESPONSE SIGNAL: wait until sensor pulls the line low (80us) and high (80us)
            pinMode(dataPin, INPUT);
            busyWaitUntilPinIsVal(dataPin, LOW);
            busyWaitUntilPinIsVal(dataPin, HIGH);
            // now ready to recieve data.
        }

        void read40Bits(void) override
        {   
            for(uint8_t i=0; i<40; i++){
                unsigned long hiTime = busyWaitUntilPinIsVal(dataPin, HIGH);
                bool bitVal = pulseTime2Bit(hiTime);

                if((i >= 0) && (i <= 7)){           // first byte
                    uint8_t bitPos = 7 - i;
                    BIT_TO_VAL(rawData.humiByte_hi, bitPos, bitVal);
                }
                else if((i >= 8) && (i <= 15)){     // second byte
                    uint8_t bitPos = 15 - i;
                    BIT_TO_VAL(rawData.humiByte_lo, bitPos, bitVal);
                }
                else if((i >= 16) && (i <= 23)){    // thrid byte
                    uint8_t bitPos = 23 - i;
                    BIT_TO_VAL(rawData.tempByte_hi, bitPos, bitVal);
                }
                else if((i >= 24) && (i <= 31)){    // fourth byte
                    uint8_t bitPos = 31 - i;
                    BIT_TO_VAL(rawData.tempByte_lo, bitPos, bitVal);
                }
                else if((i >= 32) && (i <= 39)){    // fifth byte
                    uint8_t bitPos = 39 - i;
                    BIT_TO_VAL(rawData.parity_byte, bitPos, bitVal);
                }
            }
        }

};


#pragma once
#include "bit.h"
#include "DHTdataSet.h"



/** @returns the time waited in microseconds until pin has value val */
static unsigned long busyWaitUntilPinIsVal(const uint8_t pin, const bool val)
{
    unsigned long timeStampStart = micros();
    while(digitalRead(pin) != val){
        ; // wait
    }
    unsigned long timeStampStop = micros();
    unsigned long timePassed = timeStampStop - timeStampStart;
    return timePassed;
}


/** helper for interpreting data pulses
    @returns 0 for pulse of ~27us
    @retruns 1 for pulse of ~70us
*/
static inline bool pulseTime2Bit(const unsigned long pulseTime_us)
{
    return ((pulseTime_us > 40) && (pulseTime_us < 100));
}


/** initialises hardware for the dht protocol */
void dht_protocol_init(const uint8_t dataPin)
{
    pinMode(dataPin, INPUT);
    delay(1000);
}


/** @returns true if protocol handshake was sucessful */
bool dht_protocol_performHandshake(const uint8_t dataPin)
{
    // SEND START SIGNAL: pull the line down to GND for >18ms
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, LOW);
    delay(20);
    digitalWrite(dataPin, HIGH);

    // READ RESPONSE SIGNAL: wait until sensor pulls the line low (80us) and high (80us)
    pinMode(dataPin, INPUT);
    unsigned long loTime = busyWaitUntilPinIsVal(dataPin, LOW);
    unsigned long hiTime = busyWaitUntilPinIsVal(dataPin, HIGH);
    // if impulses ~80us sensor will send data.
    return ((loTime > 8 && loTime < 800) && (hiTime > 8 && hiTime < 800));
}


/** reads 40 bits of data and writes them to the according byte in dataSet. 
    @returns true if data read passed parity check.
*/
bool dht_protocol_readData(const uint8_t dataPin, DHTdataSet* dataSet)
{   
    /*  BYTE:   [ 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 ]

        [ 0 - 7 ],  [ 8 - 15],  [16 - 23],  [24 - 31],  [32 - 39]
        1111 1111,  1111 1111,  1111 1111,  1111 1111,  1111 1111
        humi_hi,    humi_lo,    temp_hi,    temp_lo,    parity
    */
    for(uint8_t i=0; i<40; i++){
        unsigned long hiTime = busyWaitUntilPinIsVal(dataPin, HIGH);
        bool bitVal = pulseTime2Bit(hiTime);

        if((i >= 0) && (i <= 7)){           // first byte
            uint8_t bitPos = 7 - i;
            BIT_TO_VAL(dataSet->humiByte_hi, bitPos, bitVal);
        }
        else if((i >= 8) && (i <= 15)){     // second byte
            uint8_t bitPos = 15 - i;
            BIT_TO_VAL(dataSet->humiByte_lo, bitPos, bitVal);
        }
        else if((i >= 16) && (i <= 23)){    // thrid byte
            uint8_t bitPos = 23 - i;
            BIT_TO_VAL(dataSet->tempByte_hi, bitPos, bitVal);
        }
        else if((i >= 24) && (i <= 31)){    // fourth byte
            uint8_t bitPos = 31 - i;
            BIT_TO_VAL(dataSet->tempByte_lo, bitPos, bitVal);
        }
        else if((i >= 32) && (i <= 39)){    // fifth byte
            uint8_t bitPos = 39 - i;
            BIT_TO_VAL(dataSet->parity_byte, bitPos, bitVal);
        }
    }
    return dhtDataSet_parityCheck(dataSet);
}



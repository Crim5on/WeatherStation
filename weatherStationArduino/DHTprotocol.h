#pragma once
#include "bit.h"
#include "DHTdataSet.h"

#define LOOP_UPPER_BOUND 10000
#define PULSE_TIMEOUT_US 20000


/** @returns waited time in microseconds */
static unsigned long busyWaitWhile(const uint8_t pin, const bool val)
{
    uint32_t loopCount = 0;
    unsigned long timeStampStart = micros();
    while((digitalRead(pin) == val) && (loopCount < LOOP_UPPER_BOUND)){
        ; // wait
        loopCount++;
    }
    unsigned long timeStampStop = micros();
    unsigned long timePassed = timeStampStop - timeStampStart;
    return timePassed;
}

/** helper for interpreting data pulses
    @returns 0 for pulse of ~27us
    @retruns 1 for pulse of ~70us
*/
static inline bool pulse2Bit(const unsigned long pulseTime_us)
{
    return ((pulseTime_us > 50) && (pulseTime_us < 90));
}

/** @returns true if protocol handshake was sucessful */
static bool dht_protocol_performHandshake(const uint8_t dataPin)
{
    // SEND START SIGNAL: 
    pinMode(dataPin, OUTPUT);
    digitalWrite(dataPin, LOW);
    delay(20);  // pull LOW for >18ms
    digitalWrite(dataPin, HIGH);
    delayMicroseconds(10);  // to ensure pin is set high
    pinMode(dataPin, INPUT);

    // READ RESPONSE SIGNAL: wait until sensor pulls the line low (80us) and high (80us)
    unsigned long loTime = busyWaitWhile(dataPin, LOW);
    unsigned long hiTime = busyWaitWhile(dataPin, HIGH);
    // if impulses ~80us sensor will send data.
    return ((loTime < 100) && (hiTime < 100));
}

/** reads 40 bits of data and writes them to the according byte in dataSet. 
    @returns true if data read passed parity check.
*/
static bool dht_protocol_receive40bits(const uint8_t dataPin, DHTdataSet* dataSet)
{   
    /*  BYTE:   [ 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 ]

        [ 0 - 7 ],  [ 8 - 15],  [16 - 23],  [24 - 31],  [32 - 39]
        1111 1111,  1111 1111,  1111 1111,  1111 1111,  1111 1111
        humi_hi,    humi_lo,    temp_hi,    temp_lo,    parity
    */
    for(uint8_t i=0; i<40; i++)
    {
        unsigned long loTime = busyWaitWhile(dataPin, LOW);
        unsigned long hiTime = busyWaitWhile(dataPin, HIGH);
        
        if((loTime > PULSE_TIMEOUT_US) || (hiTime > PULSE_TIMEOUT_US)){
            delay(1000);
            return false;
        }

        if((i >= 0) && (i <= 7)){           // first byte
            BIT_TO_VAL(dataSet->humiByte_hi, (7 - i), pulse2Bit(hiTime));
        }
        else if((i >= 8) && (i <= 15)){     // second byte
            BIT_TO_VAL(dataSet->humiByte_lo, (15 - i), pulse2Bit(hiTime));
        }
        else if((i >= 16) && (i <= 23)){     // thrid byte
            BIT_TO_VAL(dataSet->tempByte_hi, (23 - i), pulse2Bit(hiTime));
        }
        else if((i >= 24) && (i <= 31)){    // fourth byte
            BIT_TO_VAL(dataSet->tempByte_lo, (31 - i), pulse2Bit(hiTime));
        }
        else if((i >= 32) && (i <= 39)){    // fifth byte
            BIT_TO_VAL(dataSet->parity_byte, (39 - i), pulse2Bit(hiTime));
        }
    }
    return dhtDataSet_parityCheck(dataSet);
}


/** initialises hardware for the dht protocol */
void dht_protocol_init(const uint8_t dataPin)
{
    // do not interfere with data line for for 1 second.
    pinMode(dataPin, INPUT);
    delay(1000);
}


/** @returns true if filled valid data into dataSet, false otherwise */
bool dht_protocol_readData(const uint8_t dataPin, DHTdataSet* dataSet)
{
    if(dht_protocol_performHandshake(dataPin)){
        return dht_protocol_receive40bits(dataPin, dataSet);
    }
    return false;
}


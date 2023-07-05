#pragma once
#include "bit.h"

/** DHT single wire bus protocol: 40-bit transfer */
typedef struct DHTdataSet
{       
    uint8_t humiByte_hi;
    uint8_t humiByte_lo;    // always 0 for dht11
    uint8_t tempByte_hi;
    uint8_t tempByte_lo;    // always 0 for dht11
    uint8_t parity_byte;    // lowest bits of sum of all other bytes
} DHTdataSet;

/** @returns true if parity check on dataSet was successful */
inline bool dhtDataSet_parityCheck(const DHTdataSet* dataSet)
{
    uint16_t checkSum16 = dataSet->humiByte_hi + dataSet->humiByte_lo + dataSet->tempByte_hi + dataSet->tempByte_lo;
    uint8_t checkSum8 = (uint8_t)checkSum16;    // cut off bits higher position 7
    return (checkSum8 == dataSet->parity_byte);
}

/** @returns humidity from raw data as double */
inline int8_t dhtDataSet_calcHumidity(const DHTdataSet* dataSet)
{
    if(dataSet->humiByte_lo == 0){
        return (int8_t)dataSet->humiByte_hi;
    }
    else{
        uint16_t humidity_x10 = (dataSet->humiByte_hi << 8 | dataSet->humiByte_lo);
        return (int8_t)(humidity_x10 * 0.1);
    }
}

/** @returns temperature from raw data as double */
inline int8_t dhtDataSet_calcTemperature(const DHTdataSet* dataSet)
{
    uint8_t highByte = dataSet->tempByte_hi;
    uint8_t lowByte = dataSet->tempByte_lo;
    bool isNegativeValue = BIT_IS_SET(highByte, 7);
    BIT_CLR(highByte, 7);

    if((lowByte == 0) && (!isNegativeValue)){
        return (int8_t)highByte;
    }
    else if((lowByte == 0) && (isNegativeValue)){
        return (int8_t)(0 - highByte);
    }
    else if((lowByte != 0) && (!isNegativeValue)){
        uint16_t temperature_x10 = (highByte << 8 | lowByte);
        return (int8_t)(temperature_x10 * 0.1);
    }
    else if((lowByte != 0) && (isNegativeValue)){
        uint16_t temperature_x10 = (highByte << 8 | lowByte);
        return (int8_t)(temperature_x10 * -0.1);
    }
    else{
        return EXIT_FAILURE;    // won't happen
    }
}

#pragma once

/** DHT single wire bus protocoll: 40-bit transfer */
struct DHTdataSet
{       
    // 40 bit raw data:
    uint8_t humiByte_hi;
    uint8_t humiByte_lo;    // always 0 for dht11
    uint8_t tempByte_hi;
    uint8_t tempByte_lo;    // always 0 for dht11
    uint8_t parity_byte;    // lowest bits of sum of all other bytes

	/** performs parity check on raw data */
    bool isValid(void)
    {
        uint16_t checkSum16 = humiByte_hi + humiByte_lo + tempByte_hi + tempByte_lo;
        uint8_t checkSum8 = (uint8_t)checkSum16;    // cut off bits higher position 7
        return (checkSum8 == parity_byte);
    }

	/** @returns humidity from raw data as double */
    double calcHumidity(void)
    {
        if(humiByte_lo == 0){
            return (double)humiByte_hi;
        }
        else{
            uint16_t humidity_x10 = (humiByte_hi << 8 | humiByte_lo);
            return (double)(humidity_x10 * 0.1);
        }
    }

	/** @returns temperature from raw data as double */
    double calcTemperature(void)
    {
        uint8_t highByte = tempByte_hi;
        uint8_t lowByte = tempByte_lo;
        bool isNegativeValue = BIT_IS_SET(highByte, 7);
        BIT_CLR(highByte, 7);

        if((lowByte == 0) && (!isNegativeValue)){
            return (double)highByte;
        }
        else if((lowByte == 0) && (isNegativeValue)){
            return (double)(0 - highByte);
        }
        else if((lowByte != 0) && (!isNegativeValue)){
            uint16_t temperature_x10 = (highByte << 8 | lowByte);
            return (double)(temperature_x10 * 0.1);
        }
        else if((lowByte != 0) && (isNegativeValue)){
            uint16_t temperature_x10 = (highByte << 8 | lowByte);
            return (double)(temperature_x10 * -0.1);
        }
        else{
            return EXIT_FAILURE;    // won't happen
        }
    }
};

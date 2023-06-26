#include "bit.h"



struct DHTdataSet
{
    // DHT11 protocoll: 40-bit transfer
    uint8_t humidity_high;
    uint8_t humidity_low;       // always 0 for dht11
    uint8_t temperature_high;
    uint8_t temperature_low;    // always 0 for dht11
    uint8_t parity_bits;        // sum of all other bytes
};

bool dataIsValid(const DHTdataSet& data)
{
    uint16_t checkSum16 = data.humidity_high + data.humidity_low + data.temperature_high + data.temperature_low;
    uint8_t checkSum8 = (uint8_t)checkSum16;    // cut off bits higher position 7
    return (checkSum8 == data.parity_bits);
}

double getHumidity(DHTdataSet& data)
{
    if(data.humidity_low == 0){
        return (double)data.humidity_high;
    }
    else{
        uint16_t humidity_x10 = (data.humidity_high << 8 | data.humidity_low);
        double humidity = humidity_x10 * 0.1;
        return humidity;
    }
}

double getTemperature(DHTdataSet& data)
{
    uint8_t highByte = data.temperature_high;
    uint8_t lowByte = data.temperature_low;
    bool isNegativeValue = BIT_IS_SET(highByte, 7);
    BIT_CLR(highByte, 7);

    if(lowByte == 0 && !isNegativeValue){
        return (double)highByte;
    }
    else if(lowByte == 0 && isNegativeValue){
        return (double)(0 - highByte);
    }
    else if(lowByte != 0 && !isNegativeValue){
        uint16_t humidity_x10 = (highByte << 8 | lowByte);
        double humidity = humidity_x10 * 0.1;
        return humidity;
    }
    else if(lowByte != 0 && isNegativeValue){
        uint16_t humidity_x10 = (highByte << 8 | lowByte);
        double humidity = humidity_x10 * -0.1;
        return humidity;
    }
    return EXIT_FAILURE;
}





void setup() 
{
    Serial.begin(9600);
    pinMode(2, INPUT_PULLUP);
    pinMode(13, OUTPUT);
}



void loop() 
{
    



    int sensorVal = digitalRead(2);
    Serial.println(sensorVal);
}

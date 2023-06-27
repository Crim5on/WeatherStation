#include "bit.h"
#include "helpers.h"



struct DHTdataSet
{    
    // DHT single wire bus protocoll: 40-bit transfer
    uint8_t humidity_hi;
    uint8_t humidity_lo;       // always 0 for dht11
    uint8_t temperature_hi;
    uint8_t temperature_lo;    // always 0 for dht11
    uint8_t parity_bits;       // sum of all other bytes
};


class DHTSensor
{
    private:
        uint8_t dataLine;
        DHTdataSet dataSet;
        double temperature;
        double humidity;
        const double absolutZero = -273.15;

    public:
        DHTSensor(const uint8_t dataLine)
        {
            this->dataLine = dataLine;
            dataSet.humidity_hi = 0b00000000;
            dataSet.humidity_lo = 0b00000000;
            dataSet.temperature_hi = 0b00000000;
            dataSet.temperature_lo = 0b00000000;
            dataSet.parity_bits = 0b00000000;
            temperature = absolutZero;
            humidity = -1;
        }

        void init(void)
        {
            // 1.) INITIATE: wait 1s until sensor is initated, while data line remains high by pull-up resisitor
            pinMode(dataLine, INPUT_PULLUP);       //TODO: enable internal pull-up or use external pull-up instead.
            delay(1000);
        }

    private:

        bool parityCheckIsValid(void)
        {
            uint16_t checkSum_16 = dataSet.humidity_hi + dataSet.humidity_lo + dataSet.temperature_hi + dataSet.temperature_lo;
            uint8_t checkSum_8 = (uint8_t)checkSum_16;    // cut off bits higher position 7
            return (checkSum_8 == dataSet.parity_bits);
        }

        double calculateHumidity(void)
        {
            if(dataSet.humidity_lo == 0){
                return (double)dataSet.humidity_hi;
            }
            else{
                uint16_t humidity_x10 = (dataSet.humidity_hi << 8 | dataSet.humidity_lo);
                return (double)(humidity_x10 * 0.1);
            }
        }

        double calculateTemperature(void)
        {
            uint8_t highByte = dataSet.temperature_hi;
            uint8_t lowByte = dataSet.temperature_lo;
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
                return absolutZero;
            }
        }

        bool bitFromSignalTime(const unsigned long hiTime)
        {
            // t_hi ~=  27us -> 0 bit
            // t_hi ~=  70us -> 1 bit
            return ((hiTime > 40) && (hiTime < 100));
        }

        void performHandshake(void)
        {
            // SEND START SIGNAL: pull the line down to GND for >18ms
            pinMode(dataLine, OUTPUT);
            digitalWrite(dataLine, LOW);
            delay(20);
            digitalWrite(dataLine, HIGH);

            // READ RESPONSE SIGNAL: wait until sensor pulls the line low (80us) and high (80us)
            pinMode(dataLine, INPUT);
            busyWaitUntilPinIsVal(dataLine, LOW);
            busyWaitUntilPinIsVal(dataLine, HIGH);
            // now ready to recieve data.
        }

        void read40BitsIntoDataSet(void)
        {
            /*  bits in byte:   [ 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 ]

                // bits as they are received:
                [ 0 - 7 ],  [ 8 - 15],  [16 - 23],  [24 - 31],  [32 - 39]
                1111 1111,  1111 1111,  1111 1111,  1111 1111,  1111 1111
                hum_hi,     hum_lo,     temp_hi,    temp_lo,    parity
            */

            for(uint8_t i=0; i<40; i++){
                unsigned long hiTime = busyWaitUntilPinIsVal(dataLine, HIGH);
                bool bitVal = bitFromSignalTime(hiTime);

                if((i >= 0) && (i <= 7)){             // first byte
                    uint8_t bitPos = 7 - i;
                    BIT_TO_VAL(dataSet.humidity_hi, bitPos, bitVal);
                }
                else if((i >= 8) && (i <= 15)){     // second byte
                    uint8_t bitPos = 15 - i;
                    BIT_TO_VAL(dataSet.humidity_lo, bitPos, bitVal);
                }
                else if((i >= 16) && (i <= 23)){    // thrid byte
                    uint8_t bitPos = 23 - i;
                    BIT_TO_VAL(dataSet.temperature_hi, bitPos, bitVal);
                }
                else if((i >= 24) && (i <= 31)){    // fourth byte
                    uint8_t bitPos = 31 - i;
                    BIT_TO_VAL(dataSet.temperature_lo, bitPos, bitVal);
                }
                else if((i >= 32) && (i <= 39)){    // fifth byte
                    uint8_t bitPos = 39 - i;
                    BIT_TO_VAL(dataSet.parity_bits, bitPos, bitVal);
                }
            }
        }

    public:
        /** reads new data from sensor */
        void loadData(void)
        {
            performHandshake();
            read40BitsIntoDataSet();
            if(parityCheckIsValid()){
                temperature = calculateTemperature();
                humidity = calculateHumidity();
            }
        }

        /** @note: returns previous temperature if not refreshed first */
        double getTemperature(void)
        {
            return temperature;
        }

        /** @note: returns previous humidity if not refreshed first */
        double getHumidity(void)
        {
            return humidity;
        }

};


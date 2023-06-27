#pragma once
#include "dhtDataSet.hpp"


/** interface class for the dht single wire bus protocol */
class DHT_PROTOCOL_INTERFACE
{
    protected:
        DHTdataSet rawData;


    protected:

        /** initates hardware for protocol comunication
            1.) wait 1s until sensor is initated, while dataLinePin remains pulled-up to VDD
            2.) initiate pins to starting configuration
        */
        virtual void init(void) = 0;

        /** helper for interpreting data pulses
            @return 0 for pulse of 27us
            @retrun 1 for pulse of 70us    (40us < x < 100us)
        */
        bool pulseTime2Bit(const unsigned long pulseTime_us)
        {
            return ((pulseTime_us > 40) && (pulseTime_us < 100));
        }

        /** performing handshake with the sensor starts the transmission
            1.) send start signal: pull the dataLinePin to GND for >18ms
            2.) receive response: wait until sensor pulls the line low (80us) and high (80us)
            3.) now data transmission will be sent.
        */
        virtual void performHandshake(void) = 0;

        /** reads 40 bits of data and stores them to the according raw data byte.
        
            BYTE:   [ 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 ]

            [ 0 - 7 ],  [ 8 - 15],  [16 - 23],  [24 - 31],  [32 - 39]
            1111 1111,  1111 1111,  1111 1111,  1111 1111,  1111 1111
            humi_hi,    humi_lo,    temp_hi,    temp_lo,    parity
        */
        virtual void read40Bits(void) = 0;


    public:
        double temperature;
        double humidity;
        /** reads new data from sensor */
        void loadData(void)
        {
            performHandshake();
            read40Bits();
            if(rawData.isValid()){
                temperature = rawData.calcTemperature();
                humidity = rawData.calcHumidity();
            }
        }
};


/* single bus protocol data interpretation */

struct DHTdataSet
{   
    public:
        static const double absolutZero = -273.15; 

    public:
        // DHT single wire bus protocoll: 40-bit transfer
        uint8_t humidity_hi;
        uint8_t humidity_lo;       // always 0 for dht11
        uint8_t temperature_hi;
        uint8_t temperature_lo;    // always 0 for dht11
        uint8_t parity_bits;        // sum of all other bytes

    public:
        DHTdataSet(void){
            humidity_hi = 0b00000000;
            humidity_lo = 0b00000000;
            temperature_hi = 0b00000000;
            temperature_lo = 0b00000000;
            parity_bits = 0b00000000;
        }

    public:
        bool isValid(void)
        {
            uint16_t checkSum16 = humidity_hi + humidity_lo + temperature_hi + temperature_lo;
            uint8_t checkSum8 = (uint8_t)checkSum16;    // cut off bits higher position 7
            return (checkSum8 == parity_bits);
        }

        double getHumidity(void)
        {
            if(humidity_lo == 0){
                return (double)humidity_hi;
            }
            else{
                uint16_t humidity_x10 = (humidity_hi << 8 | humidity_lo);
                return (double)(humidity_x10 * 0.1);
            }
        }

        double getTemperature(void)
        {
            uint8_t highByte = temperature_hi;
            uint8_t lowByte = temperature_lo;
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
};

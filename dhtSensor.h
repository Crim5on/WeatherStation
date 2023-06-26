

// TODO: implement DHTSensor class

class DHTSensor{

};







/** @returns the time waited in microseconds */
static inline unsigned long busyWaitUntilPinIsVal(const uint8_t pin, const bool val)
{
    unsigned long timeStampStart = micros();
    while(digitalRead(pin) != val){
        ; // wait
    }
    unsigned long timeStampStop = micros();
    unsigned long timePassed = timeStampStop - timeStampStart;
    return timePassed;
}

static inline bool bitFromSignalTime(const unsigned long hiTime)
{
    // t_hi ~=  27us -> 0 bit
    // t_hi ~=  70us -> 1 bit
    return ((hiTime > 40) && (hiTime < 100));
}

void initiateDHTSensor(void)
{
    // 1.) INITIATE: wait 1s until sensor is initated, while data line remains high by pull-up resisitor
    pinMode(DATA_LINE, INPUT_PULLUP);       //TODO: enable internal pull-up or use external pull-up instead.
    delay(1000);
}

void performHandshake(void)
{
    // SEND START SIGNAL: pull the line down to GND for >18ms
    pinMode(DATA_LINE, OUTPUT);
    digitalWrite(DATA_LINE, LOW);
    delay(20);
    digitalWrite(DATA_LINE, HIGH);

    // READ RESPONSE SIGNAL: wait until sensor pulls the line low (80us) and high (80us)
    pinMode(DATA_LINE, INPUT);
    busyWaitUntilPinIsVal(DATA_LINE, LOW);
    busyWaitUntilPinIsVal(DATA_LINE, HIGH);
    // now ready to recieve data.
}

void read40BitsIntoDataSet(uint8_t dataLine, DHTdataSet& dataSet)
{
    /*  bits in byte:   [ 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 ]

        // bits as they are received:
        [ 0 - 7 ],  [ 8 - 15],  [16 - 23],  [24 - 31],  [32 - 39]
        1111 1111,  1111 1111,  1111 1111,  1111 1111,  1111 1111
        hum_hi,     hum_lo,     temp_hi,    temp_lo,    parity
    */

    for(uint8_t i=0; i<40; i++){
        unsigned long hiTime = busyWaitUntilPinIsVal(DATA_LINE, HIGH);
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

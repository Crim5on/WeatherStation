#include "arduino_hal/dhtSensor_Arduino.hpp"

#define SAMPLING_RATE_MS 1000


static DHTSensor tempHumSensor = DHTSensor(2);


void setup() 
{
    Serial.begin(9600);
    tempHumSensor.init();
}


void loop() 
{
    tempHumSensor.loadData();

    Serial.println("temperature: " + (String)tempHumSensor.temperature + "\thumidity: " + (String)tempHumSensor.humidity);

    delay(SAMPLING_RATE_MS);
}

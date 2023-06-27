#include "dhtSensor.hpp"

#define SAMPLING_RATE_MS 1000

static DHTSensor dhtSensor = DHTSensor(2);


void setup() 
{
    Serial.begin(9600);
    dhtSensor.init();
}



void loop() 
{
    dhtSensor.loadData();
    double temperature = dhtSensor.getTemperature();
    double humidity = dhtSensor.getHumidity();

    // PRINT DATA:
    Serial.println("temperature: " + (String)temperature + "\thumidity: " + (String)humidity);

    delay(SAMPLING_RATE_MS);
}

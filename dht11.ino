#include "bit.h"
#include "dhtDataSet.h"
#include "dhtSensor.h"

#define DATA_LINE 2
#define SAMPLING_RATE_MS 1000






/* Arduino Standard Code */

// global data set
static DHTdataSet g_dataSet;
static double g_temperature = g_dataSet.absolutZero;
static double g_humidity = -1;

void setup() 
{
    Serial.begin(9600);
    initiateDHTSensor();
}



void loop() 
{
    performHandshake();

    // RECEIVE NEW DATA:
    read40BitsIntoDataSet(DATA_LINE, g_dataSet);

    // VALIDATE DATA:
    if(g_dataSet.isValid()){
        g_temperature = g_dataSet.getTemperature();
        g_humidity = g_dataSet.getHumidity();
    }

    // PRINT DATA:
    Serial.println("temperature: " + (String)g_temperature + "\thumidity: " + (String)g_humidity);

    delay(SAMPLING_RATE_MS);
}

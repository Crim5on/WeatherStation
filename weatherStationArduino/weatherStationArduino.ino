#include "DHTdataSet.h"
#include "DHTprotocol.h"

#define SAMPLING_RATE_MS 1000
#define DHT_DATA_PIN 2
#define ABSOLUT_ZERO -273.15

static DHTdataSet g_dataSet;
static double g_temperature;
static double g_humidity;


void setup() 
{
    Serial.begin(115200);
    dht_protocol_init(DHT_DATA_PIN);
    g_temperature = ABSOLUT_ZERO;
    g_humidity = -1;
}


void loop() 
{
    bool handshakeSuccess = dht_protocol_performHandshake(DHT_DATA_PIN);
    if(handshakeSuccess){
        bool readDataSuccess = dht_protocol_readData(DHT_DATA_PIN, &g_dataSet);
        if(readDataSuccess){
            g_temperature = dhtDataSet_calcTemperature(&g_dataSet);
            g_humidity = dhtDataSet_calcHumidity(&g_dataSet);
        }
    }

    Serial.println("temperature: " + (String)g_temperature + "\thumidity: " + (String)g_humidity);
    delay(SAMPLING_RATE_MS);
}

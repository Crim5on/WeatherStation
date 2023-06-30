#include "DHTdataSet.h"
#include "DHTprotocol.h"

#define SAMPLING_RATE_MS 2000
#define DHT_DATA_PIN 2
#define ABSOLUT_ZERO -273.15

static DHTdataSet g_dataSet;
static double g_temperature;
static double g_humidity;
static bool g_ledPinState;


void setup() 
{
    Serial.begin(115200);
    Serial.print("--------------------------------------------------\n");
    dht_protocol_init(DHT_DATA_PIN);
    pinMode(LED_BUILTIN, OUTPUT);
    g_ledPinState = LOW;
    g_temperature = ABSOLUT_ZERO;
    g_humidity = -1;
}


void loop() 
{
    g_ledPinState = !g_ledPinState;
    digitalWrite(LED_BUILTIN, g_ledPinState);

    bool handshakeSuccess = dht_protocol_performHandshake(DHT_DATA_PIN);
    if(handshakeSuccess){
        bool readDataSuccess = dht_protocol_readData(DHT_DATA_PIN, &g_dataSet);
        if(readDataSuccess){
            g_temperature = dhtDataSet_calcTemperature(&g_dataSet);
            g_humidity = dhtDataSet_calcHumidity(&g_dataSet);
        }
    }

    Serial.println("Temperature: " + (String)g_temperature + "*C\tHumidity: " + (String)g_humidity + "%");
    delay(SAMPLING_RATE_MS);
}

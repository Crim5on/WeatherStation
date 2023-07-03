#include "DHTdataSet.h"
#include "DHTprotocol.h"

#define DHT_DATA_PIN 2
#define SAMPLING_RATE_MS 2000
#define ABSOLUT_ZERO -273.15

static DHTdataSet g_dataSet;
static double g_temperature;
static double g_humidity;
static bool g_ledPinState;


void pinToggle(uint8_t pin)
{
    static bool pinState = LOW;
    pinState = !pinState;
    digitalWrite(pin, (bool)pinState);
}


void setup() 
{
    Serial.begin(115200);
    Serial.println("--------------------------------------------------");
    dht_protocol_init(DHT_DATA_PIN);
    pinMode(LED_BUILTIN, OUTPUT);
    g_ledPinState = LOW;
    g_temperature = ABSOLUT_ZERO;
    g_humidity = -1;
}


void loop() 
{
    //pinToggle(LED_BUILTIN);

    if(dht_protocol_readData(DHT_DATA_PIN, &g_dataSet)){
        g_temperature = dhtDataSet_calcTemperature(&g_dataSet);
        g_humidity = dhtDataSet_calcHumidity(&g_dataSet);
    }
    else{
        g_temperature = ABSOLUT_ZERO;
        g_humidity = -1;
    }
    
    Serial.println("Temperature: " + (String)g_temperature + "*C\tHumidity: " + (String)g_humidity + "%");
    delay(SAMPLING_RATE_MS);
}

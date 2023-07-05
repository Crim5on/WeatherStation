#include "DHTdataSet.h"
#include "DHTprotocol.h"

#define DHT_DATA_PIN 2
#define SAMPLING_RATE_MS 2000
#define ABSOLUT_MIN_VAL -128

static DHTdataSet g_dataSet;
static int8_t g_temperature;
static int8_t g_humidity;
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
    g_temperature = ABSOLUT_MIN_VAL;
    g_humidity = ABSOLUT_MIN_VAL;
}


void loop() 
{
    pinToggle(LED_BUILTIN);

    if(dht_protocol_readData(DHT_DATA_PIN, &g_dataSet)){
        g_temperature = dhtDataSet_calcTemperature(&g_dataSet);
        g_humidity = dhtDataSet_calcHumidity(&g_dataSet);
    }
    else{
        g_temperature = ABSOLUT_MIN_VAL;
        g_humidity = ABSOLUT_MIN_VAL;
    }
    
    Serial.println("Temperature: " + (String)g_temperature + "*C\tHumidity: " + (String)g_humidity + "%");
    delay(SAMPLING_RATE_MS);
}

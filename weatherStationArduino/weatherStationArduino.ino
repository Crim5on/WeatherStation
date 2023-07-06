#include "DHTdataSet.h"
#include "DHTprotocol.h"
#include <LiquidCrystal.h>

#define DHT_DATA_PIN A5
#define SAMPLING_RATE_MS 2000
#define ABSOLUT_MIN_VAL -128
// LCD pin to GPIO mapping
#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

const  char degreeSymbol = 0xDF;
static double g_temperature;
static double g_humidity;
static bool g_ledPinState;
static DHTdataSet g_dataSet;
static LiquidCrystal LCDisplay(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


void pinToggle(uint8_t pin)
{
    static bool pinState = LOW;
    pinState = !pinState;
    digitalWrite(pin, (bool)pinState);
}


void setup() 
{
    Serial.begin(115200);
    Serial.println("-----------------------------");
    LCDisplay.begin(16, 2);
    LCDisplay.print("Avi's Embedded");
    LCDisplay.setCursor(0, 1);
    LCDisplay.print("Weather Station");
    pinMode(DHT_DATA_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(3500);
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
    String temperature_str = String("Temp: " + String(g_temperature, 1) + degreeSymbol + "C");
    String humidity_str = String("Humi: " + String(g_humidity, 1) + " %");
    
    Serial.println(temperature_str + "\t" + humidity_str);
    LCDisplay.clear();
    LCDisplay.print(temperature_str);
    LCDisplay.setCursor(0, 1);
    LCDisplay.print(humidity_str);
    delay(SAMPLING_RATE_MS);
}

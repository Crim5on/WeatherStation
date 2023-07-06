# Embedded Weather Station
An embedded weather station feauturing a DHTxx series temperature and humidity sensor and a 2x16 LCD display for data output. Integration of a barometer for thunderstorm detection and buzzer alarm is planed. Supports Arduino Uno and STM32 Nuclueo boards. (might need to be upgraded to a Arduino Mega bc of pin shortage)


## Directories
* `weatherStationArduino` contains full implementation for Arduino Uno
* `weatherStationSTM32` contains full implementation for STM32 Nucleo


# Components

## DHT11
* Supply voltage (VDD): 3V3 - 5V5
* Supply current: 0.3mA while measuring, 60uA while in standby
* 2.5mA max current use during conversion (while requesting data)
* Humidity range: 20 - 80% with 5% accuracy
* Temperature range: 0 - 50°C with ±2°C accuracy
* Sampling period: every 1 second
* dimesions: 15.5mm x 12mm x 5.5mm - (0.1" pin spacing)


## DHT22
* Supply voltage (VDD): 3V3 - 5V5
* Supply current: 2.5mA while measuring
* Humidity range: 0 - 100% with 2-5% accuracy
* Temperature range: -40 - 125°C with ±0.5°C accuracy
* Sampling period: every 2 seconds
* dimesions: 15.1mm x 25mm x 7.7mm - (0.1" pin spacing)


## LCD Display
* Standard 16X2 LCD display compatible with the Hitachi HD44780 driver
* 10KΩ potentiometer for adjusting contrast (empirically determined value = xxΩ)
* 220Ω resisitor required to drop 5V supply voltage for backlight led's


## Barometer
Evaluate: https://store.arduino.cc/products/grove-barometer-sensor-bmp280?variant=35573310980247

## Buzzer
  

## MCUs
* Arduino Uno Rev3 SMD  (tested)
* STM32 Nucleo-64 development board with STM32F401RE MCU

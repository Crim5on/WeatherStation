/************** MAKE CHANGES HERE ********************/
#include "stm32f4xx_hal.h"

#define TYPE_DHT11    // define according to your sensor
//#define TYPE_DHT22

#define DHT_PORT GPIOA
#define DHT_PIN GPIO_PIN_10
#include "helpers.h"

/*******************************************     NO CHANGES AFTER THIS LINE      ****************************************************/

#include "DHT.h"
#include "helpers.h"

void DHT_Start(void)
{
	DWT_Delay_Init();
	pinModeOutput(DHT_PORT, DHT_PIN);  // set the pin as output
	HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 0);   // pull the pin low

#if defined(TYPE_DHT11)
	delay_us(18000);   // wait for 18ms
#endif

#if defined(TYPE_DHT22)
	delay_us (1200);  // >1ms delay
#endif

	HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 1);   // pull the pin high
	delay_us(20);   // wait for 30us
	pinModeInput(DHT_PORT, DHT_PIN);    // set as input
}

bool DHT_Check_Response(void)
{
	bool response = false;
	delay_us(40);
	if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == LOW) {
		delay_us(80);
		if (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == HIGH) {
			response = true;
		}
	}
	bool waitedSuccessfully = busyWaitWhile(DHT_PORT, DHT_PIN, HIGH);
	return (response && waitedSuccessfully);
}

uint8_t DHT_Read(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++) {
		while (!(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN))) {
			;   // wait for the pin to go high
		}
		delay_us(40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)))   // if the pin is low
		{
			i &= ~(1 << (7 - j));   // write 0
		}
		else {
			i |= (1 << (7 - j));  // if the pin is high, write 1
		}
		while ((HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN))) {
			;  // wait for the pin to go low
		}
	}
	return i;
}

bool DHT_GetData(DHTdataSet *DHT_Data)
{
	DHT_Start();
	if (DHT_Check_Response() == false) {
		return false;
	}
	uint8_t Rh_byte1 = DHT_Read();
	uint8_t Rh_byte2 = DHT_Read();
	uint8_t Temp_byte1 = DHT_Read();
	uint8_t Temp_byte2 = DHT_Read();
	uint16_t SUM = DHT_Read();

	bool parityCheckIsValid = (SUM == (Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2));
	if (parityCheckIsValid) {
#if defined(TYPE_DHT11)
		DHT_Data->temperature = Temp_byte1;
		DHT_Data->humidity = Rh_byte1;
#endif
#if defined(TYPE_DHT22)
		DHT_Data->temperature = ((Temp_byte1<<8)|Temp_byte2);
		DHT_Data->humidity = ((Rh_byte1<<8)|Rh_byte2);
#endif
	}
	return parityCheckIsValid;
}


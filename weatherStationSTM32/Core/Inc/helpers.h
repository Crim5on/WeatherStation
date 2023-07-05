/*
 * helpers.h
 *
 *  Created on: Jul 3, 2023
 *      Author: sandro
 */

#ifndef INC_HELPERS_H_
#define INC_HELPERS_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "string.h"

#define UART_TIMEOUT 100

/** prints string on serial line. */
void printSerialLine(UART_HandleTypeDef *handle, char stringBuffer[])
{
	(void) strcat(stringBuffer, "\r\n");	// adds newLine and nullByte.
	HAL_UART_Transmit(handle, (uint8_t*) stringBuffer, strlen(stringBuffer), UART_TIMEOUT);
}

/** prints string on serial line. */
void printValuesSerialLine(UART_HandleTypeDef *handle, int temperature, int humidity)
{
	char stringBuffer[255] = { '\0' };
	char numberString[8] = { '\0' };
	// build serial line:
	(void) strcat(stringBuffer, "Temperature: ");
	(void) sprintf(numberString, "%i", temperature);
	(void) strcat(stringBuffer, numberString);
	(void) strcat(stringBuffer, "*C \tHumidity: ");
	(void) sprintf(numberString, "%i", humidity);
	(void) strcat(stringBuffer, numberString);
	(void) strcat(stringBuffer, "%");
	// send to uart to print (appends line-break)
	printSerialLine(handle, stringBuffer);
}

void delayMicroseconds(TIM_HandleTypeDef *timerHandle, uint16_t delay)
{
	uint16_t timeStampStart = __HAL_TIM_GET_COUNTER(timerHandle);
	while ((__HAL_TIM_GET_COUNTER(timerHandle) - timeStampStart) < delay) {
		; // wait
	}
}

#endif /* INC_HELPERS_H_ */

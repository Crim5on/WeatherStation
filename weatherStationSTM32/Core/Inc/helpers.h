/*
 * helpers.h
 *
 *  Created on: Jul 3, 2023
 *      Author: sandro
 */

#ifndef INC_HELPERS_H_
#define INC_HELPERS_H_

#include "stm32f4xx_hal.h"
#include "string.h"
#define UART_TIMEOUT 100

/** prints string on serial line. */
void printSerialLine(UART_HandleTypeDef* handle, char string[])
{
	(void)strcat(string, "\n\r");	// adds newLine and nullByte.
	HAL_UART_Transmit(handle, (uint8_t *)string, strlen(string), UART_TIMEOUT);
}



void delayMicroseconds(TIM_HandleTypeDef* timerHandle, uint16_t delay)
{
	uint16_t timeStampStart = __HAL_TIM_GET_COUNTER(timerHandle);
	while((__HAL_TIM_GET_COUNTER(timerHandle) - timeStampStart) < delay){
		; // wait
	}
}

#endif /* INC_HELPERS_H_ */

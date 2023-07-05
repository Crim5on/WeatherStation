/*
 * helpers.c
 *
 *  Created on: Jul 5, 2023
 *      Author: sandro
 */

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "helpers.h"


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

void pinModeOutput(GPIO_TypeDef *port, uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void pinModeInput(GPIO_TypeDef *port, uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

uint32_t DWT_Delay_Init(void)
{
	/* Disable TRC */
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
	/* Enable TRC */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

	/* Disable clock cycle counter */
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
	/* Enable  clock cycle counter */
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

	/* Reset the clock cycle counter value */
	DWT->CYCCNT = 0;

	/* 3 NO OPERATION instructions */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");

	/* Check if clock cycle counter has started */
	if (DWT->CYCCNT) {
		return 0; /*clock cycle counter started*/
	}
	else {
		return 1; /*clock cycle counter not started*/
	}
}

/** delays for given microseconds */
void delay_us(volatile uint32_t microseconds)
{
	uint32_t clk_cycle_start = DWT->CYCCNT;

	/* Go to number of cycles for system */
	microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

	/* Delay till end */
	while ((DWT->CYCCNT - clk_cycle_start) < microseconds) {
		;	// wait
	}
}

bool busyWaitWhile(GPIO_TypeDef *port, uint16_t pin, bool val)
{
	for (uint16_t loopCount = 1; HAL_GPIO_ReadPin(port, pin) == val; loopCount++) {
		if (loopCount == 0) {	//overflow
			return false;
		}
	}
	return true;
}


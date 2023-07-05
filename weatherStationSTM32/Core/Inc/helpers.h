/*
 * helpers.h
 *
 *  Created on: Jul 3, 2023
 *      Author: sandro
 */

#ifndef INC_HELPERS_H_
#define INC_HELPERS_H_

#define HIGH 1
#define LOW 0

#include <stdbool.h>

/** prints string on serial line. */
void printSerialLine(UART_HandleTypeDef *handle, char stringBuffer[]);

/** prints string on serial line. */
void printValuesSerialLine(UART_HandleTypeDef *handle, int temperature, int humidity);

void pinModeOutput(GPIO_TypeDef *port, uint16_t pin);

void pinModeInput(GPIO_TypeDef *port, uint16_t pin);

uint32_t DWT_Delay_Init(void);

void delay_us(volatile uint32_t microseconds);

/** @returns true if waited successfully, false if timed out. */
bool busyWaitWhile(GPIO_TypeDef *port, uint16_t pin, bool val);

#endif /* INC_HELPERS_H_ */

/*
 * DHTprotocol.h
 *
 *  Created on: Jun 29, 2023
 *      Author: sandro
 */

#ifndef INC_DHTPROTOCOL_H_
#define INC_DHTPROTOCOL_H_

#include "bit.h"
#include "DHTdataSet.h"
#include "stm32f4xx_hal.h"
#include "helpers.h"

#define LOOP_UPPER_BOUND 10000
#define PULSE_TIMEOUT_US 20000

#define LOW 0
#define HIGH 1

/** @returns waited time in microseconds */
static uint16_t busyWaitWhile(TIM_HandleTypeDef *timerHandle, GPIO_TypeDef *port, uint16_t dataPin,
		const GPIO_PinState val)
{
	uint16_t loopCount = 0;
	uint16_t timeStampStart = __HAL_TIM_GET_COUNTER(timerHandle);

	while ((HAL_GPIO_ReadPin(port, dataPin) == val) && (loopCount < LOOP_UPPER_BOUND)) {
		; // wait
		loopCount++;
	}
	uint16_t timeStampStop = __HAL_TIM_GET_COUNTER(timerHandle);
	uint16_t timePassed = timeStampStop - timeStampStart;
	return timePassed;
}

/** helper for interpreting data pulses
 @returns 0 for pulse of ~27us
 @retruns 1 for pulse of ~70us
 */
static inline bool pulse2Bit(const uint16_t pulseTime_us)
{
	return ((pulseTime_us > 50) && (pulseTime_us < 90));
}

/** @returns true if protocol handshake was successful */
static bool dht_protocol_performHandshake(TIM_HandleTypeDef *timerHandle, GPIO_TypeDef *port, uint16_t dataPin)
{
	/*Re-Configure GPIO pin to OUTPUT : dhtDataPin_Pin */
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = dataPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	// SEND START SIGNAL:
	HAL_GPIO_WritePin(port, dataPin, LOW);
	HAL_Delay(20);  // pull LOW for >18ms
	HAL_GPIO_WritePin(port, dataPin, HIGH);
	delayMicroseconds(timerHandle, 10);  // to ensure pin is set high

	/*Re-Configure GPIO pin to INPUT : dhtDataPin_Pin */
	GPIO_InitStruct.Pin = dataPin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	// READ RESPONSE SIGNAL: wait until sensor pulls the line low (80us) and high (80us)
	uint16_t loTime = busyWaitWhile(timerHandle, port, dataPin, LOW);
	uint16_t hiTime = busyWaitWhile(timerHandle, port, dataPin, HIGH);
	// if impulses ~80us sensor will send data.
	return ((loTime < 100) && (hiTime < 100));
}

/** reads 40 bits of data and writes them to the according byte in dataSet.
 @returns true if data read passed parity check.
 */
static bool dht_protocol_receive40bits(TIM_HandleTypeDef *timerHandle, GPIO_TypeDef *port, uint16_t dataPin,
		DHTdataSet *dataSet)
{
	/*  BYTE:   [ 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 ]

	 [ 0 - 7 ],  [ 8 - 15],  [16 - 23],  [24 - 31],  [32 - 39]
	 1111 1111,  1111 1111,  1111 1111,  1111 1111,  1111 1111
	 humi_hi,    humi_lo,    temp_hi,    temp_lo,    parity
	 */
	for (uint8_t i = 0; i < 40; i++) {
		uint16_t loTime = busyWaitWhile(timerHandle, port, dataPin, LOW);
		uint16_t hiTime = busyWaitWhile(timerHandle, port, dataPin, HIGH);

		if ((loTime > PULSE_TIMEOUT_US) || (hiTime > PULSE_TIMEOUT_US)) {
			HAL_Delay(1000);
			return false;
		}

		if ((i >= 0) && (i <= 7)) {           // first byte
			BIT_TO_VAL(dataSet->humiByte_hi, (7 - i), pulse2Bit(hiTime));
		}
		else if ((i >= 8) && (i <= 15)) {     // second byte
			BIT_TO_VAL(dataSet->humiByte_lo, (15 - i), pulse2Bit(hiTime));
		}
		else if ((i >= 16) && (i <= 23)) {     // third byte
			BIT_TO_VAL(dataSet->tempByte_hi, (23 - i), pulse2Bit(hiTime));
		}
		else if ((i >= 24) && (i <= 31)) {    // fourth byte
			BIT_TO_VAL(dataSet->tempByte_lo, (31 - i), pulse2Bit(hiTime));
		}
		else if ((i >= 32) && (i <= 39)) {    // fifth byte
			BIT_TO_VAL(dataSet->parity_byte, (39 - i), pulse2Bit(hiTime));
		}
	}
	return dhtDataSet_parityCheck(dataSet);
}

/** @returns true if filled valid data into dataSet, false otherwise */
bool dht_protocol_readData(TIM_HandleTypeDef *timerHandle, GPIO_TypeDef *port, uint16_t dataPin, DHTdataSet *dataSet)
{
	if (dht_protocol_performHandshake(timerHandle, port, dataPin)) {
		return dht_protocol_receive40bits(timerHandle, port, dataPin, dataSet);
	}
	return false;
}

#endif /* INC_DHTPROTOCOL_H_ */

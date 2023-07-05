/*
 * DHT.h
 *
 *  Created on: Jun 28, 2020
 *      Author: Controllerstech.com
 *
 *	Modified on: July 05, 2023
 *		Author: Sandro Schnetzer
 */

#ifndef DHT_H_
#define DHT_H_

typedef struct
{
	float temperature;
	float humidity;
} DHTdataSet;

bool DHT_GetData(DHTdataSet *DHT_Data);

#endif /* INC_DHT_H_ */

/*
 * historicalData.h
 *
 *  Created on: 24.06.2019
 *      Author: TomaszSok
 */

#ifndef HISTORICALDATA_H_
#define HISTORICALDATA_H_

#include <stdint.h>

#define HIST_DATA_SIZE 3600

typedef struct
{
	float *head;
	float *tail;
	uint16_t numberOfItems;
	float data[HIST_DATA_SIZE];
}HIST_DATA;

extern HIST_DATA historicalData[];

void HIST_DATA_Init(HIST_DATA *histData);
void HIST_DATA_AddNew(HIST_DATA *histData, float newValue);

#endif /* HISTORICALDATA_H_ */

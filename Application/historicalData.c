/*
 * historicalData.c
 *
 *  Created on: 24.06.2019
 *      Author: TomaszSok
 */

#include "historicalData.h"


#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

SDRAM HIST_DATA historicalData[100];

void HIST_DATA_Init(HIST_DATA *histData)
{
	histData->head = &histData->data[0];
	histData->tail = &histData->data[0];
	histData->numberOfItems = 0;
	for (int i = 0; i < HIST_DATA_SIZE; ++i)
		histData->data[i] = 0;
}

void HIST_DATA_AddNew(HIST_DATA *histData, float newValue)
{
	if(histData->head == &histData->data[HIST_DATA_SIZE-1])
		histData->head = &histData->data[0];
	else
		histData->head++;

	if(histData->head == histData->tail)
	{
		if(histData->tail == &histData->data[HIST_DATA_SIZE-1])
			histData->tail = &histData->data[0];
		else
			histData->tail++;
	}

	*histData->head = newValue;
	if(	histData->numberOfItems < HIST_DATA_SIZE)
		histData->numberOfItems++;
	else if(histData->numberOfItems > HIST_DATA_SIZE)
		histData->numberOfItems = HIST_DATA_SIZE;
}

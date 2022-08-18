/*
 * MeasurementsSynchronization.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#include "ChannelsSynchronization.h"
#include "FreeRTOS.h"
#include "semphr.h"

xSemaphoreHandle xSemaphoreChannels;

void InitChannelsMutex(void)
{
	xSemaphoreChannels = xSemaphoreCreateMutex();
}

uint32_t TakeChannelsMutex(uint32_t timeout)
{
	if(xSemaphoreTake( xSemaphoreChannels, timeout) == pdTRUE)
		return 1;
	else
		return 0;
}

void GiveChannelsMutex(void)
{
	xSemaphoreGive(xSemaphoreChannels);
}

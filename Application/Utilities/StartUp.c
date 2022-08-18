/*
 * StartUp.c
 *
 *  Created on: 22.02.2017
 *      Author: Tomaszs
 */

#include "StartUp.h"

#include "FreeRTOS.h"
#include "event_groups.h"

EventGroupHandle_t taskSequence;

void STARTUP_Init(void)
{
	taskSequence = xEventGroupCreate();
	xEventGroupSetBits(taskSequence, 0x0000);
}

uint32_t STARTUP_WaitForBits(uint32_t bits)
{
	if ((xEventGroupWaitBits(taskSequence, bits,pdFALSE, pdFALSE, 100) & bits) == bits)
		return 1;
	else
		return 0;
}


void STARTUP_SetBits(uint32_t bits)
{
	xEventGroupSetBits(taskSequence, bits);
}

void STARTUP_ClaerBits(uint32_t bits)
{
	xEventGroupClearBits(taskSequence, bits);
}

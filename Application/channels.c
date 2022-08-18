/*
 * channels.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#include "channels.h"
#include <math.h>
#include <string.h>
#include "skins.h"
#include "fram.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

SDRAM CHANNEL Channels[100];
SDRAM CHANNEL bkChannels[100];

uint16_t NumberOfHistoryItems = 0;

void CHANNELS_Initialize(void)
{
	uint8_t ResetTimeAndDateBuffer[5];
	for (int channelNumber = 0; channelNumber < 100; channelNumber++)
	{
		Channels[channelNumber].value = 0.0;
		Channels[channelNumber].failureState = 0;
		Channels[channelNumber].firstIterration = 1;

		FRAM_ReadMultiple(START_MINIMUM_ADDR + 4 * channelNumber, (uint8_t*) &Channels[channelNumber].min, 4);
		if (isfinite(Channels[channelNumber].min) == 0)
			Channels[channelNumber].min = 1000000000.0;

		FRAM_ReadMultiple(START_MAXIMUM_ADDR + 4 * channelNumber, (uint8_t*) &Channels[channelNumber].max, 4);
		if (isfinite(Channels[channelNumber].max) == 0)
			Channels[channelNumber].max = -1000000000.0;

		FRAM_ReadMultiple(START_COUNTER_ADDR + 16 * channelNumber, (uint8_t*) &Channels[channelNumber].Tot1.value, 8);
		if (__fpclassifyd(Channels[channelNumber].Tot1.value) != FP_NORMAL)
			Channels[channelNumber].Tot1.value = 0.0;

		FRAM_ReadMultiple(START_COUNTER_ADDR + 16 * channelNumber + 8, (uint8_t*) &Channels[channelNumber].Tot2.value, 8);
		if (__fpclassifyd(Channels[channelNumber].Tot2.value) != FP_NORMAL)
			Channels[channelNumber].Tot2.value = 0.0;

		Channels[channelNumber].Color = SKINS_GetTextColor();

		for (int j = 0; j < 2; j++)
		{
			Channels[channelNumber].alarm[j].state = 0;
			Channels[channelNumber].alarm[j].AckAlarm = 1;
		}

		Channels[channelNumber].histData = &historicalData[channelNumber];
		HIST_DATA_Init(Channels[channelNumber].histData);

		FRAM_ReadMultiple(START_RESET_DATE_ADDR + 5 * channelNumber, (uint8_t*) &ResetTimeAndDateBuffer, 5);

		if (ResetTimeAndDateBuffer[0] > 31)
			Channels[channelNumber].ResetDate.Date = 1;
		else
			Channels[channelNumber].ResetDate.Date = ResetTimeAndDateBuffer[0];
		if (ResetTimeAndDateBuffer[1] > 12)
			Channels[channelNumber].ResetDate.Month = 1;
		else
			Channels[channelNumber].ResetDate.Month = ResetTimeAndDateBuffer[1];
		if (ResetTimeAndDateBuffer[2] > 99)
			Channels[channelNumber].ResetDate.Year = 16;
		else
			Channels[channelNumber].ResetDate.Year = ResetTimeAndDateBuffer[2];
		if (ResetTimeAndDateBuffer[3] > 23)
			Channels[channelNumber].ResetTime.Hours = 0;
		else
			Channels[channelNumber].ResetTime.Hours = ResetTimeAndDateBuffer[3];
		if (ResetTimeAndDateBuffer[4] > 59)
			Channels[channelNumber].ResetTime.Minutes = 0;
		else
			Channels[channelNumber].ResetTime.Minutes = ResetTimeAndDateBuffer[4];

		Channels[channelNumber].ResetTime.Seconds = 0;
	}
}

void GetChannel(CHANNEL * Channel, int index)
{
	*Channel = Channels[index];
}

void SetChannel(const CHANNEL * Channel, int index)
{
	Channels[index] = *Channel;
}

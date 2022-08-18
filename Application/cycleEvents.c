/*
 * cycleEvents.c
 *
 *  Created on: 27 paź 2015
 *      Author: Tomaszs
 */

#include <channels.h>
#include <cycleEvents.h>
#include <watchdog.h>
#include "rtc.h"
#include "FreeRTOS.h"
#include "task.h"#include "archive.h"#include "parameters.h"
#include "UserMessage.h"
#include "StartUp.h"
#include "ChannelsSynchronization.h"
#include "smtp_netconn.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

xTaskHandle vtaskCycleEventsHandle;

enum
{
	CHECK_DATE = 0, NEW_DAY, NEW_WEEK, NEW_MONTH, RESET_TOTALIZERS
};

void CreateNewCyclicArchive();
uint8_t ResetCycleTotalizers(uint8_t resetDayTotalizers, uint8_t resetWeekTotalizers, uint8_t resetMonthTotalizers);

void vtaskCycleEvents(void *pvParameters)
{
	unsigned char resetDayTotalizers = 0, resetWeekTotalizers = 0, resetMonthTotalizers = 0;
	RTC_DateTypeDef prevRTCDate;
	RTC_TimeTypeDef CycleEventsTime;
	RTC_DateTypeDef CycleEventsDate;
	RTC_GetTimeAndDate(&CycleEventsTime, &CycleEventsDate);
	prevRTCDate = CycleEventsDate;
	uint8_t CycleEventsState = CHECK_DATE;

	while (1)
	{
		if (STARTUP_WaitForBits(0x0200))
		{
			STARTUP_ClaerBits(0x0200);
			STARTUP_SetBits(0x0400);

			while (1)
			{
				WDFlags[6] = ALIVE;
				switch (CycleEventsState)
				{
				case CHECK_DATE:
					RTC_GetTimeAndDate(&CycleEventsTime, &CycleEventsDate);

					SMTP_SendCyceledEmail(&CycleEventsTime, &CycleEventsDate);

					if (GeneralSettings.DSTMode == 1)
						DST_AutoChange(&CycleEventsTime, &CycleEventsDate);

					if (CycleEventsDate.Date != prevRTCDate.Date)
					{
						CycleEventsState = NEW_DAY;
						prevRTCDate = CycleEventsDate;
					}
					else
						vTaskDelay(250);
					break;

				case NEW_DAY:
					if (ArchiveSettings.ArchiveMaxSize == 0)
						CreateNewCyclicArchive();

					resetDayTotalizers = 1;
					if (CycleEventsDate.WeekDay == RTC_WEEKDAY_MONDAY)
						CycleEventsState = NEW_WEEK;
					else if (CycleEventsDate.WeekDay != RTC_WEEKDAY_MONDAY && CycleEventsDate.Date == 1)
						CycleEventsState = NEW_MONTH;
					else
						CycleEventsState = RESET_TOTALIZERS;
					break;

				case NEW_WEEK:
					if (ArchiveSettings.ArchiveMaxSize == 1)
						CreateNewCyclicArchive();

					resetWeekTotalizers = 1;
					if (CycleEventsDate.Date == 1)
						CycleEventsState = NEW_MONTH;
					else
						CycleEventsState = RESET_TOTALIZERS;
					break;

				case NEW_MONTH:
					if (ArchiveSettings.ArchiveMaxSize == 2)
						CreateNewCyclicArchive();

					resetMonthTotalizers = 1;
					CycleEventsState = RESET_TOTALIZERS;
					break;

				case RESET_TOTALIZERS:

					if(ResetCycleTotalizers(resetDayTotalizers, resetWeekTotalizers, resetMonthTotalizers))
					{
						resetDayTotalizers = 0;
						resetWeekTotalizers = 0;
						resetMonthTotalizers = 0;
						CycleEventsState = CHECK_DATE;
					}
					break;
				default:
					break;
				}
			}
		}
		else
			continue;
	}
}

void CreateCycleEventsTask(void)
{
	xTaskCreate(vtaskCycleEvents, "vtaskCycleEvents", 1000, NULL, ( unsigned portBASE_TYPE )3, &vtaskCycleEventsHandle);
}

void CreateNewCyclicArchive()
{
	if (ARCHIVE_GetState() == 0)
		ARCHIVE_CreateNewArchives();
	else
	{
		ARCHIVE_SafeStop();
		ARCHIVE_CreateNewArchives();
		ARCHIVE_SafeStart();
	}
}

uint8_t ResetCycleTotalizers(uint8_t resetDayTotalizers, uint8_t resetWeekTotalizers, uint8_t resetMonthTotalizers)
{
	if (TakeChannelsMutex(500))
	{
		for (int i = 0; i < 100; i++)
		{
			if (Channels[i].source.type != 0)
			{
				if (Channels[i].Tot1.type == 3 && resetDayTotalizers == 1)
					Channels[i].Tot1.value = 0;
				else if (Channels[i].Tot1.type == 4 && resetWeekTotalizers == 1)
					Channels[i].Tot1.value = 0;
				else if (Channels[i].Tot1.type == 5 && resetMonthTotalizers == 1)
					Channels[i].Tot1.value = 0;

				if (Channels[i].Tot2.type == 3 && resetDayTotalizers == 1)
					Channels[i].Tot2.value = 0;
				else if (Channels[i].Tot2.type == 4 && resetWeekTotalizers == 1)
					Channels[i].Tot2.value = 0;
				else if (Channels[i].Tot2.type == 5 && resetMonthTotalizers == 1)
					Channels[i].Tot2.value = 0;
			}
		}
		GiveChannelsMutex();
		return 1;
	}
	else
		return 0;
}

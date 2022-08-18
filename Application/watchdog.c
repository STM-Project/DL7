/*
 * watchdog.c
 *
 *  Created on: 2 gru 2015
 *  Author: METRONIC AKP
 */

#include <watchdog.h>
#include "iwdg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "StartUp.h"
#include "archive.h"

uint8_t WDFlags[8];

static int IWDGResetFlag = 0;

xTaskHandle vtaskWatchdogHandle;

void WatchdogRefresh(void)
{
	HAL_IWDG_Refresh(&hiwdg1);
}

void SetWatchdogFlag(void)
{
	IWDGResetFlag = 1;
}

void vtaskWatchdog(void *argument)
{
	int i;
	for (i = 0; i < 8; i++)
		WDFlags[i] = UNKNOWN;

	while (1)
	{
		if (STARTUP_WaitForBits(0x0800))
		{
			if (IWDGResetFlag == 1)
			{
				IWDGResetFlag = 0;
				ARCHIVE_SendEvent("SYS:WATCHDOG RESET");
			}

			MX_IWDG1_Init();
			while (1)
			{
				if (WDFlags[1] == ALIVE && WDFlags[2] == ALIVE && WDFlags[3] == ALIVE && WDFlags[4] == ALIVE && WDFlags[5] == ALIVE
						&& WDFlags[6] == ALIVE && WDFlags[7] == ALIVE)
				{
					WatchdogRefresh();
					for (i = 0; i < 8; i++)
						WDFlags[i] = UNKNOWN;
				}
				vTaskDelay(100);
			}
		}
		else
			continue;
	}
}

void CreateWatchdogTask(void)
{
	xTaskCreate(vtaskWatchdog, "vtaskWatchdog", 800, NULL, ( unsigned portBASE_TYPE )7, &vtaskWatchdogHandle);
}

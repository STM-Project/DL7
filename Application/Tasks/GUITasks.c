/*
 * GUITasks.c
 *
 *  Created on: 16.05.2017
 *      Author: Tomaszs
 */

#include "GUITasks.h"
#include "FreeRTOS.h"
#include "task.h"

#include "StartUp.h"
#include "parameters.h"
#include "fram.h"
#include "backlight.h"
#include "screenSaver.h"

#include "StartUpDLG.h"
#include "TitleBarDLG.h"
#include "MenuBarDLG.h"
#include "touchpanel.h"

extern int WindowsState;
extern int WindowInstance;
extern osSemaphoreId LcdUpdateSemaphoreId;

xTaskHandle vtaskGUIHandle;
xTaskHandle vtaskBSPHandle;


void vtaskBSP(void *pvParameters)
{
	while (1)
	{
		if (STARTUP_WaitForBits(0x0001))
		{
			STARTUP_ClaerBits(0x0001);
			STARTUP_SetBits(0x0002);

			while (1)
			{
				TOUCHPANEL_UpdateState();
				GUI_Exec();
				vTaskDelay(20);
			}
		}
		else
			continue;
	}
}

void vtaskGUI(void *pvParameters)
{
	WM_HWIN hStartUpWindow;
	int i = 0;
	int brightChangeSpeed = 10;

	while (1)
	{
		if (STARTUP_WaitForBits(0x0002))
		{
			hStartUpWindow = CreateSplashScreenDLG();
			GUI_Exec();

			for (i = 0; i <= 100; ++i)
			{
				BACKLIGHT_Set(i);
				vTaskDelay(brightChangeSpeed);
			}
			STARTUP_ClaerBits(0x0002);
			STARTUP_SetBits(0x0004);
			while (1)
			{
				vTaskDelay(200);

				if (STARTUP_WaitForBits(0x0400))
				{
					STARTUP_ClaerBits(0x0400);
					STARTUP_SetBits(0x0800);

					for (i = 100; i >= 0; --i)
					{
						BACKLIGHT_Set(i);
						vTaskDelay(brightChangeSpeed);
					}

					WM_DeleteWindow(hStartUpWindow);
					GUI_Exec();

					WindowsState = FRAM_Read(WINDOWS_STATE_ADDR);
					if (WindowsState < 1 || WindowsState > 7)
						WindowsState = 1;

					WindowInstance = FRAM_Read(WINDOW_INSTANCE_ADDR);
					if (WindowInstance < 0 || WindowInstance > 99)
						WindowInstance = 0;

					CreateTitleBar();
					CreateMenuBar();
					GUI_Exec();

					for (i = 0; i <= GeneralSettings.Brightness; ++i)
					{
						BACKLIGHT_Set(i);
						vTaskDelay(brightChangeSpeed);
					}

					StartScreenSaverTimer();
					vTaskDelete(NULL);
				}
				else
					continue;
			}
		}
		else
			continue;
	}
}

void CreateGUITask(void)
{
	xTaskCreate(vtaskGUI, "vtaskGUI", 1000, NULL, ( unsigned portBASE_TYPE )5, &vtaskGUIHandle);
}

void CreateBSPTask(void)
{
	xTaskCreate(vtaskBSP, "vtaskBSP", 1000, NULL, ( unsigned portBASE_TYPE )2, &vtaskBSPHandle);
}


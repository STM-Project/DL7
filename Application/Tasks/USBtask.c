/*
 * USBtask.c
 *
 *  Created on: 29 lis 2021
 *      Author: TomaszSokolowski
 */

#include "USBtask.h"
#include "fatfs.h"
#include "mini-printf.h"
#include "fram.h"
#include "shift_reg.h"
#include "PopUpMessageDLG.h"
#include "archive.h"
#include "watchdog.h"
#include "parameters.h"
#include "version.h"
#include "StartUp.h"
#include "buzzer.h"
#include "UpgradeBoardFirmwareDLG.h"
#include "UserMessage.h"
#include "TaskUpgradeBoardFirmware.h"

char DeleteFilePath[128];
char SrcFilePath[128];
char DstFilePath[128];

xTaskHandle vtskUSBHandle;

int USBState;

EventGroupHandle_t USBGroup;
xTaskHandle vtaskFileHandle;
xTaskHandle vtaskUSBHandle;

static void StopUSB(void)
{
	USB_BACK;
	USB_SW_OFF;
	USB_PWR_OFF;
	USBState = INIT_USB;
}

static void DeleteFile(void)
{
	if (FR_OK != f_unlink(DeleteFilePath))
		CreateMessage(GUI_LANG_GetText(167), DEFAULT_COLOR, DEFAULT_COLOR);
}

static void SendMessageAboutCopyStatus(int status)
{
	if (status == FR_OK)
		CreateMessage(GUI_LANG_GetText(168), DEFAULT_COLOR, DEFAULT_COLOR);
	else
		CreateMessage(GUI_LANG_GetText(169), DEFAULT_COLOR, DEFAULT_COLOR);
}

static void vtaskCopyActualArchives(void *argument)
{
	int status, ActualID;
	FILINFO fno;

	ARCHIVE_SaveBuffersToSDCard();
	ActualID = FRAM_Read(CURRENT_ARCH_ADDR);

	mini_snprintf(SrcFilePath, 22, "0:/archive/%dAD%02d.csv", GeneralSettings.DeviceID, ActualID);
	mini_snprintf(DstFilePath, 22, "1:%dAD%02d.csv", GeneralSettings.DeviceID, ActualID);
	if (f_stat(SrcFilePath, &fno) == FR_OK)
	{
		status = FREERTOS_CopyFile(SrcFilePath, DstFilePath);
		if (status != FR_OK)
		{
			CreateMessage(GUI_LANG_GetText(169), DEFAULT_COLOR, DEFAULT_COLOR);
			vTaskDelete(NULL);
		}
	}

	mini_snprintf(SrcFilePath, 22, "0:/archive/%dAT%02d.csv", GeneralSettings.DeviceID, ActualID);
	mini_snprintf(DstFilePath, 22, "1:%dAT%02d.csv", GeneralSettings.DeviceID, ActualID);
	if (f_stat(SrcFilePath, &fno) == FR_OK)
	{
		status = FREERTOS_CopyFile(SrcFilePath, DstFilePath);
		if (status != FR_OK)
		{
			CreateMessage(GUI_LANG_GetText(169), DEFAULT_COLOR, DEFAULT_COLOR);
			vTaskDelete(NULL);
		}
	}
	mini_snprintf(SrcFilePath, 22, "0:/archive/%dAE%02d.csv", GeneralSettings.DeviceID, ActualID);
	mini_snprintf(DstFilePath, 22, "1:%dAE%02d.csv", GeneralSettings.DeviceID, ActualID);
	if (f_stat(SrcFilePath, &fno) == FR_OK)
	{
		status = FREERTOS_CopyFile(SrcFilePath, DstFilePath);
		if (status != FR_OK)
		{
			CreateMessage(GUI_LANG_GetText(169), DEFAULT_COLOR, DEFAULT_COLOR);
			vTaskDelete(NULL);
		}
	}
	CreateMessage(GUI_LANG_GetText(168), DEFAULT_COLOR, DEFAULT_COLOR);

	xEventGroupSetBits(USBGroup, READY);
	vTaskDelete(NULL);
}

static void vtaskCopyArchive(void *argument)
{
	int status;
	ARCHIVE_SaveBuffersToSDCard();
	status = FREERTOS_CopyFile(SrcFilePath, DstFilePath);
	SendMessageAboutCopyStatus(status);
	xEventGroupSetBits(USBGroup, READY);
	vTaskDelete(NULL);
}

static void vtaskCopyParToUSB(void *argument)
{
	int status;
	char USBparameterFileName[15];
	char parameterFileName[22];

	mini_snprintf(USBparameterFileName, 15, "1:%02d%s.par", GeneralSettings.DeviceID,NAME);
	mini_snprintf(parameterFileName, 22, "0:config/%s.par",NAME);
	status = FATFS_CopyFile(parameterFileName, USBparameterFileName);

	SendMessageAboutCopyStatus(status);
	xEventGroupSetBits(USBGroup, READY);
	USERMESSAGE_SendBroadcast(WM_USER_REFRESH_USB_LISTVIEW);
	vTaskDelete(NULL);
}

static void vtaskCopyNewParToSD(void *argument)
{
	int status;
	char parameterFileName[22];

	if (ChcekParametersFile(SrcFilePath) == 0)
	{
		mini_snprintf(parameterFileName, 22, "0:config/%s.par",NAME);
		status = FATFS_CopyFile(SrcFilePath, parameterFileName);

		if (status == FR_OK)
		{
			FRAM_Write(NEW_PARAMETERS_ADDR, 1);
			CreateMessage(GUI_LANG_GetText(170), DEFAULT_COLOR, DEFAULT_COLOR);
			FRAM_Write(NEW_ARCHIVE_FLAG_ADDR, 1);
			ARCHIVE_SaveBuffersToSDCard();
			vTaskDelay(2000);
			HAL_NVIC_SystemReset();
		}
		else
			CreateMessage(GUI_LANG_GetText(168), DEFAULT_COLOR, DEFAULT_COLOR);
		xEventGroupSetBits(USBGroup, READY);
	}
	else
	{
		CreateMessage(GUI_LANG_GetText(175), DEFAULT_COLOR, DEFAULT_COLOR);
		xEventGroupSetBits(USBGroup, READY);
	}
	vTaskDelete(NULL);
}

int GetUSBState(void)
{
	return USBState;
}


void vtaskUSB(void *argument)
{
	EventBits_t event = 0;
	USBGroup = xEventGroupCreate();

	while (1)
	{
		if (STARTUP_WaitForBits(0x0020))
		{
			STARTUP_ClaerBits(0x0020);
			STARTUP_SetBits(0x0040);

			USB_PWR_OFF;
			USB_BACK;
			USB_SW_ON;
			USBState = INIT_USB;
			vTaskDelay(100);

			while (1)
			{
				WDFlags[4] = ALIVE;
				event = xEventGroupWaitBits(USBGroup, 0xFFFF, pdTRUE, pdFALSE, 750);

				switch (USBState)
				{
				case INIT_USB:
					switch (event)
					{
					case START:
						USB_SW_ON;
						USB_PWR_ON;
						USBState = REGISTER_USB;
						break;
					case STOP:
						StopUSB();
						break;
					}
					break;
				case REGISTER_USB:
					switch (event)
					{
					case DELETE_FILE:
						DeleteFile();
						break;
					case DEVICE_CONNECTING:
						USBState = USB_CONNECTING;
						break;
					case STOP:
						StopUSB();
						break;
					case 0:
						USB_FRONT_BACK_TOGGLE;
						vTaskDelay(200);
						break;
					}
					break;
				case USB_CONNECTING:
					switch (event)
					{
					case DEVICE_CONNECTED:
						USBState = USB_OK;
						break;
					case STOP:
						StopUSB();
						break;
					}
					break;
				case USB_OK:
					switch (event)
					{
					case COPY_PAR_TO_USB:
						USBState = USB_BUSY;
						xTaskCreate(vtaskCopyParToUSB, "vtskCopyParToUSB", 1200, NULL, ( unsigned portBASE_TYPE )0, &vtaskFileHandle);
						break;
					case COPY_PAR_TO_SD:
						USBState = USB_BUSY;
						xTaskCreate(vtaskCopyNewParToSD, "vtskCopyNewParToSD", 1200, NULL, ( unsigned portBASE_TYPE )0, &vtaskFileHandle);
						break;
					case DELETE_FILE:
						DeleteFile();
						break;
					case COPY_FILE:
						USBState = USB_BUSY;
						xTaskCreate(vtaskCopyArchive, "vtskCopyArchive", 1200, NULL, ( unsigned portBASE_TYPE )0, &vtaskFileHandle);
						break;
					case DEVICE_DISCONNECTED:
						USBState = REGISTER_USB;
						USB_BACK;
						BUZZER_Beep();
						break;
					case COPY_ACTUAL_ARCHIVES:
						USBState = USB_BUSY;
						xTaskCreate(vtaskCopyActualArchives, "vtskCopyActualArchives", 1200, NULL, ( unsigned portBASE_TYPE )0, &vtaskFileHandle);
						break;
					case START_TASK_FIRMWARE_UPGRADE:
						USBState = USB_BUSY;
						CreateUpgradeBoardFirmwareTask(GetBoardNumber());
						sendUSBEvent(READY);
						break;
					case STOP:
						StopUSB();
						break;
					}
					break;
				case USB_BUSY:
					switch (event)
					{
					case DEVICE_DISCONNECTED:
						USBState = REGISTER_USB;
						USB_BACK;
						BUZZER_Beep();
						break;
					case READY:
						USBState = USB_OK;
						break;
					case STOP:
						StopUSB();
						break;
					}
					break;
				}
			}
		}
		else
			continue;
	}
}

void CreateUSBTask(void)
{
	xTaskCreate(vtaskUSB, (char*) "vtskUSB", 1800, NULL, (unsigned portBASE_TYPE ) 4, &vtskUSBHandle);
}

void sendUSBEvent(uint32_t event)
{
	xEventGroupSetBits(USBGroup, event);
}


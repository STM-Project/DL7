/*
 * StartUpTask.c
 *
 *  Created on: 16.05.2017
 *      Author: Tomaszs
 */

#include "StartUpTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "NSMAC.h"
#include "ChannelsSynchronization.h"
#include "passwords.h"
#include "parameters.h"
#include "rtc.h"
#include "skins.h"
#include "StartUp.h"
#include "GUITasks.h"
#include "USBtask.h"
#include "ExpansionCardsTask.h"
#include "ChannelsSynchronization.h"
#include "ChannelsTask.h"
#include "archive.h"
#include "usb_host.h"
#include "modbus.h"
#include "EthernetTask.h"
#include "alarms.h"
#include "cycleEvents.h"
#include "watchdog.h"

void CreateStartUpTasks(void)
{
	InitSNMAC();

	InitChannelsMutex();
	PASSWORDS_Init();
	SKINS_InitGraphics();
	LoadParametersFile();
  	DST_Initialize(GeneralSettings.DSTMode);
	InitLanguage();
	InitializeModules();

	STARTUP_Init();
	STARTUP_SetBits(0x0000);

	CreateGUITask();
	CreateBSPTask();
	CreateExpansionCardsTask();
	CreateChannelsTask();
	CreateArchiveTask();
	CreateUSBTask();
	CreateEthTask();
	CreateModbusRTUTask();
	CreateAlarmsTask();
	CreateCycleEventsTask();
	//CreateWatchdogTask();

	STARTUP_SetBits(0x0001);
}


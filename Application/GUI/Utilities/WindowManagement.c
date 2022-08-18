/*
 * WindowManagement.c
 *
 *  Created on: 15.05.2017
 *      Author: Tomaszs
 */

#include "WindowManagement.h"
#include "fram.h"

#include "InfoDLG.h"
#include "UserTablesDLG.h"
#include "UserTrendsDLG.h"
#include "ArchiveDLG.h"
#include "SettingsDLG.h"
#include "SingleChannelDLG.h"
#include "AlarmsDLG.h"

int WindowsState;
int WindowInstance;
WM_HWIN hActualWindow;

WM_HWIN ChangeWindow(WM_HWIN ActiveWindow, int NewWindowState)
{
	WM_HWIN NewWindow;
	if (ActiveWindow)
		WM_DeleteWindow(ActiveWindow);

	switch (NewWindowState)
	{
	case INFO:
		NewWindow = CreateInfo();
		WindowsState = INFO;
		FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
		break;
	case USERVALUES:
		NewWindow = CreateUserTables();
		WindowsState = USERVALUES;
		FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
		break;
	case USERTRENDS:
		NewWindow = CreateUserTrends();
		WindowsState = USERTRENDS;
		FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
		break;
	case ARCHIVE:
		NewWindow = CreateArchive();
		WindowsState = ARCHIVE;
		FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
		break;
	case SETTINGS:
		NewWindow = CreateSettings();
		WindowsState = SETTINGS;
		break;
	case SINGLE_VALUE:
		NewWindow = CreateSingleChannelWindow(WindowInstance);
		WindowsState = SINGLE_VALUE;
		FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
		FRAM_Write(WINDOW_INSTANCE_ADDR, (uint8_t) WindowInstance);
		break;
	case ALARMS:
		NewWindow = CreateAlarms();
		WindowsState = ALARMS;
		FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
		break;

	default:
		NewWindow = ActiveWindow;
		break;
	}

	return NewWindow;
}

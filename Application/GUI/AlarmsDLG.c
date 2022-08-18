/*
 * AlarmsDLG.c
 *
 *  Created on: 30 lip 2015
 *      Author: Tomaszs
 */
#include <alarms.h>
#include <channels.h>
#include "AlarmsDLG.h"
#include "draw.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

#define ID_WIN_ALARM 						(GUI_ID_USER + 0x100)
#define ID_TEXT_CHANNEL_1_20 		(GUI_ID_USER + 0x101)
#define ID_TEXT_CHANNEL_21_40 	(GUI_ID_USER + 0x102)
#define ID_TEXT_CHANNEL_41_60 	(GUI_ID_USER + 0x103)
#define ID_TEXT_CHANNEL_61_80 	(GUI_ID_USER + 0x104)
#define ID_TEXT_CHANNEL_81_100 	(GUI_ID_USER + 0x105)
#define ID_BUTTON_ACK_ALARMS 		(GUI_ID_USER + 0x106)

#define START_X 250
#define START_Y 5
#define SIZE_X	18
#define SIZE_Y	25

ALARM temp_alarms[2] __attribute__ ((section(".sdram")));

static void GetAlarms(ALARM * alarm, int index);

static int AlarmBlinkState = 0;

void DrawSingleAlarmCell(int xStart, int yStart, int xSize, int ySize, int pulseState, ALARM *alarm)
{

	int AlarmState = 0;
	if (alarm->type == 0)
		AlarmState = 0;
	else if (!alarm->state && alarm->AckAlarm)
		AlarmState = 1;
	else if (alarm->state && alarm->AckAlarm)
		AlarmState = 2;
	else if (!alarm->AckAlarm && alarm->mode == 0)
		AlarmState = 3;

	switch (AlarmState)
	{
	case 0:
		DRAW_Rect(xStart, yStart, xSize, ySize, 1, GUI_LIGHTGRAY, GUI_BLACK);
		break;
	case 1:
		DRAW_Rect(xStart, yStart, xSize, ySize, 1, GUI_WHITE, GUI_BLACK);
		break;
	case 2:
		if(alarm->color == 0)
			DRAW_Rect(xStart, yStart, xSize, ySize, 1, GUI_BLUE, GUI_BLACK);
		else
			DRAW_Rect(xStart, yStart, xSize, ySize, 1, ALARMS_GetColor(alarm->color), GUI_BLACK);
		break;
	case 3:
		if (pulseState)
		{
			if (alarm->color == 0)
				DRAW_Rect(xStart, yStart, xSize, ySize, 1, GUI_BLUE, GUI_BLACK);
			else
				DRAW_Rect(xStart, yStart, xSize, ySize, 1, ALARMS_GetColor(alarm->color), GUI_BLACK);
		}
		else
			DRAW_Rect(xStart, yStart, xSize, ySize, 1, GUI_WHITE, GUI_BLACK);
		break;
	}
}

void DrawAlarmTable(int pulseState)
{
	int i, j, xoffset;
	ALARM tempAlarms[2];

	for (i = 0; i < 5; i++)
	{
		DRAW_Text("AL1", START_X - 25, START_Y + i * (2 * SIZE_Y + 10), GUI_BLUE, GUI_TRANSPARENT,
				&GUI_FontLato30);
		DRAW_Text("AL2", START_X - 25, START_Y + SIZE_Y + i * (2 * SIZE_Y + 10), GUI_BLUE, GUI_TRANSPARENT,
				&GUI_FontLato30);

		xoffset = 0;
		for (j = 0; j < 20; j++)
		{
			GetAlarms(tempAlarms, i * 20 + j);

			DrawSingleAlarmCell((START_X + xoffset) + j * (SIZE_X + 5), START_Y + i * (2 * SIZE_Y + 10),
					SIZE_X, SIZE_Y, pulseState, &tempAlarms[0]);

			DrawSingleAlarmCell((START_X + xoffset) + j * (SIZE_X + 5), START_Y + SIZE_Y + i * (2 * SIZE_Y + 10),
					SIZE_X, SIZE_Y, pulseState, &tempAlarms[1]);
			if ((j % 5) == 4)
				xoffset = xoffset + 5;
		}
	}
}

static const GUI_WIDGET_CREATE_INFO _aAlarmsDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "Alarmy", ID_WIN_ALARM, 0, 60, 740, 420, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "Channels 1-20", ID_TEXT_CHANNEL_1_20, 6, 15, 360, 30, 0, 0x64, 0 },
				{ TEXT_CreateIndirect, "Channels 21-40", ID_TEXT_CHANNEL_21_40, 6, 75, 360, 30, 0, 0x64, 0 },
				{ TEXT_CreateIndirect, "Channels 41-60", ID_TEXT_CHANNEL_41_60, 6, 135, 360, 30, 0, 0x64, 0 },
				{ TEXT_CreateIndirect, "Channels 61-80", ID_TEXT_CHANNEL_61_80, 6, 195, 360, 30, 0, 0x64, 0 },
				{ TEXT_CreateIndirect, "Channels 81-100", ID_TEXT_CHANNEL_81_100, 6, 255, 360, 30, 0, 0x64, 0 },
				{ BUTTON_CreateIndirect, "ACK_ALARM", ID_BUTTON_ACK_ALARMS, 478, 360, 260, 58, 0, 0x0, 0 },
		};


static void _cbAlarmsDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	int NCode;
	int Id;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		WM_SelectWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_1_20);
		mini_snprintf(GUITextBuffer, 30, "%s 1-20", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_21_40);
		mini_snprintf(GUITextBuffer, 30, "%s 21-40", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_41_60);
		mini_snprintf(GUITextBuffer, 30, "%s 41-60", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_61_80);
		mini_snprintf(GUITextBuffer, 30, "%s 61-80", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_81_100);
		mini_snprintf(GUITextBuffer, 30, "%s 81-100", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ACK_ALARMS);
		BUTTON_SetText(hItem, GUI_LANG_GetText(173));
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
			WM_HideWindow(hItem);

		WM_CreateTimer(pMsg->hWin, 1, 500, 0);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_ACK_ALARMS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
					ALARMS_AckAll();
				else
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				break;
			}
			break;
		}
		break;
	case WM_TIMER:

		if (AlarmBlinkState)
			AlarmBlinkState = 0;
		else
			AlarmBlinkState = 1;

		WM_InvalidateWindow(pMsg->hWin);
		WM_RestartTimer(pMsg->Data.v, 1000);
		break;

	case WM_POST_PAINT:
		DrawAlarmTable(AlarmBlinkState);
		break;

	case WM_USER_LOGIN:
		if (pMsg->Data.v < USER_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ACK_ALARMS);
			WM_HideWindow(hItem);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateAlarms(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aAlarmsDialogCreate, GUI_COUNTOF(_aAlarmsDialogCreate), _cbAlarmsDialog, WM_HBKWIN, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(25));
	return hWin;
}

static void GetAlarms(ALARM * alarm, int index)
{
	*alarm = Channels[index].alarm[0];
	alarm++;
	*alarm = Channels[index].alarm[1];
}

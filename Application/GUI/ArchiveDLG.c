#include <channels.h>
#include "ArchiveDLG.h"

#include <stdlib.h>
#include <string.h>

#include "archive.h"
#include "parameters.h"
#include "draw.h"
#include "skins.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "fram.h"
#include "passwords.h"
#include "buzzer.h"
#include "fatfs.h"
#include "mini-printf.h"
#include "archiveRead.h"
#include "ArchiveReadMenuDLG.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

#define ID_WIN_ARCH 									(GUI_ID_USER + 100)
#define ID_BUTTON_START_STOP_ARCHIVE 	(GUI_ID_USER + 101)
#define ID_TEXT_INTERVAL_DESC 				(GUI_ID_USER + 102)
#define ID_PROGBAR_DISKSPACE 					(GUI_ID_USER + 103)
#define ID_TEXT_TOT_INTERVAL_DESC 		(GUI_ID_USER + 104)
#define ID_TEXT_INTERVAL_1 						(GUI_ID_USER + 105)
#define ID_TEXT_INTERVAL_2 						(GUI_ID_USER + 106)
#define ID_TEXT_DISKSPACE 						(GUI_ID_USER + 107)
#define ID_TEXT_TOT_INTERVAL 					(GUI_ID_USER + 108)
#define ID_MULTIPAGE_ARCHIVE_DATA 		(GUI_ID_USER + 109)
#define ID_BUTTON_NEW_ARCHIVE 				(GUI_ID_USER + 110)
#define ID_TEXT_ARCHIVE_ID_DESC 			(GUI_ID_USER + 111)
#define ID_TEXT_ARCHIVE_ID 						(GUI_ID_USER + 112)

#define ID_BUTTON_ARCHIVE_READ				(GUI_ID_USER + 113)

#define ID_WIN_VAL 										(GUI_ID_USER + 114)
#define ID_TEXT_VAL_CHANNELS_1_20 		(GUI_ID_USER + 115)
#define ID_TEXT_VAL_CHANNELS_21_40 		(GUI_ID_USER + 116)
#define ID_TEXT_VAL_CHANNELS_41_60 		(GUI_ID_USER + 117)
#define ID_TEXT_VAL_CHANNELS_61_80 		(GUI_ID_USER + 118)
#define ID_TEXT_VAL_CHANNELS_81_100 	(GUI_ID_USER + 119)

#define ID_WIN_TOT 										(GUI_ID_USER + 120)
#define ID_TEXT_TOT_CHANNELS_1_20 		(GUI_ID_USER + 121)
#define ID_TEXT_TOT_CHANNELS_21_40 		(GUI_ID_USER + 122)
#define ID_TEXT_TOT_CHANNELS_41_60 		(GUI_ID_USER + 123)
#define ID_TEXT_TOT_CHANNELS_61_80 		(GUI_ID_USER + 124)
#define ID_TEXT_TOT_CHANNELS_81_100 	(GUI_ID_USER + 125)

extern const char ArchiveFrequencyDescription[15][7];
extern const char TotalizerArchiveFrequencyDescription[10][7];

WM_HWIN CreateArchiveValuesWindow(WM_HWIN hParent);
WM_HWIN CreateArchiveTotalizersWindow(WM_HWIN hParent);

uint8_t ArchiveState __attribute__ ((section(".sdram")));

static WM_HWIN hArchiveReadMenuWin = 0;

static const GUI_WIDGET_CREATE_INFO _aArchiveDialogCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WIN_ARCH, 0, 60, 740, 420, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_START_STOP_ARCHIVE, 2, 360, 200, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_NEW_ARCHIVE, 204, 360, 200, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_ARCHIVE_READ, 498, 360, 240, 58, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_INTERVAL_DESC, 10, 40, 400, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_INTERVAL_1, 420, 40, 140, 30, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_INTERVAL_2, 570, 40, 140, 30, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_INTERVAL, 400, 75, 300, 30, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_INTERVAL_DESC, 10, 75, 460, 30, 0, 0x64, 0 },
{ PROGBAR_CreateIndirect, "", ID_PROGBAR_DISKSPACE, 400, 110, 332, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DISKSPACE, 10, 110, 400, 30, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_ARCHIVE_DATA, 4, 150, 730, 210, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ARCHIVE_ID_DESC, 10, 5, 310, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ARCHIVE_ID, 400, 5, 300, 30, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aArchiveValuesWindowCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WIN_VAL, 0, 0, 732, 175, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_VAL_CHANNELS_1_20, 6, 10, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_VAL_CHANNELS_21_40, 6, 40, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_VAL_CHANNELS_41_60, 6, 70, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_VAL_CHANNELS_61_80, 6, 100, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_VAL_CHANNELS_81_100, 6, 130, 360, 30, 0, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aArchiveTotalizers1WindowCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WIN_TOT, 0, 0, 732, 175, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_CHANNELS_1_20, 6, 10, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_CHANNELS_21_40, 6, 40, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_CHANNELS_41_60, 6, 70, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_CHANNELS_61_80, 6, 100, 360, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOT_CHANNELS_81_100, 6, 130, 360, 30, 0, 0x64, 0 }, };

static void _cbArchiveDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	int NCode;
	int Id;
	int ArchID = 0;
	int ProgBarValue = 0;
	char GUITextBuffer[40];

	uint8_t accesLevel = 0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hArchiveReadMenuWin = 0;
		ArchiveState = ARCHIVE_GetState();

		hItem = pMsg->hWin;
		WM_SelectWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_START_STOP_ARCHIVE);
		BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, 0x0000c000);
		BUTTON_SetTextColor(hItem, BUTTON_CI_PRESSED, 0x0000c000);
		if (ArchiveState == 1)
			BUTTON_SetText(hItem, "STOP");
		else
			BUTTON_SetText(hItem, "START");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_NEW_ARCHIVE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(90));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ARCHIVE_READ);
		BUTTON_SetText(hItem, GUI_LANG_GetText(258));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(59));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_INTERVAL_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(60));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_1);
		TEXT_SetText(hItem, ArchiveFrequencyDescription[ArchiveSettings.ArchivizationFrequency1]);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_2);
		TEXT_SetText(hItem, ArchiveFrequencyDescription[ArchiveSettings.ArchivizationFrequency2]);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DISKSPACE);
		TEXT_SetText(hItem, GUI_LANG_GetText(14));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_INTERVAL);
		TEXT_SetText(hItem, TotalizerArchiveFrequencyDescription[ArchiveSettings.TotalizerArchivizationFrequency]);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE_ID_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(162));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE_ID);
		ArchID = FRAM_Read(CURRENT_ARCH_ADDR);
		mini_snprintf(GUITextBuffer, 40, "%02d (%s)", ArchID, GUI_LANG_GetText(43 + ArchiveSettings.ArchiveMaxSize));
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_DISKSPACE);
		PROGBAR_SetFont(hItem, &GUI_FontLato30);
		PROGBAR_SetBarColor(hItem, 0, GUI_BLUE);
		PROGBAR_SetBarColor(hItem, 1, GUI_WHITE);
		PROGBAR_SetTextColor(hItem, 0, GUI_WHITE);
		PROGBAR_SetTextColor(hItem, 1, GUI_BLUE);
		PROGBAR_SetMinMax(hItem, 0, 100);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_ARCHIVE_DATA);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		MULTIPAGE_AddPage(hItem, CreateArchiveValuesWindow(hItem), GUI_LANG_GetText(61));
		MULTIPAGE_AddPage(hItem, CreateArchiveTotalizersWindow(hItem), GUI_LANG_GetText(24));
		MULTIPAGE_SetTabWidth(hItem, 364, 0);
		MULTIPAGE_SetTabWidth(hItem, 364, 1);
		MULTIPAGE_SelectPage(hItem, 0);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_START_STOP_ARCHIVE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				accesLevel = PASSWORDS_GetCurrentLevel();
				if (ADMIN_LEVEL <= accesLevel || (USER_LEVEL == accesLevel && 1 == ArchiveSettings.canUserStopArchive) )
				{
					if (ArchiveState == 1)
					{
						BUTTON_SetText(pMsg->hWinSrc, "START");
						ARCHIVE_SafeStop();
						ARCHIVE_SendEvent("ARCH:STOP");
						ArchiveState = 0;
					}
					else
					{
						BUTTON_SetText(pMsg->hWinSrc, "STOP");
						ARCHIVE_SafeStart();

						ARCHIVE_SendEvent("ARCH:START");
						ArchiveState = 1;
					}
					USERMESSAGE_Archive(ArchiveState);
				}
				else
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);

				break;
			}
			break;
		case ID_BUTTON_NEW_ARCHIVE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL && ArchiveState == 0)
				{
					ARCHIVE_CreateNewArchives();
					GUI_Delay(500);
					ArchID = FRAM_Read(CURRENT_ARCH_ADDR);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE_ID);
					mini_snprintf(GUITextBuffer, 40, "%02d (%s)", ArchID, GUI_LANG_GetText(43 + ArchiveSettings.ArchiveMaxSize));
					TEXT_SetText(hItem, GUITextBuffer);
				}
				else if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL && ArchiveState == 1)
				{
					mini_snprintf(GUITextBuffer, 100, "%s\n%s", GUI_LANG_GetText(165), GUI_LANG_GetText(166));
					CreateMessage(GUITextBuffer, GUI_YELLOW, GUI_BLACK);
				}

				else
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);

				break;
			}
			break;
		case ID_MULTIPAGE_ARCHIVE_DATA:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_ARCHIVE_READ:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
				{
					hArchiveReadMenuWin = CreateArchiveReadMenu(pMsg->hWin);
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				}
				break;
			}
			break;
		}
		break;
	case WM_TIMER:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_DISKSPACE);
		ProgBarValue = FATFS_GetFreeSpaceOfSD();

		PROGBAR_SetValue(hItem, ProgBarValue);
		if (ProgBarValue < 70)
		{
			PROGBAR_SetBarColor(hItem, 0, GUI_BLUE);
			PROGBAR_SetBarColor(hItem, 1, GUI_WHITE);
			PROGBAR_SetTextColor(hItem, 0, GUI_WHITE);
			PROGBAR_SetTextColor(hItem, 1, GUI_BLUE);
		}
		else if (ProgBarValue >= 70 && ProgBarValue < 90)
		{
			PROGBAR_SetBarColor(hItem, 0, GUI_YELLOW);
			PROGBAR_SetBarColor(hItem, 1, GUI_WHITE);
			PROGBAR_SetTextColor(hItem, 0, GUI_BLACK);
			PROGBAR_SetTextColor(hItem, 1, GUI_YELLOW);
		}
		else if (ProgBarValue >= 90)
		{
			PROGBAR_SetBarColor(hItem, 0, GUI_RED);
			PROGBAR_SetBarColor(hItem, 1, GUI_WHITE);
			PROGBAR_SetTextColor(hItem, 0, GUI_WHITE);
			PROGBAR_SetTextColor(hItem, 1, GUI_RED);
		}
		WM_RestartTimer(pMsg->Data.v, 600000);

		break;
	case WM_USER_NEWARCHIVEID:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE_ID);
		mini_snprintf(GUITextBuffer, 40, "%02d (%s)", pMsg->Data.v, GUI_LANG_GetText(43 + ArchiveSettings.ArchiveMaxSize));
		TEXT_SetText(hItem, GUITextBuffer);
		break;
	case WM_USER_ALARM:

		if (ARCHIVE_GetFrequencyState() == 1)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_1);
			TEXT_SetTextColor(hItem, GUI_GRAY);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_2);
			TEXT_SetTextColor(hItem, SKINS_GetTextColor());
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_1);
			TEXT_SetTextColor(hItem, SKINS_GetTextColor());

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INTERVAL_2);
			TEXT_SetTextColor(hItem, GUI_GRAY);
		}

		break;
	case WM_USER_LOGIN:
		if(PASSWORDS_GetCurrentLevel() < USER_LEVEL)
		{
			if (hArchiveReadMenuWin)
			{
				WM_DeleteWindow(hArchiveReadMenuWin);
				hArchiveReadMenuWin = 0;
			}
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(399, 109, 333, 31, 0, 1, GUI_BLUE);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbArchiveValuesDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int i, j, xoffset;
	char GUITextBuffer[30];
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		WM_SelectWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VAL_CHANNELS_1_20);
		mini_snprintf(GUITextBuffer, 30, "%s 1-20", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VAL_CHANNELS_21_40);
		mini_snprintf(GUITextBuffer, 30, "%s 21-40", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VAL_CHANNELS_41_60);
		mini_snprintf(GUITextBuffer, 30, "%s 41-60", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VAL_CHANNELS_61_80);
		mini_snprintf(GUITextBuffer, 30, "%s 61-80", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VAL_CHANNELS_81_100);
		mini_snprintf(GUITextBuffer, 30, "%s 81-100", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		break;

	case WM_POST_PAINT:
		for (i = 0; i < 5; i++)
		{
			xoffset = 0;
			for (j = 0; j < 20; j++)
			{
				if (bkChannels[i * 20 + j].ToArchive == 1)
					DRAW_Rect((210 + xoffset) + j * 25, 15 + i * 30, 20, 20, 1, 0x0000c000, GUI_BLACK);
				else
					DRAW_Rect((210 + xoffset) + j * 25, 15 + i * 30, 20, 20, 1,
					GUI_LIGHTGRAY, GUI_BLACK);

				if ((j % 5) == 4)
					xoffset = xoffset + 5;
			}
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbArchiveTotalizersDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int i, j, xoffset;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		WM_SelectWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_CHANNELS_1_20);
		mini_snprintf(GUITextBuffer, 30, "%s 1-20", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_CHANNELS_21_40);
		mini_snprintf(GUITextBuffer, 30, "%s 21-40", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_CHANNELS_41_60);
		mini_snprintf(GUITextBuffer, 30, "%s 41-60", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_CHANNELS_61_80);
		mini_snprintf(GUITextBuffer, 30, "%s 61-80", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_CHANNELS_81_100);
		mini_snprintf(GUITextBuffer, 30, "%s 81-100", GUI_LANG_GetText(62));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		break;

	case WM_POST_PAINT:

		for (i = 0; i < 5; i++)
		{
			xoffset = 0;
			for (j = 0; j < 20; j++)
			{
				if (bkChannels[i * 20 + j].Tot1.ToArchive == 1)
					DRAW_Rect((210 + xoffset) + j * 25, 15 + i * 30, 20, 10, 1, 0x0000c000, GUI_BLACK);
				else
					DRAW_Rect((210 + xoffset) + j * 25, 15 + i * 30, 20, 10, 1,
					GUI_LIGHTGRAY, GUI_BLACK);

				if (bkChannels[i * 20 + j].Tot2.ToArchive == 1)
					DRAW_Rect((210 + xoffset) + j * 25, 25 + i * 30, 20, 10, 1, 0x0000c000, GUI_BLACK);
				else
					DRAW_Rect((210 + xoffset) + j * 25, 25 + i * 30, 20, 10, 1,
					GUI_LIGHTGRAY, GUI_BLACK);

				if ((j % 5) == 4)
					xoffset = xoffset + 5;
			}
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateArchive(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aArchiveDialogCreate, GUI_COUNTOF(_aArchiveDialogCreate), _cbArchiveDialog, WM_HBKWIN, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(4));
	WM_CreateTimer(hWin, 0, 1, 0);
	return hWin;
}

WM_HWIN CreateArchiveValuesWindow(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aArchiveValuesWindowCreate, GUI_COUNTOF(_aArchiveValuesWindowCreate), _cbArchiveValuesDialog, hParent, 0, 0);
	return hWin;
}

WM_HWIN CreateArchiveTotalizersWindow(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aArchiveTotalizers1WindowCreate, GUI_COUNTOF(_aArchiveTotalizers1WindowCreate), _cbArchiveTotalizersDialog,
			hParent, 0, 0);

	return hWin;
}

#include "SetUserScreensDLG.h"
#include <string.h>
#include <stdlib.h>
#include "parameters.h"
#include "draw.h"
#include "dtos.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "mini-printf.h"
#include "EDIT_double.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_SET_USER_SCREENS 			(GUI_ID_USER + 0x201)
#define ID_MULTIPAGE_USER_SCREENS  			(GUI_ID_USER + 0x202)
#define ID_BUTTON_OK    								(GUI_ID_USER + 0x203)
#define ID_BUTTON_CANCEL     						(GUI_ID_USER + 0x204)

#define ID_WINDOW_SET_USER_TABLES 			(GUI_ID_USER + 0x205)
#define ID_DROPDOWN_SELECT_TABLE				(GUI_ID_USER + 0x206)
#define ID_TEXT_TABLE_DESC     					(GUI_ID_USER + 0x207)
#define ID_EDIT_TABLE_DESC     					(GUI_ID_USER + 0x208)
#define ID_BUTTON_SET_CELL_1 						(GUI_ID_USER + 0x209)
#define ID_BUTTON_SET_CELL_2 						(GUI_ID_USER + 0x20A)
#define ID_BUTTON_SET_CELL_3 						(GUI_ID_USER + 0x20B)
#define ID_BUTTON_SET_CELL_4 						(GUI_ID_USER + 0x20C)
#define ID_BUTTON_SET_CELL_5 						(GUI_ID_USER + 0x20D)
#define ID_BUTTON_SET_CELL_6 						(GUI_ID_USER + 0x20E)
#define ID_BUTTON_SET_CELL_7 						(GUI_ID_USER + 0x20F)
#define ID_BUTTON_SET_CELL_8 						(GUI_ID_USER + 0x210)
#define ID_BUTTON_SET_CELL_9 						(GUI_ID_USER + 0x211)
#define ID_BUTTON_SET_CELL_10 					(GUI_ID_USER + 0x212)
#define ID_BUTTON_SET_CELL_11 					(GUI_ID_USER + 0x213)
#define ID_BUTTON_SET_CELL_12 					(GUI_ID_USER + 0x214)
#define ID_BUTTON_SET_CELL_13 					(GUI_ID_USER + 0x215)
#define ID_BUTTON_SET_CELL_14 					(GUI_ID_USER + 0x216)
#define ID_BUTTON_SET_CELL_15 					(GUI_ID_USER + 0x217)

#define ID_WINDOW_SET_USER_TRENDS 			(GUI_ID_USER + 0x219)
#define ID_DROPDOWN_SELECT_TREND 				(GUI_ID_USER + 0x21A)
#define ID_TEXT_TREND_DESC     					(GUI_ID_USER + 0x21B)
#define ID_EDIT_TREND_DESC     					(GUI_ID_USER + 0x21C)
#define ID_DROPDOWN_SOURCE_LINE_1 			(GUI_ID_USER + 0x21D)
#define ID_DROPDOWN_SOURCE_LINE_2 			(GUI_ID_USER + 0x21E)
#define ID_DROPDOWN_SOURCE_LINE_3 			(GUI_ID_USER + 0x21F)
#define ID_DROPDOWN_SOURCE_LINE_4 			(GUI_ID_USER + 0x220)
#define ID_DROPDOWN_SOURCE_LINE_5 			(GUI_ID_USER + 0x221)
#define ID_DROPDOWN_SOURCE_LINE_6 			(GUI_ID_USER + 0x222)
#define ID_TEXT_TREND_MAX     					(GUI_ID_USER + 0x223)
#define ID_EDIT_TREND_MAX     					(GUI_ID_USER + 0x224)
#define ID_TEXT_TREND_MIN     					(GUI_ID_USER + 0x225)
#define ID_EDIT_TREND_MIN								(GUI_ID_USER + 0x226)

#define ID_FRAMEWIN_SET_TABLE_CELL			(GUI_ID_USER + 0x227)
#define ID_DROPDOWN_SOURCE_CHANNEL			(GUI_ID_USER + 0x228)
#define ID_DROPDOWN_SOURCE_TYPE					(GUI_ID_USER + 0x229)
#define ID_BUTTON_CONFIRM								(GUI_ID_USER + 0x22A)
#define ID_BUTTON_ANNUL									(GUI_ID_USER + 0x22B)

static WM_HWIN CreateTableSet(WM_HWIN hParent);
static WM_HWIN CreateTrendsSet(WM_HWIN hParent);
static WM_HWIN CreateSetUserData(WM_HWIN hParent, short selection);

USER_TABLES_SETTINGS tempUserTablesSettings __attribute__ ((section(".sdram")));
USER_TREND tempUserTrendsSettings[6];

char GUILocalStringBuffer[5][10] __attribute__ ((section(".sdram")));

uint8_t cellSelection = 255;
WM_HWIN hButtonToModify;

static const GUI_WIDGET_CREATE_INFO _aSetUserTablesDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_USER_TABLES, 0, 0, 730, 315, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SELECT_TABLE, 10, 10, 210, 180, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_TABLE_DESC, 240, 10, 135, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x0, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_TABLE_DESC, 390, 10, 320, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 20 },

				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_1, 10, 60, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_2, 10, 110, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_3, 10, 160, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_4, 10, 210, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_5, 10, 260, 235, 45, 0, 0x0, 0 },

				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_6, 250, 60, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_7, 250, 110, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_8, 250, 160, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_9, 250, 210, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_10, 250, 260, 235, 45, 0, 0x0, 0 },

				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_11, 490, 60, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_12, 490, 110, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_13, 490, 160, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_14, 490, 210, 235, 45, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CELL_15, 490, 260, 235, 45, 0, 0x0, 0 },

		};

static const GUI_WIDGET_CREATE_INFO _aSetUserTrendsDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_USER_TRENDS, 0, 0, 730, 315, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SELECT_TREND, 10, 10, 210, 180, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_DESC, 240, 10, 135, 35, EDIT_CF_RIGHT | EDIT_CF_VCENTER, 0x0, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_TREND_DESC, 390, 10, 320, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 20 },

				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_MAX, 90, 60, 280, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x0, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_TREND_MAX, 390, 60, 190, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },

				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_MIN, 90, 110, 280, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x0, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_TREND_MIN, 390, 110, 190, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },

				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_LINE_1, 10, 160, 225, 150, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_LINE_2, 10, 210, 225, 150, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_LINE_3, 10, 260, 225, 150, 0, 0x0, 0 },

				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_LINE_4, 390, 160, 225, 150, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_LINE_5, 390, 210, 225, 150, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_LINE_6, 390, 260, 225, 150, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetUserScreensDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_USER_SCREENS, 0, 0, 740, 420, 0, 0x0, 0 },
				{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_USER_SCREENS, 5, 2, 730, 350, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 496, 360, 120, 58, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 618, 360, 120, 58, 0, 0x0, 0 },
		};

static const GUI_WIDGET_CREATE_INFO _aSetUserDataDialogCreate[] =
		{
				{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_SET_TABLE_CELL, 175, 86, 380, 192, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_CHANNEL, 2, 2, 210, 90, 0, 0x0, 0 },
				{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SOURCE_TYPE, 214, 2, 160, 90, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_CONFIRM, 2, 128, 185, 58, 2, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_ANNUL, 189, 128, 185, 58, 0, 0x0, 0 },
		};

static void _cbSetUserTableDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int i, j;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TABLE_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(23));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for (j = 1; j <= 6; j++)
		{
			mini_snprintf(GUITextBuffer, 30, "%s %d", GUI_LANG_GetText(63), j);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		DROPDOWN_SetSel(hItem, 0);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TABLE_DESC);
		EDIT_SetText(hItem, tempUserTablesSettings.TablesTitles[0]);

		for (i = 0; i < 15; i++)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_CELL_1 + i);
			BUTTON_SetSkinClassic(hItem);
			BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_LIGHTGRAY);
			BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLACK);
			BUTTON_SetFrameColor(hItem, GUI_GRAY);

			if (tempUserTablesSettings.TablesData[i + 15 * DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE))] == -1)
			{
				BUTTON_SetText(hItem, GUI_LANG_GetText(27));
			}
			else
			{
				mini_snprintf(GUITextBuffer, 30, "%s %d: %s",
						GUI_LANG_GetText(6),
						(tempUserTablesSettings.TablesData[i + 15 * DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE))]) % 100
								+ 1,
						GUILocalStringBuffer[(tempUserTablesSettings.TablesData[i
								+ 15 * DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE))])
								/ 100]);
				BUTTON_SetText(hItem, GUITextBuffer);
			}
		}
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (NCode)
		{
		case WM_NOTIFICATION_RELEASED:
			BUZZER_Beep();

			if (Id == ID_EDIT_TABLE_DESC)
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 20);

			if (Id == ID_DROPDOWN_SELECT_TABLE)
				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_TABLE_DESC),
						tempUserTablesSettings.TablesTitles[DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE))], 20);

			if (Id <= ID_BUTTON_SET_CELL_15 && Id >= ID_BUTTON_SET_CELL_1)
			{
				hButtonToModify = pMsg->hWinSrc;
				cellSelection = (Id - ID_BUTTON_SET_CELL_1)
						+ 15 * DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE));
				CreateSetUserData(pMsg->hWin, tempUserTablesSettings.TablesData[cellSelection]);
			}
			break;
		case WM_NOTIFICATION_SEL_CHANGED:
			if (Id == ID_DROPDOWN_SELECT_TABLE)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TABLE_DESC);
				EDIT_SetText(hItem, tempUserTablesSettings.TablesTitles[DROPDOWN_GetSel(pMsg->hWinSrc)]);

				for (i = 0; i < 15; i++)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_CELL_1 + i);
					if (tempUserTablesSettings.TablesData[i + 15 * DROPDOWN_GetSel(pMsg->hWinSrc)] == -1)
						BUTTON_SetText(hItem, GUI_LANG_GetText(27));
					else
					{
						mini_snprintf(GUITextBuffer, 30, "%s %d: %s",
								GUI_LANG_GetText(6),
								(tempUserTablesSettings.TablesData[i + 15 * DROPDOWN_GetSel(pMsg->hWinSrc)]) % 100 + 1,
								GUILocalStringBuffer[(tempUserTablesSettings.TablesData[i + 15 * DROPDOWN_GetSel(pMsg->hWinSrc)] + 1)
								/ 100]);
						BUTTON_SetText(hItem, GUITextBuffer);
					}
				}
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(385, 8, 335, 39, 4, 2, GUI_BLUE);
		break;

	case WM_DELETE:
		EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_TABLE_DESC),
				tempUserTablesSettings.TablesTitles[DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE))], 41);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetUserTrendsDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int i, j, Selection;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(23));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_DisableWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TREND);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for (j = 1; j <= 6; j++)
		{
			mini_snprintf(GUITextBuffer, 30, "%s %d", GUI_LANG_GetText(64), j);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_DESC);
		EDIT_SetText(hItem, tempUserTrendsSettings[0].description);


		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MAX);
		TEXT_SetText(hItem, GUI_LANG_GetText(35));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_DisableWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MAX);
		EDIT_SetFloat(hItem, tempUserTrendsSettings[0].max , 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MIN);
		TEXT_SetText(hItem, GUI_LANG_GetText(65));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_DisableWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MIN);
		EDIT_SetFloat(hItem, tempUserTrendsSettings[0].min , 4);

		for (i = 0; i < 6; ++i)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_LINE_1 + i);
			DROPDOWN_SetTextSkin(hItem);
			DROPDOWN_SetAutoScroll(hItem, 1);
			DROPDOWN_SetUpMode(hItem, 1);
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
			for (j = 1; j <= 100; ++j)
			{
				mini_snprintf(GUITextBuffer, 30, "%s %d", GUI_LANG_GetText(6), j);
				DROPDOWN_AddString(hItem, GUITextBuffer);
			}
			DROPDOWN_SetSel(hItem,	tempUserTrendsSettings[0].channelID[i] + 1);
		}
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (NCode)
		{
		case WM_NOTIFICATION_RELEASED:
			BUZZER_Beep();
			switch (Id)
			{
			case ID_EDIT_TREND_DESC:
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 20);
				break;
			case ID_EDIT_TREND_MAX:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			case ID_EDIT_TREND_MIN:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			case ID_DROPDOWN_SELECT_TREND:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TREND);
				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_DESC), tempUserTrendsSettings[DROPDOWN_GetSel(hItem)].description, 48);
				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MAX), GUITextBuffer, 10);
				tempUserTrendsSettings[DROPDOWN_GetSel(hItem)].max = atoff(GUITextBuffer);
				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MIN), GUITextBuffer, 10);
				tempUserTrendsSettings[DROPDOWN_GetSel(hItem)].min = atoff(GUITextBuffer);
				break;
			}
			break;
		case WM_NOTIFICATION_SEL_CHANGED:
			if (Id == ID_DROPDOWN_SELECT_TREND)
			{
				Selection = DROPDOWN_GetSel(pMsg->hWinSrc);
				for (i = 0; i < 6; ++i)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_LINE_1 + i);
					DROPDOWN_SetSel(hItem, tempUserTrendsSettings[Selection].channelID[i]+1);
				}
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_DESC);
				EDIT_SetText(hItem, tempUserTrendsSettings[Selection].description);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MAX);
				EDIT_SetFloat(hItem, tempUserTrendsSettings[Selection].max, 4);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MIN);
				EDIT_SetFloat(hItem, tempUserTrendsSettings[Selection].min, 4);

			}
			else if (Id <= ID_DROPDOWN_SOURCE_LINE_6 && Id >= ID_DROPDOWN_SOURCE_LINE_1)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TREND);
				Selection = DROPDOWN_GetSel(hItem);
				tempUserTrendsSettings[Selection].channelID[Id - ID_DROPDOWN_SOURCE_LINE_1] = DROPDOWN_GetSel(pMsg->hWinSrc) - 1;
			}

			break;
		case WM_NOTIFICATION_VALUE_CHANGED:
			if (Id == ID_EDIT_TREND_DESC)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TREND);
				Selection = DROPDOWN_GetSel(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_DESC);
				EDIT_GetText(hItem, tempUserTrendsSettings[Selection].description, 17);
			}
			break;
		}
		break;

	case WM_POST_PAINT:
		DRAW_RoundedFrame(385, 8, 330, 39, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(385, 58, 285, 39, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(385, 108, 285, 39, 4, 2, GUI_BLUE);

		for (i = 0; i < 3; i++)
			DRAW_Rect(250, 160 + 50 * i, 80, 33, 1, penColor[i], penColor[i]);
		for (i = 0; i < 3; i++)
			DRAW_Rect(630, 160 + 50 * i, 80, 33, 1, penColor[i + 3], penColor[i + 3]);
		break;
	case WM_DELETE:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TREND);
		Selection = DROPDOWN_GetSel(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_DESC);
		EDIT_GetText(hItem, tempUserTrendsSettings[Selection].description, 17);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MAX);
		EDIT_GetText(hItem, GUITextBuffer, 21);
		tempUserTrendsSettings[Selection].max = atoff(GUITextBuffer);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TREND_MIN);
		EDIT_GetText(hItem, GUITextBuffer, 21);
		tempUserTrendsSettings[Selection].min = atoff(GUITextBuffer);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetUserScreensDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem, temp;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		for (int i = 0; i < 6; ++i)
			CopyUserTrendsSettings(&tempUserTrendsSettings[i], &bkUserTrendsSettings[i]);
		CopyUserTablesSettings(&tempUserTablesSettings, &bkUserTablesSettings);

		strncpy(GUILocalStringBuffer[0], "PV", 10);
		strncpy(GUILocalStringBuffer[1], GUI_LANG_GetText(35), 10);
		strncpy(GUILocalStringBuffer[2], GUI_LANG_GetText(65), 10);
		strncpy(GUILocalStringBuffer[3], "∑1", 10);
		strncpy(GUILocalStringBuffer[4], "∑2", 10);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_SCREENS);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		temp = CreateTableSet(hItem);
		MULTIPAGE_AddEmptyPage(hItem, temp, GUI_LANG_GetText(2));
		temp = CreateTrendsSet(hItem);
		MULTIPAGE_AddEmptyPage(hItem, temp, GUI_LANG_GetText(3));
		MULTIPAGE_SetTabWidth(hItem, 365, 0);
		MULTIPAGE_SetTabWidth(hItem, 365, 1);
		MULTIPAGE_SelectPage(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				NewSettings = 1;

				WM_DeleteWindow(pMsg->hWin);
				for (int i = 0; i < 6; ++i)
					CopyUserTrendsSettings(&bkUserTrendsSettings[i], &tempUserTrendsSettings[i]);
				CopyUserTablesSettings(&bkUserTablesSettings, &tempUserTablesSettings);
				break;
			}
			break;
		case ID_MULTIPAGE_USER_SCREENS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_CANCEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_DELETE:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_TABLE);
		EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_TABLE_DESC), tempUserTablesSettings.TablesTitles[DROPDOWN_GetSel(hItem)], 41);
		break;
	case WM_USER_LOGIN:
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
			WM_HideWindow(hItem);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetUserData(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_LIGHTBLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetTitleHeight(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_CHANNEL);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		for (int j = 1; j <= 100; j++)
		{
			mini_snprintf(GUITextBuffer, 30, "%s %d", GUI_LANG_GetText(6), j);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_TYPE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "PV");
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(35));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(65));
		DROPDOWN_AddString(hItem, "∑1");
		DROPDOWN_AddString(hItem, "∑2");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CONFIRM);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 73, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ANNUL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 79, 16);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_CONFIRM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_CHANNEL);

				if (DROPDOWN_GetSel(hItem) == 0)
				{
					tempUserTablesSettings.TablesData[cellSelection] = -1;
					BUTTON_SetText(hButtonToModify, GUI_LANG_GetText(27));
				}
				else
				{
					tempUserTablesSettings.TablesData[cellSelection] = (DROPDOWN_GetSel(hItem) - 1)
					+ 100 * DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_TYPE));

					mini_snprintf(GUITextBuffer, 30, "%s %d: %s",
							GUI_LANG_GetText(6),
							tempUserTablesSettings.TablesData[cellSelection] % 100 + 1,
							GUILocalStringBuffer[(tempUserTablesSettings.TablesData[cellSelection]) / 100]);
					BUTTON_SetText(hButtonToModify, GUITextBuffer);
				}

				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_ANNUL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_DROPDOWN_SOURCE_CHANNEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_SEL_CHANGED:
				if (DROPDOWN_GetSel(pMsg->hWinSrc) == 0)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_TYPE);
					WM_HideWindow(hItem);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SOURCE_TYPE);
					WM_ShowWindow(hItem);
				}
				break;
			}
			break;
		}
		break;
	}
}

WM_HWIN CreateSetUserScreens(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetUserScreensDialogCreate, GUI_COUNTOF(_aSetUserScreensDialogCreate), _cbSetUserScreensDialog, hParent, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(11));
	return hWin;
}

static WM_HWIN CreateTableSet(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetUserTablesDialogCreate, GUI_COUNTOF(_aSetUserTablesDialogCreate), _cbSetUserTableDialog,
			hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateTrendsSet(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetUserTrendsDialogCreate, GUI_COUNTOF(_aSetUserTrendsDialogCreate), _cbSetUserTrendsDialog,
			hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetUserData(WM_HWIN hParent, short selection)
{
	WM_HWIN hWin, hItem;
	hWin = GUI_CreateDialogBox(_aSetUserDataDialogCreate, GUI_COUNTOF(_aSetUserDataDialogCreate), _cbSetUserData,
			hParent, 0, 0);
	WM_MakeModal(hWin);
	if (selection == -1)
	{
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_SOURCE_CHANNEL);
		DROPDOWN_SetSel(hItem, 0);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_SOURCE_TYPE);
		WM_HideWindow(hItem);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_SOURCE_CHANNEL);
		DROPDOWN_SetSel(hItem, selection % 100 + 1);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_SOURCE_TYPE);
		WM_ShowWindow(hItem);
		DROPDOWN_SetSel(hItem, selection / 100);
	}
	return hWin;
}

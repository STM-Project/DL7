#include "channels.h"
#include "SingleChannelDLG.h"
#include "draw.h"
#include "skins.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"

#include "WindowManagement.h"
#include "buzzer.h"
#include "fram.h"
#include "archive.h"
#include "passwords.h"
#include "rtc.h"

#include "EDIT_double.h"
#include "ChannelsSynchronization.h"
#include "mini-printf.h"
#include "dtos.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato40D;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato60;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato90;

extern GUI_CONST_STORAGE GUI_BITMAP bmCLOSE;

#define ID_WINDOW_SINGLE_CHANNEL	(GUI_ID_USER + 0x101)
#define ID_GRAPH_SINGLE_CHANNEL		(GUI_ID_USER + 0x102)
#define ID_TEXT_UNIT							(GUI_ID_USER + 0x103)
#define ID_TEXT_DESC							(GUI_ID_USER + 0x104)
#define ID_TEXT_TOT_1							(GUI_ID_USER + 0x105)
#define ID_TEXT_TOT_2 						(GUI_ID_USER + 0x106)
#define ID_TEXT_MIN 							(GUI_ID_USER + 0x107)
#define ID_TEXT_MAX 							(GUI_ID_USER + 0x108)
#define ID_TEXT_RESET 						(GUI_ID_USER + 0x109)
#define ID_GRAPH_BUTTON 					(GUI_ID_USER + 0x10A)
#define ID_EDIT_VALUE 						(GUI_ID_USER + 0x10B)
#define ID_EDIT_TOT_1 						(GUI_ID_USER + 0x10C)
#define ID_EDIT_TOT_2 						(GUI_ID_USER + 0x10D)
#define ID_EDIT_MIN 							(GUI_ID_USER + 0x10E)
#define ID_EDIT_MAX 							(GUI_ID_USER + 0x10F)
#define ID_TEXT_TOTALIZER_1_UNIT 	(GUI_ID_USER + 0x110)
#define ID_TEXT_TOTALIZER_2_UNIT 	(GUI_ID_USER + 0x111)
#define ID_RESET_BUTTON 					(GUI_ID_USER + 0x112)

#define ID_FRAMEWIN_RESET_MENU 		(GUI_ID_USER + 0x113)
#define ID_TEXT_RESET_INFO 				(GUI_ID_USER + 0x114)
#define ID_BUTTON_RESET_SINGLE_CHANNEL 	(GUI_ID_USER + 0x115)
#define ID_BUTTON_RESET_ALL_CHANNELS 		(GUI_ID_USER + 0x116)

#define ID_SLIDER_TREND 			(GUI_ID_USER + 0x117)
#define ID_BUTTON_TREND_UP 		(GUI_ID_USER + 0x118)
#define ID_BUTTON_TREND_DOWN 	(GUI_ID_USER + 0x119)

#define ID_TEXT_TREND_MAX				(GUI_ID_USER + 0x11A)
#define ID_TEXT_THIRD_QUARTER		(GUI_ID_USER + 0x11B)
#define ID_TEXT_HALF						(GUI_ID_USER + 0x11C)
#define ID_TEXT_FIRST_QUARTER		(GUI_ID_USER + 0x11D)
#define ID_TEXT_TREND_MIN				(GUI_ID_USER + 0x11E)
#define ID_TEXT_TREND_TIME_UNIT	(GUI_ID_USER + 0x11F)

#define MAX_TREND_POINTS 390

static void MaximizeGraph(WM_HWIN hWin);
static void MinimizeGraph(WM_HWIN hWin);

static int ResetAllAuxiliaryValues(void);
static WM_HWIN CreateResetMenu(WM_HWIN hParent);

static WM_HWIN hResetMenu;
static CHANNEL *localChannel;

static GRAPH_DATA_Handle hTrendData;
static GRAPH_SCALE_Handle hTrendScale;
static GUI_POINT localTrendPoints[HIST_DATA_SIZE] __attribute__ ((section(".sdram")));

static float trendFactor = 3;
static int trendPenSize = 4;
static int trendFullscreenFlag = 0;
static int trendOffsetX = 0;

static float localGraphMax = 0;
static float localGraphMin = 0;

static int CopyHistDataToGraphPoints(GUI_POINT *data, CHANNEL *channel)
{
	float *pHist = channel->histData->head;
	GUI_POINT *pData = data;
	int i = 0;
	int pointsOnTrend = 0;
	if (trendOffsetX)
	{
		for (i = 0; i < trendOffsetX / 2; i++)
		{
			if (pHist == &channel->histData->data[0])
				pHist = &channel->histData->data[HIST_DATA_SIZE - 1];
			else
				pHist--;
		}
	}

	pointsOnTrend = channel->histData->numberOfItems - trendOffsetX / 2;

	if (pointsOnTrend <= 0)
		pointsOnTrend = 0;
	else if (pointsOnTrend > MAX_TREND_POINTS)
		pointsOnTrend = MAX_TREND_POINTS;

	for (i = 0; i < pointsOnTrend; i++)
	{
		if (localGraphMin < localGraphMax)
		{
			if (*pHist < localGraphMin || *pHist == 0x7FFF)
				pData->y = (short) (localGraphMin / trendFactor) - 4;
			else if (*pHist > localGraphMax)
				pData->y = (short) (localGraphMax / trendFactor) + 4;
			else
				pData->y = (*pHist / trendFactor);
		}
		else
		{
			if (*pHist > localGraphMin || *pHist == 0x7FFF)
				pData->y = (short) (localGraphMin / trendFactor) - 4;
			else if (*pHist < localGraphMax)
				pData->y = (short) (localGraphMax / trendFactor) + 4;
			else
				pData->y = (*pHist / trendFactor);
		}

		if (pHist == &channel->histData->data[0])
			pHist = &channel->histData->data[HIST_DATA_SIZE - 1];
		else
			pHist--;
		pData++;
	}
	return pointsOnTrend;
}

static GRAPH_DATA_Handle UpdateTrendData(CHANNEL *channel, GUI_POINT *data)
{
	int pointsOnTrend = CopyHistDataToGraphPoints(data, channel);
	return GRAPH_DATA_XY_Create(SKINS_GetTextColor(), MAX_TREND_POINTS, data, pointsOnTrend);
}

void SetChannelValue(WM_HWIN hEdit, CHANNEL * kanal)
{
	if (kanal->failureMode == 0)
	{
		switch (kanal->failureState)
		{
		case 1:
			EDIT_SetShortFloat(hEdit, kanal->value, kanal->resolution);
			EDIT_SetTextColor(hEdit, EDIT_CI_ENABLED, kanal->Color);
			break;
		case 0:
			EDIT_SetText(hEdit, "-------");
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			break;
		case 10:
			EDIT_SetText(hEdit, "---W---");
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			break;
		case 2:
			EDIT_SetText(hEdit, "---||---");
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			break;
		case 3:
			EDIT_SetText(hEdit, "---E---");
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			break;
		case 4:
			EDIT_SetText(hEdit, "---R---");
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			break;
		default:
			EDIT_SetText(hEdit, "--ERR--");
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			break;
		}
	}
	else if (kanal->failureMode == 1)
	{
		EDIT_SetShortFloat(hEdit, kanal->value, kanal->resolution);
		if (kanal->failureState == 1)
		{
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			EDIT_SetTextColor(hEdit, EDIT_CI_ENABLED, kanal->Color);
		}
		else
		{
			EDIT_SetTextColor(hEdit, EDIT_CI_ENABLED, GUI_BLACK);
			EDIT_SetBkColor(hEdit, EDIT_CI_ENABLED, GUI_YELLOW);
		}
	}
}

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "CH ", ID_WINDOW_SINGLE_CHANNEL, 0, 60, 740, 420, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "Description", ID_TEXT_DESC, 10, 0, 720, 60, 0, 0x64, 0 },
				{ TEXT_CreateIndirect, "Units", ID_TEXT_UNIT, 500, 150, 230, 60, TEXT_CF_RIGHT | TEXT_CF_TOP, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_VALUE, 10, 65, 720, 90, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 11 },

				{ TEXT_CreateIndirect, "∑1", ID_TEXT_TOT_1, 10, 170, 35, 40, TEXT_CF_LEFT | GUI_TA_BOTTOM, 0x64, 0 },
				{ EDIT_CreateIndirect, "L1_V", ID_EDIT_TOT_1, 45, 170, 240, 40, TEXT_CF_RIGHT | EDIT_CF_BOTTOM, 0x64, 15 },
				{ TEXT_CreateIndirect, "L1_U", ID_TEXT_TOTALIZER_1_UNIT, 290, 170, 75, 40, TEXT_CF_LEFT | GUI_TA_BOTTOM, 0x64, 0 },

				{ TEXT_CreateIndirect, "∑2", ID_TEXT_TOT_2, 10, 219, 35, 40, TEXT_CF_LEFT | GUI_TA_BOTTOM, 0x64, 0 },
				{ EDIT_CreateIndirect, "L2_V", ID_EDIT_TOT_2, 45, 219, 240, 40, TEXT_CF_RIGHT | EDIT_CF_BOTTOM, 0x64, 15 },
				{ TEXT_CreateIndirect, "L1_U", ID_TEXT_TOTALIZER_2_UNIT, 290, 219, 75, 40, TEXT_CF_LEFT | GUI_TA_BOTTOM, 0x64, 0 },

				{ TEXT_CreateIndirect, "▲", ID_TEXT_MAX, 10, 269, 100, 30, TEXT_CF_LEFT | EDIT_CF_BOTTOM, 0x64, 0 },
				{ EDIT_CreateIndirect, "MAX_V", ID_EDIT_MAX, 110, 269, 250, 30, EDIT_CF_RIGHT | EDIT_CF_VCENTER, 0x64, 15 },

				{ TEXT_CreateIndirect, "▼", ID_TEXT_MIN, 50, 309, 100, 30, TEXT_CF_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "MIN_V", ID_EDIT_MIN, 150, 309, 210, 30, EDIT_CF_RIGHT | EDIT_CF_VCENTER, 0x64,15 },

				{ TEXT_CreateIndirect, "RESET:", ID_TEXT_RESET, 90, 349, 270, 30, TEXT_CF_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_RESET_BUTTON, 10, 170, 355, 210, WM_CF_HASTRANS, 0 },

				{ GRAPH_CreateIndirect, "Graph", ID_GRAPH_SINGLE_CHANNEL, 370, 210, 360, 200, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", ID_GRAPH_BUTTON, 370, 210, 360, 200, WM_CF_HASTRANS, 0 },

				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_MAX, 15, 35, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_THIRD_QUARTER, 15, 103, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_HALF, 15, 185, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_FIRST_QUARTER, 15, 268, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_MIN, 15, 321, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_TIME_UNIT, 735, 385, 61, 24, GUI_TA_RIGHT | GUI_TA_BOTTOM, 0x64, 0 },

				{ SLIDER_CreateIndirect, "", ID_SLIDER_TREND, 101, 382, 538, 35, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "◀", ID_BUTTON_TREND_DOWN, 10, 385, 90, 33, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "▶", ID_BUTTON_TREND_UP, 640, 385, 90, 33, 0, 0x0, 0 },

		};

static const GUI_WIDGET_CREATE_INFO _aResetMenuDialogCreate[] =
		{
				{ FRAMEWIN_CreateIndirect, "Reset Menu", ID_FRAMEWIN_RESET_MENU, 80, 120, 580, 167, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "Reset values: ▼ (MIN) ,▲ (MAX), ∑1, ∑2", ID_TEXT_RESET_INFO, 10, 39, 560, 60, 0, 0x64, 0 },
				{ BUTTON_CreateIndirect, "Single Channel", ID_BUTTON_RESET_SINGLE_CHANNEL, 2, 104, 285, 58, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "All Channels", ID_BUTTON_RESET_ALL_CHANNELS, 289, 104, 285, 58, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", GUI_ID_CLOSE, 544, 0, 32, 34, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "Title", GUI_ID_HELP, 0, 0, 544, 34, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
		};

static void _cbResetMenuDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[100];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetBarColor(hItem, 0, GUI_BLUE);

		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_BLUE);
		FRAMEWIN_SetBorderSize(hItem, 1);
		FRAMEWIN_SetText(hItem, "");
		FRAMEWIN_SetTitleVis(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CLOSE);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED, GUI_BLUE);
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmapEx(hItem, 0, &bmCLOSE, 0, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_HELP);
		TEXT_SetText(hItem, "");
		TEXT_SetTextColor(hItem, GUI_WHITE);
		TEXT_SetBkColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RESET_INFO);
		mini_snprintf(GUITextBuffer, 100, "%s\n▲ (%s), ▼ (%s), ∑1, ∑2", GUI_LANG_GetText(119), GUI_LANG_GetText(35),
				GUI_LANG_GetText(65));
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RESET_SINGLE_CHANNEL);
		BUTTON_SetText(hItem, GUI_LANG_GetText(120));
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RESET_ALL_CHANNELS);
		BUTTON_SetText(hItem, GUI_LANG_GetText(121));
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case GUI_ID_CLOSE:

			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
					WM_MakeModal(0);
					break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(hResetMenu);
				break;
			}
			break;

		case ID_BUTTON_RESET_SINGLE_CHANNEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
					WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
				{
					BUZZER_Beep();
					localChannel = &Channels[WindowInstance];
					if (localChannel->Tot1.type == 2)
						localChannel->Tot1.value = 0;
					if (localChannel->Tot2.type == 2)
						localChannel->Tot2.value = 0;
					localChannel->min = 1000000000.0;
					FRAM_WriteMultiple(START_MINIMUM_ADDR + 4 * WindowInstance, (uint8_t*) &localChannel->min, 4);
					localChannel->max = -1000000000.0;
					FRAM_WriteMultiple(START_MAXIMUM_ADDR + 4 * WindowInstance, (uint8_t*) &localChannel->max, 4);
					RTC_GetTimeAndDate(&localChannel->ResetTime, &localChannel->ResetDate);
					FRAM_Write((START_RESET_DATE_ADDR + 5 * WindowInstance), localChannel->ResetDate.Date);
					FRAM_Write((START_RESET_DATE_ADDR + 1 + 5 * WindowInstance), localChannel->ResetDate.Month);
					FRAM_Write((START_RESET_DATE_ADDR + 2 + 5 * WindowInstance), localChannel->ResetDate.Year);
					FRAM_Write((START_RESET_DATE_ADDR + 3 + 5 * WindowInstance), localChannel->ResetTime.Hours);
					FRAM_Write((START_RESET_DATE_ADDR + 4 + 5 * WindowInstance), localChannel->ResetTime.Minutes);

					hItem = WM_GetDialogItem(hActualWindow, ID_TEXT_RESET);
					mini_snprintf(GUITextBuffer,25, "RESET %02d-%02d-%02d, %02d:%02d",  localChannel->ResetDate.Date, localChannel->ResetDate.Month, localChannel->ResetDate.Year,
							localChannel->ResetTime.Hours, localChannel->ResetTime.Minutes);
					TEXT_SetText(hItem, GUITextBuffer);
					mini_snprintf(GUITextBuffer, 100, "SYS:CH%d: AUX VALUES RESET", WindowInstance + 1);
					ARCHIVE_SendEvent(GUITextBuffer);

					WM_DeleteWindow(hResetMenu);
				}
				break;
			}
			break;
		case ID_BUTTON_RESET_ALL_CHANNELS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
					WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
				{
					BUZZER_Beep();
					ResetAllAuxiliaryValues();
					ARCHIVE_SendEvent("SYS: ALL CHANNELS: AUX VALUES RESET");

					localChannel = &Channels[WindowInstance];

					hItem = WM_GetDialogItem(hActualWindow, ID_TEXT_RESET);
					mini_snprintf(GUITextBuffer,25, "RESET %02d-%02d-%02d, %02d:%02d",  localChannel->ResetDate.Date, localChannel->ResetDate.Month, localChannel->ResetDate.Year,
							localChannel->ResetTime.Hours, localChannel->ResetTime.Minutes);
					TEXT_SetText(hItem, GUITextBuffer);

					WM_DeleteWindow(hResetMenu);
				}
				break;
			}
			break;
		}
		break;
	}
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	float trendOYDiff = 0.0;

	char GUITextBuffer[22];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_SINGLE_CHANNEL);
		GRAPH_SetGridVis(hItem, 1);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_GRID);
		GRAPH_SetLineStyle(hItem, GUI_LS_DASH);
		GRAPH_SetSkinColors(hItem);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_FRAME);
		GRAPH_SetBorder(hItem, 2, 2, 2, 30);
		GRAPH_SetAutoScrollbar(hItem,GUI_COORD_X,0);

		hTrendScale = GRAPH_SCALE_Create(9, GUI_TA_LEFT | GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, 120);
		GRAPH_SCALE_SetFont(hTrendScale, &GUI_FontLato30);
		GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX);
		GRAPH_SCALE_SetNumDecs(hTrendScale, 0);
		GRAPH_SCALE_SetTextColor(hTrendScale, GUI_BLUE);
		GRAPH_AttachScale(hItem, hTrendScale);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UNIT);
		TEXT_SetText(hItem, localChannel->unit);
		TEXT_SetBkColor(hItem, GUI_TRANSPARENT);
		TEXT_SetFont(hItem, &GUI_FontLato60);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DESC);
		TEXT_SetFont(hItem, &GUI_FontLato60);
		TEXT_SetText(hItem, localChannel->description);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VALUE);
		EDIT_SetFocussable(hItem, 0);
		EDIT_SetInsertMode(hItem, 0);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);
		EDIT_SetFont(hItem, &GUI_FontLato90);
		SetChannelValue(hItem, localChannel);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MIN);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
		EDIT_SetFocussable(hItem, 0);
		EDIT_SetInsertMode(hItem, 0);
		EDIT_SetLongFloat(hItem, localChannel->min, localChannel->resolution);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MAX);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
		EDIT_SetFocussable(hItem, 0);
		EDIT_SetInsertMode(hItem, 0);
		EDIT_SetLongFloat(hItem, localChannel->max, localChannel->resolution);

		if (localChannel->Tot1.type)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_1);
			TEXT_SetTextColor(hItem, GUI_BLUE);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_1_UNIT);
			TEXT_SetText(hItem, localChannel->Tot1.unit);
			TEXT_SetTextColor(hItem, GUI_BLUE);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_1);
			EDIT_SetFocussable(hItem, 0);
			EDIT_SetInsertMode(hItem, 0);
			EDIT_SetFont(hItem, &GUI_FontLato40D);
			EDIT_SetLongStringDouble(hItem, localChannel->Tot1.value, localChannel->Tot1.resolution);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_1);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_1_UNIT);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_1);
			WM_HideWindow(hItem);
		}

		if (localChannel->Tot2.type)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_2);
			TEXT_SetTextColor(hItem, GUI_BLUE);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_2_UNIT);
			TEXT_SetText(hItem, localChannel->Tot2.unit);
			TEXT_SetTextColor(hItem, GUI_BLUE);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_2);
			EDIT_SetFocussable(hItem, 0);
			EDIT_SetInsertMode(hItem, 0);
			EDIT_SetFont(hItem, &GUI_FontLato40D);
			EDIT_SetLongStringDouble(hItem, localChannel->Tot2.value, localChannel->Tot2.resolution);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOT_2);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_2_UNIT);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_2);
			WM_HideWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RESET);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		mini_snprintf(GUITextBuffer,22 ,"RESET %02d-%02d-%02d, %02d:%02d",
				localChannel->ResetDate.Date, localChannel->ResetDate.Month, localChannel->ResetDate.Year,
				localChannel->ResetTime.Hours, localChannel->ResetTime.Minutes);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MAX);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetFloat(hItem,localChannel->graph_max_value,localChannel->resolution);
		trendOYDiff = localChannel->graph_max_value - localChannel->graph_min_value;
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_THIRD_QUARTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetFloat(hItem,localChannel->graph_max_value - trendOYDiff / 4,localChannel->resolution);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HALF);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetFloat(hItem,localChannel->graph_max_value -trendOYDiff / 2,localChannel->resolution);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FIRST_QUARTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetFloat(hItem,localChannel->graph_min_value + trendOYDiff / 4,localChannel->resolution);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MIN);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetFloat(hItem,localChannel->graph_min_value,localChannel->resolution);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_TIME_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetBkColor(hItem, SKINS_GetBkColor());
		TEXT_SetText(hItem, "[s]");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
		SLIDER_SetSkinClassic(hItem);
		SLIDER_EnableFocusRect(hItem,0);
		SLIDER_SetBarColor(hItem, GUI_GRAY);
		SLIDER_SetRange(hItem, -108, 0);
		SLIDER_SetNumTicks(hItem, 0);
		SLIDER_SetWidth(hItem, 80);

		if (trendFullscreenFlag)
			MaximizeGraph(pMsg->hWin);
		else
			MinimizeGraph(pMsg->hWin);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{

		case ID_RESET_BUTTON:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
					hResetMenu = CreateResetMenu(pMsg->hWin);
				else
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				break;
			}
			break;
		case ID_GRAPH_BUTTON:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				if (!trendFullscreenFlag)
					MaximizeGraph(pMsg->hWin);
				else
					MinimizeGraph(pMsg->hWin);
				break;
			}
			break;
		case ID_SLIDER_TREND:
			switch (NCode)
			{
			case WM_NOTIFICATION_VALUE_CHANGED:
				trendOffsetX = -60 * SLIDER_GetValue(pMsg->hWinSrc);
				GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX + 721);
				GRAPH_DATA_XY_SetOffX(hTrendData, trendOffsetX);

				break;
			}
			break;
		case ID_BUTTON_TREND_UP:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
				SLIDER_Inc(hItem);
				break;
			}
			break;
		case ID_BUTTON_TREND_DOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
				SLIDER_Dec(hItem);
				break;
			}
			break;
		}
		break;
	case WM_TIMER:
		localChannel = &Channels[WindowInstance];

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VALUE);
		SetChannelValue(hItem, localChannel);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_1);
		if (localChannel->Tot1.type)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_1);
			EDIT_SetLongStringDouble(hItem, localChannel->Tot1.value, localChannel->Tot1.resolution);
		}

		if (localChannel->Tot2.type)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOT_2);
			EDIT_SetLongStringDouble(hItem, localChannel->Tot2.value, localChannel->Tot2.resolution);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MIN);
		EDIT_SetLongFloat(hItem, localChannel->min, localChannel->resolution);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAX);
		EDIT_SetLongFloat(hItem, localChannel->max, localChannel->resolution);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_SINGLE_CHANNEL);
		GRAPH_DetachData(hItem, hTrendData);

		if(hTrendData)
			GRAPH_DATA_XY_Delete(hTrendData);

		hTrendData = UpdateTrendData(localChannel, localTrendPoints);
		GRAPH_DATA_XY_SetPenSize(hTrendData, trendPenSize);
		GRAPH_DATA_XY_SetOffY(hTrendData, -localGraphMin / trendFactor);
		GRAPH_AttachData(hItem, hTrendData);

		WM_RestartTimer(pMsg->Data.v, 500);
		break;

	case WM_POST_PAINT:
		if(!trendFullscreenFlag)
		{
			DRAW_Line(10, 211, 360, 211, 1, GUI_GRAY);
			DRAW_Line(10, 260, 360, 260, 1, GUI_GRAY);
			DRAW_Line(10, 300, 360, 300, 1, GUI_GRAY);
			DRAW_Line(50, 340, 360, 340, 1, GUI_GRAY);
			DRAW_Line(90, 380, 360, 380, 1, GUI_GRAY);
		}
		break;
	case WM_DELETE:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_SINGLE_CHANNEL);
		GRAPH_DetachData(hItem, hTrendData);

		if(hTrendData)
		{
			GRAPH_DATA_XY_Delete(hTrendData);
			hTrendData = 0;
		}

		break;
	case WM_USER_LOGIN:
		if (pMsg->Data.v == NO_USER && hResetMenu != 0)
		{
			WM_DeleteWindow(hResetMenu);
			hResetMenu = 0;
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSingleChannelWindow(int instance)
{
	WM_HWIN hWin;
	char titleBuffer[16] =
			{ 0 };
	WindowInstance = instance;
	localChannel = &Channels[WindowInstance];

	localGraphMax = localChannel->graph_max_value;
	localGraphMin = localChannel->graph_min_value;

	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);

	mini_snprintf(titleBuffer, 32, "%s %d", GUI_LANG_GetText(6), instance + 1);
	USERMESSAGE_ChangeWinTitle(titleBuffer);
	WM_CreateTimer(hWin, instance, 1, 0);
	return hWin;
}

static WM_HWIN CreateResetMenu(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aResetMenuDialogCreate, GUI_COUNTOF(_aResetMenuDialogCreate), _cbResetMenuDialog,
			hParent,
			0, 0);
	WM_MakeModal(hWin);
	WM_SetFocus(hWin);
	return hWin;
}

static int ResetAllAuxiliaryValues(void)
{
	RTC_TimeTypeDef TempTime;
	RTC_DateTypeDef TempDate;
	if ( TakeChannelsMutex(100))
	{
		RTC_GetTimeAndDate(&TempTime, &TempDate);
		for (int i = 0; i < 100; i++)
		{
			if (Channels[i].source.type != 0)
			{
				if (Channels[i].Tot1.type == 2)
					Channels[i].Tot1.value = 0;
				if (Channels[i].Tot2.type == 2)
					Channels[i].Tot2.value = 0;
				Channels[i].min = 1000000000.0;
				FRAM_WriteMultiple(START_MINIMUM_ADDR + 4 * i, (uint8_t*) &Channels[i].min, 4);
				Channels[i].max = -1000000000.0;
				FRAM_WriteMultiple(START_MAXIMUM_ADDR + 4 * i, (uint8_t*) &Channels[i].max, 4);
				Channels[i].ResetDate = TempDate;
				Channels[i].ResetTime = TempTime;
				FRAM_Write((START_RESET_DATE_ADDR + 5 * i), Channels[i].ResetDate.Date);
				FRAM_Write((START_RESET_DATE_ADDR + 1 + 5 * i), Channels[i].ResetDate.Month);
				FRAM_Write((START_RESET_DATE_ADDR + 2 + 5 * i), Channels[i].ResetDate.Year);
				FRAM_Write((START_RESET_DATE_ADDR + 3 + 5 * i), Channels[i].ResetTime.Hours);
				FRAM_Write((START_RESET_DATE_ADDR + 4 + 5 * i), Channels[i].ResetTime.Minutes);
			}
		}
		GiveChannelsMutex();
		return 1;
	}
	else
		return 0;
}

static void MaximizeGraph(WM_HWIN hWin)
{
	WM_HWIN hItem;

	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_DESC), &GUI_FontLato30);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_UNIT);
	WM_MoveChildTo(hItem, 600, 0);
	WM_SetSize(hItem, 130, 30);
	TEXT_SetFont(hItem, &GUI_FontLato30);
	TEXT_SetTextAlign(hItem, EDIT_CF_LEFT | EDIT_CF_VCENTER);

	hItem = WM_GetDialogItem(hWin, ID_EDIT_VALUE);
	WM_MoveChildTo(hItem, 445, 0);
	WM_SetSize(hItem, 150, 30);
	EDIT_SetFont(hItem, &GUI_FontLato30);
	EDIT_SetTextAlign(hItem, EDIT_CF_RIGHT | EDIT_CF_VCENTER);

	hItem = WM_GetDialogItem(hWin, ID_GRAPH_BUTTON);
	WM_MoveChildTo(hItem, 10, 30);
	WM_SetSize(hItem, 720, 330);
	WM_HideWindow(WM_GetDialogItem(hWin, ID_RESET_BUTTON));

	hItem = WM_GetDialogItem(hWin, ID_TEXT_THIRD_QUARTER);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_HALF);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_FIRST_QUARTER);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_TIME_UNIT);
	WM_MoveChildTo(hItem, 670, 360);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_MAX);
	WM_MoveChildTo(hItem, 15, 32);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_MIN);
	WM_MoveChildTo(hItem, 15, 338);

	hItem = WM_GetDialogItem(hWin, ID_BUTTON_TREND_DOWN);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_BUTTON_TREND_UP);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_SLIDER_TREND);
	WM_ShowWindow(hItem);
	SLIDER_SetValue(hItem, 0);

	hItem = WM_GetDialogItem(hWin, ID_GRAPH_SINGLE_CHANNEL);
	WM_MoveChildTo(hItem, 10, 30);
	WM_SetSize(hItem, 720, 350);

	GRAPH_SetGridDistX(hItem, 60);
	GRAPH_SetGridDistY(hItem, 41);
	GRAPH_SetGridOffY(hItem,40);
	GRAPH_SetBorder(hItem, 2, 2, 2, 21);

	GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX + 721);
	GRAPH_SCALE_SetFactor(hTrendScale, 0.5);
	GRAPH_SCALE_SetPos(hTrendScale, WM_GetWindowSizeY(hItem) - 10);

	trendOffsetX = 0;
	trendFactor = (localGraphMax - localGraphMin) / 328.0;
	trendPenSize = 4;
	trendFullscreenFlag = 1;
	for (int i = 0; i < HIST_DATA_SIZE; ++i)
		localTrendPoints[i].x = 720 - 2 * i;
}

static void MinimizeGraph(WM_HWIN hWin)
{
	WM_HWIN hItem;

	TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT_DESC), &GUI_FontLato60);

	hItem = WM_GetDialogItem(hWin, ID_EDIT_VALUE);
	WM_MoveChildTo(hItem, 10, 65);
	WM_SetSize(hItem, 720, 90);
	EDIT_SetFont(hItem, &GUI_FontLato90);
	EDIT_SetTextAlign(hItem, EDIT_CF_HCENTER | EDIT_CF_VCENTER);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_UNIT);
	WM_MoveChildTo(hItem, 500, 150);
	WM_SetSize(hItem, 230, 60);
	TEXT_SetFont(hItem, &GUI_FontLato60);
	TEXT_SetTextAlign(hItem, EDIT_CF_RIGHT | EDIT_CF_VCENTER);

	hItem = WM_GetDialogItem(hWin, ID_GRAPH_BUTTON);
	WM_MoveChildTo(hItem, 370, 210);
	WM_SetSize(hItem, 360, 200);
	WM_ShowWindow(WM_GetDialogItem(hWin, ID_RESET_BUTTON));

	hItem = WM_GetDialogItem(hWin, ID_TEXT_THIRD_QUARTER);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_HALF);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_FIRST_QUARTER);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_TIME_UNIT);
	WM_MoveChildTo(hItem, 670, 390);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_MAX);
	WM_MoveChildTo(hItem, 376, 212);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_MIN);
	WM_MoveChildTo(hItem, 376, 368);

	hItem = WM_GetDialogItem(hWin, ID_BUTTON_TREND_DOWN);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_BUTTON_TREND_UP);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_SLIDER_TREND);
	WM_HideWindow(hItem);

	hItem = WM_GetDialogItem(hWin, ID_GRAPH_SINGLE_CHANNEL);
	WM_MoveChildTo(hItem, 371,210);
	WM_SetSize(hItem, 359, 200);

	GRAPH_SetGridDistX(hItem, 60);
	GRAPH_SetGridDistY(hItem, 20);
	GRAPH_SetGridOffY(hItem, 19);
	GRAPH_SetBorder(hItem, 2, 2, 2, 21);

	GRAPH_SCALE_SetOff(hTrendScale, 362);
	GRAPH_SCALE_SetFactor(hTrendScale, 1);
	GRAPH_SCALE_SetPos(hTrendScale, WM_GetWindowSizeY(hItem) - 10);

	trendOffsetX = 0;
	trendFactor = (localGraphMax - localGraphMin) / 178.0;
	trendPenSize = 2;
	trendFullscreenFlag = 0;
	for (int i = 0; i < HIST_DATA_SIZE; ++i)
		localTrendPoints[i].x = 360 - i;
}

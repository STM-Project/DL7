#include <channels.h>
#include "UserTrendsDLG.h"
#include "parameters.h"
#include "skins.h"
#include "UserMessage.h"
#include "WindowManagement.h"
#include "buzzer.h"
#include "SingleChannelDLG.h"
#include "fram.h"
#include "dtos.h"
#include "EDIT_double.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

#define ID_WINDOW_USER_TRENDS 		(GUI_ID_USER + 0x101)
#define ID_MULTIPAGE_USER_TRENDS 	(GUI_ID_USER + 0x102)
#define ID_WINDOW_USER_TREND 			(GUI_ID_USER + 0x110)
#define ID_GRAPH_USER_TRENDS 			(GUI_ID_USER + 0x111)
#define ID_MAXIMIZE_BUTTON 				(GUI_ID_USER + 0x112)
#define ID_USER_TRENDS_DESC 			(GUI_ID_USER + 0x113)
#define ID_USER_TRENDS_UNIT 			(GUI_ID_USER + 0x123)
#define ID_USER_TRENDS_VALUE			(GUI_ID_USER + 0x133)

#define ID_SLIDER_TREND 					(GUI_ID_USER + 0x143)
#define ID_BUTTON_TREND_UP 				(GUI_ID_USER + 0x144)
#define ID_BUTTON_TREND_DOWN 			(GUI_ID_USER + 0x145)

#define ID_TEXT_TREND_MAX				(GUI_ID_USER + 0x146)
#define ID_TEXT_THIRD_QUARTER		(GUI_ID_USER + 0x147)
#define ID_TEXT_HALF						(GUI_ID_USER + 0x148)
#define ID_TEXT_FIRST_QUARTER		(GUI_ID_USER + 0x149)
#define ID_TEXT_TREND_MIN				(GUI_ID_USER + 0x14A)
#define ID_TEXT_TREND_TIME_UNIT	(GUI_ID_USER + 0x14B)

#define DATA_C 6
#define GRAPH_FAIL_VALUE (I16P) -32000

#define MAX_TREND_POINTS 390
#define TREND_PEN_SIZE 4
#define USER_TREND_LINES 6

static short TrendLegend = 0;
static WM_HWIN CreateUserTrend();

static GUI_POINT userGraphPoints[6][HIST_DATA_SIZE]__attribute__ ((section(".sdram")));
static int instance = 7;
static float trendFactor = 3;
static GRAPH_DATA_Handle hLineData[6];
GRAPH_SCALE_Handle hTrendScale;
static WM_HWIN TrendWindow = 0;
static WM_HMEM hRefreshTimer;
static int trendOffsetX = 0;

static int UpdateLinePoints(GUI_POINT *data, HIST_DATA *hist, USER_TREND *trend)
{
	float *pHist = hist->head;
	GUI_POINT *pData = data;
	int i = 0;
	int pointsOnTrend = 0;

	if (trendOffsetX)
	{
		for (i = 0; i < trendOffsetX / 2; ++i)
		{
			if (pHist == &hist->data[0])
				pHist = &hist->data[HIST_DATA_SIZE - 1];
			else
				pHist--;
		}
	}

	pointsOnTrend = hist->numberOfItems - trendOffsetX / 2;

	if (pointsOnTrend <= 0)
		pointsOnTrend = 0;
	else if (pointsOnTrend > MAX_TREND_POINTS)
		pointsOnTrend = MAX_TREND_POINTS;

	for (i = 0; i < pointsOnTrend; ++i)
	{
		if(trend->min < trend->max)
		{
			if(*pHist < trend->min || *pHist == 0x7FFF)
				pData->y = (short)(trend->min/trendFactor) - 4;
			else if(*pHist > trend->max)
				pData->y = (short)(trend->max/trendFactor) + 4;
			else
				pData->y = (*pHist / trendFactor);
		}
		else
		{
			if(*pHist > trend->min || *pHist == 0x7FFF)
				pData->y = (short)(trend->min/trendFactor) - 4;
			else if(*pHist < trend->max)
				pData->y = (short)(trend->max/trendFactor) + 4;
			else
				pData->y = (*pHist / trendFactor);
		}

		if (pHist == &hist->data[0])
			pHist = &hist->data[HIST_DATA_SIZE - 1];
		else
			pHist--;
		pData++;
	}
	return pointsOnTrend;
}

static GRAPH_DATA_Handle RefreshTrend(GUI_POINT *data, USER_TREND *trend,int lineNo)
{
	return GRAPH_DATA_XY_Create(penColor[lineNo],MAX_TREND_POINTS,data,UpdateLinePoints(data,trend->pChannel[lineNo]->histData, trend));
}

static void GRAPH_Refresh(GRAPH_Handle hObj, USER_TREND *trend)
{
	for(int i = 0; i < USER_TREND_LINES; ++i)
	{
		GRAPH_DetachData(hObj, hLineData[i]);
		if(hLineData[i])
		{
			GRAPH_DATA_XY_Delete(hLineData[i]);
			hLineData[i] = 0;
		}

		if (NULL != trend->pChannel[i])
		{
			hLineData[i] = RefreshTrend(&userGraphPoints[i][0], trend, i);
			GRAPH_DATA_XY_SetPenSize(hLineData[i], TREND_PEN_SIZE);
			GRAPH_DATA_XY_SetOffY(hLineData[i], -trend->min / trendFactor);
			GRAPH_AttachData(hObj, hLineData[i]);
		}
	}
}

static void USER_GRAPH_Minimize(GRAPH_Handle hObj)
{
	WM_SetSize(hObj, 481, 330);
	GRAPH_SetVSizeX(hObj,481);

	GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX+481);

	TrendLegend = 1;

	for (int j = 0; j < USER_TREND_LINES; ++j)
		for (int i = 0; i < MAX_TREND_POINTS; ++i)
			userGraphPoints[j][i].x =480-2*i;
}

static void USER_GRAPH_Maximize(GRAPH_Handle hObj)
{
	WM_SetSize(hObj, 721, 330);
	GRAPH_SetVSizeX(hObj,721);

	GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX+721);

	TrendLegend = 0;

	for (int j = 0;j < USER_TREND_LINES;++j)
		for (int i= 0;i<MAX_TREND_POINTS;++i)
			userGraphPoints[j][i].x =720-2*i;
}

static const GUI_WIDGET_CREATE_INFO _aUserTrendsDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "Trendy", ID_WINDOW_USER_TRENDS, 0, 60, 740, 420, 0, 0x0, 0 },
				{ MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_USER_TRENDS, 5, 5, 730, 415, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSingleTrendDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "User_Graph", ID_WINDOW_USER_TREND, 0, 0, 730, 380, 0, 0x0, 0 },
				{ GRAPH_CreateIndirect, "Graph", ID_GRAPH_USER_TRENDS, 5, 3, 720, 332, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", ID_MAXIMIZE_BUTTON, 10, 10, 710, 310, WM_CF_HASTRANS, 0 },

				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_MAX, 10, 5, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_THIRD_QUARTER, 10, 68, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_HALF, 10, 146, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_FIRST_QUARTER, 10, 224, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_MIN, 10, 291, 150, 20, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_TREND_TIME_UNIT, 665, 315, 61, 24, GUI_TA_RIGHT | GUI_TA_BOTTOM, 0x64, 0 },

				{ SLIDER_CreateIndirect, "", ID_SLIDER_TREND, 96, 341, 540, 33, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "◀", ID_BUTTON_TREND_DOWN, 5, 341, 90, 35, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "▶", ID_BUTTON_TREND_UP, 640, 341, 90, 35, 0, 0x0, 0 },
		};


static void _cbUserTrendsDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	short i;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_TRENDS);
		TrendWindow = CreateUserTrend();
		MULTIPAGE_AddPage(hItem, TrendWindow, UserTrendsSettings[0].description);
		MULTIPAGE_AddPage(hItem, TrendWindow, UserTrendsSettings[1].description);
		MULTIPAGE_AddPage(hItem, TrendWindow, UserTrendsSettings[2].description);
		MULTIPAGE_AddPage(hItem, TrendWindow, UserTrendsSettings[3].description);
		MULTIPAGE_AddPage(hItem, TrendWindow, UserTrendsSettings[4].description);
		MULTIPAGE_AddPage(hItem, TrendWindow, UserTrendsSettings[5].description);
		MULTIPAGE_SetTabHeight(hItem, 35);

		for (i = 0; i < 6; ++i)
		{
			if(0 == UserTrendsSettings[i].isActive)
			{
				MULTIPAGE_SetTabWidth(hItem, 0, i);
				MULTIPAGE_DisablePage(hItem, i);
			}
		}


		if( MULTIPAGE_IsPageEnabled (hItem, instance))
			MULTIPAGE_SelectPage(hItem, instance);
		else
		{
			for(i = 0; i < 6; ++i)
			{
				if( MULTIPAGE_IsPageEnabled(hItem, i))
				{
					MULTIPAGE_SelectPage(hItem, i);
					instance = i;
					break;
				}
			}
			if(i==5)
			{
				MULTIPAGE_SelectPage(hItem, 0);
				instance = 0;
			}
			FRAM_Write(USER_TREND_SCREEN_ADDR, (uint8_t) instance);
		}
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_MULTIPAGE_USER_TRENDS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_TRENDS);
				UserMessage.MsgId = WM_USER_REFRESH;
				UserMessage.hWin = TrendWindow;
				UserMessage.hWinSrc = pMsg->hWin;
				UserMessage.Data.v = MULTIPAGE_GetSelection(hItem);
				WM_SendMessage(TrendWindow, &UserMessage);
				FRAM_Write(USER_TREND_SCREEN_ADDR, (uint8_t) UserMessage.Data.v);
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				instance = MULTIPAGE_GetSelection(pMsg->hWinSrc);
			}
			break;
		}
		break;
		case WM_TIMER:

			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_TRENDS);
			UserMessage.MsgId = WM_USER_REFRESH;
			UserMessage.hWin = TrendWindow;
			UserMessage.hWinSrc = pMsg->hWin;
			UserMessage.Data.v = MULTIPAGE_GetSelection(hItem);
			WM_SendMessage(TrendWindow, &UserMessage);

			WM_RestartTimer(pMsg->Data.v, 1000);
			break;
	case WM_DELETE:

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSingleTrendDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem, hGraph, hUnit, hDesc, hValue;
	int NCode;
	int Id;
	int i;
	float trendOYDiff = 0.0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_USER_TRENDS);
		GRAPH_SetGridVis(hItem, 1);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_GRID);
		GRAPH_SetLineStyle(hItem, GUI_LS_DASH);
		GRAPH_SetSkinColors(hItem);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_FRAME);
		GRAPH_SetAutoScrollbar(hItem,GUI_COORD_X,0);
		GRAPH_SetBorder(hItem, 1, 1, 1, 20);
		GRAPH_SetGridDistX(hItem, 60);
		GRAPH_SetGridDistY(hItem, 39);
		GRAPH_SetGridOffY(hItem,38);

		hTrendScale = GRAPH_SCALE_Create(9, GUI_TA_LEFT | GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, 120);
		GRAPH_SCALE_SetFont(hTrendScale, &GUI_FontLato30);
		GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX);
		GRAPH_SCALE_SetNumDecs(hTrendScale, 0);
		GRAPH_SCALE_SetTextColor(hTrendScale, GUI_BLUE);
		GRAPH_SCALE_SetPos(hTrendScale,WM_GetWindowSizeY(hItem)-10);
		GRAPH_SCALE_SetFactor(hTrendScale,0.5);
		GRAPH_AttachScale(hItem, hTrendScale);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
		SLIDER_SetSkinClassic(hItem);
		SLIDER_EnableFocusRect(hItem,0);
		SLIDER_SetBarColor(hItem, GUI_GRAY);
		SLIDER_SetNumTicks(hItem, 0);
		SLIDER_SetWidth(hItem, 80);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MAX);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_THIRD_QUARTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HALF);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FIRST_QUARTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MIN);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_TIME_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetBkColor(hItem, SKINS_GetBkColor());
		TEXT_SetText(hItem, "[s]");

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (NCode)
		{
		case WM_NOTIFICATION_RELEASED:
			switch (Id)
			{
			case ID_MAXIMIZE_BUTTON:
				BUZZER_Beep();
				if (!TrendLegend)
				{
					WM_SetSize(pMsg->hWinSrc, 470, 310);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
					WM_SetSize(hItem, 298, 32);
					SLIDER_SetRange(hItem, -112, 0);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_USER_TRENDS);
					USER_GRAPH_Minimize(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_TREND_UP);
					WM_MoveChildTo(hItem, 398, 341);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_TIME_UNIT);
					WM_MoveChildTo(hItem, 425, 315);
				}
				else
				{
					WM_SetSize(pMsg->hWinSrc, 710, 310);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
					WM_SetSize(hItem, 540, 32);
					if (SLIDER_GetValue(hItem) < -108)
						SLIDER_SetValue(hItem, -108);
					SLIDER_SetRange(hItem, -108, 0);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_USER_TRENDS);
					USER_GRAPH_Maximize(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_TREND_UP);
					WM_MoveChildTo(hItem, 640, 341);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_TIME_UNIT);
					WM_MoveChildTo(hItem, 665, 315);
				}

				WM_RestartTimer(hRefreshTimer, 50);
				break;
			case ID_BUTTON_TREND_UP:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
				SLIDER_Inc(hItem);
				break;
			case ID_BUTTON_TREND_DOWN:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_TREND);
				SLIDER_Dec(hItem);
				break;
			default:
				if ((Id >= ID_USER_TRENDS_VALUE) && (Id <= (ID_USER_TRENDS_VALUE + 5)))
				{
					i = Id - ID_USER_TRENDS_VALUE;
					if (UserTrendsSettings[instance].pChannel[i]->source.type)
					{
						BUZZER_Beep();
						WM_DeleteWindow(hActualWindow);
						hActualWindow = CreateSingleChannelWindow(UserTrendsSettings[instance].channelID[i]);
						WindowsState = SINGLE_VALUE;
						FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
						FRAM_Write(WINDOW_INSTANCE_ADDR, (uint8_t) WindowInstance);
					}
				}
				if ((Id >= ID_USER_TRENDS_UNIT) && (Id <= (ID_USER_TRENDS_UNIT + 5)))
				{
					i = Id - ID_USER_TRENDS_UNIT;
					if (UserTrendsSettings[instance].pChannel[i]->source.type)
					{
						BUZZER_Beep();
						WM_DeleteWindow(hActualWindow);
						hActualWindow = CreateSingleChannelWindow(UserTrendsSettings[instance].channelID[i]);
						WindowsState = SINGLE_VALUE;
						FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
						FRAM_Write(WINDOW_INSTANCE_ADDR, (uint8_t) WindowInstance);
					}
				}
				if ((Id >= ID_USER_TRENDS_DESC) && (Id <= (ID_USER_TRENDS_DESC + 5)))
				{
					i = Id - ID_USER_TRENDS_DESC;
					if (UserTrendsSettings[instance].pChannel[i]->source.type)
					{
						BUZZER_Beep();
						WM_DeleteWindow(hActualWindow);
						hActualWindow = CreateSingleChannelWindow(UserTrendsSettings[instance].channelID[i]);
						WindowsState = SINGLE_VALUE;
						FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
						FRAM_Write(WINDOW_INSTANCE_ADDR, (uint8_t) WindowInstance);
					}
				}
				break;
			}
			break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				switch (Id)
				{
				case ID_SLIDER_TREND:
					trendOffsetX = -60 * SLIDER_GetValue(pMsg->hWinSrc);
					if (!TrendLegend)
						GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX + 721);
					else
						GRAPH_SCALE_SetOff(hTrendScale, trendOffsetX + 481);
					break;
				}
				break;
		}
		break;
	case WM_DELETE:
		hGraph = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_USER_TRENDS);
		for (i = 0; i < USER_TREND_LINES; ++i)
		{
			GRAPH_DetachData(hGraph, hLineData[i]);
			if (hLineData[i])
			{
				GRAPH_DATA_XY_Delete(hLineData[i]);
				hLineData[i] = 0;
			}
		}
		break;
	case WM_USER_REFRESH:
		hGraph = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_USER_TRENDS);
		trendFactor = (UserTrendsSettings[pMsg->Data.v].max - UserTrendsSettings[pMsg->Data.v].min) / 309;

		GRAPH_Refresh(hGraph, &UserTrendsSettings[pMsg->Data.v]);

		for (i = 0; i < 6; ++i)
		{
			hDesc = WM_GetDialogItem(pMsg->hWin, ID_USER_TRENDS_DESC + i);
			hValue = WM_GetDialogItem(pMsg->hWin, ID_USER_TRENDS_VALUE + i);
			hUnit = WM_GetDialogItem(pMsg->hWin, ID_USER_TRENDS_UNIT + i);

			if (1 == TrendLegend && NULL != UserTrendsSettings[pMsg->Data.v].pChannel[i])
			{
				WM_ShowWindow(hDesc);
				WM_ShowWindow(hUnit);
				WM_ShowWindow(hValue);

				if (UserTrendsSettings[pMsg->Data.v].pChannel[i]->source.type)
				{
					TEXT_SetText(hDesc, UserTrendsSettings[pMsg->Data.v].pChannel[i]->description);
					TEXT_SetTextColor(hDesc, penColor[i]);

					TEXT_SetText(hUnit, UserTrendsSettings[pMsg->Data.v].pChannel[i]->unit);
					TEXT_SetTextColor(hUnit, penColor[i]);

					SetChannelValue(hValue, UserTrendsSettings[pMsg->Data.v].pChannel[i]);
					EDIT_SetTextColor(hValue, EDIT_CI_ENABLED, penColor[i]);
					if (UserTrendsSettings[pMsg->Data.v].pChannel[i]->failureMode == 1)
					{
						if (UserTrendsSettings[pMsg->Data.v].pChannel[i]->failureState == 1)
						{
							TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);
							TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
						}
						else
						{
							TEXT_SetBkColor(hDesc, GUI_YELLOW);
							TEXT_SetBkColor(hUnit, GUI_YELLOW);
						}
					}
					else
					{
						TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);
						TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
					}

				}
				else
				{
					EDIT_SetTextMode(hValue);
					EDIT_SetText(hValue, "-------");
					EDIT_SetSkin(hValue, EDIT_CI_ENABLED);
					EDIT_SetTextColor(hValue, EDIT_CI_ENABLED, GUI_GRAY);

					TEXT_SetText(hDesc, UserTrendsSettings[pMsg->Data.v].pChannel[i]->description);
					TEXT_SetTextColor(hDesc, GUI_GRAY);
					TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);

					TEXT_SetText(hUnit, UserTrendsSettings[pMsg->Data.v].pChannel[i]->unit);
					TEXT_SetTextColor(hUnit, GUI_GRAY);
					TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
				}
			}
			else
			{
				WM_HideWindow(hDesc);
				WM_HideWindow(hValue);
				WM_HideWindow(hUnit);
			}
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MAX);
		TEXT_SetFloat(hItem,UserTrendsSettings[pMsg->Data.v].max, 4);

		trendOYDiff = UserTrendsSettings[pMsg->Data.v].max - UserTrendsSettings[pMsg->Data.v].min;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_THIRD_QUARTER);
		TEXT_SetFloat(hItem, UserTrendsSettings[pMsg->Data.v].max - trendOYDiff/4, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HALF);
		TEXT_SetFloat(hItem,UserTrendsSettings[pMsg->Data.v].max - trendOYDiff / 2, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FIRST_QUARTER);
		TEXT_SetFloat(hItem,UserTrendsSettings[pMsg->Data.v].min + trendOYDiff / 4, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TREND_MIN);
		TEXT_SetFloat(hItem,UserTrendsSettings[pMsg->Data.v].min, 4);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateUserTrends(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aUserTrendsDialogCreate, GUI_COUNTOF(_aUserTrendsDialogCreate), _cbUserTrendsDialog, WM_HBKWIN, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(3));
	hRefreshTimer = WM_CreateTimer(hWin, 0, 50, 0);

	return hWin;
}

static WM_HWIN CreateUserTrend()
{
	WM_HWIN hWin, hItem;
	int i=0, j=0;
	hWin = GUI_CreateDialogBox(_aSingleTrendDialogCreate, GUI_COUNTOF(_aSingleTrendDialogCreate), _cbSingleTrendDialog, WM_HBKWIN, 0, 0);

	instance = FRAM_Read(USER_TREND_SCREEN_ADDR);
	if (instance < 0 || instance > 6)
		instance = 0;

	for (j = 0; j < 6; ++j)
	{
		hItem = TEXT_CreateEx(490, 3 + 62 * j, 240, 30, hWin, WM_CF_SHOW, GUI_TA_LEFT | GUI_TA_VCENTER, ID_USER_TRENDS_DESC + j, "");
		if (0 == TrendLegend)
			WM_HideWindow(hItem);

		hItem = TEXT_CreateEx(635, 33 + 62 * j, 95, 30, hWin, WM_CF_SHOW, GUI_TA_LEFT | GUI_TA_VCENTER, ID_USER_TRENDS_UNIT + j, "");
		if (0 == TrendLegend)
			WM_HideWindow(hItem);

		hItem = EDIT_CreateAsChild(490, 33 + 62 * j, 145, 30, hWin, ID_USER_TRENDS_VALUE + j, WM_CF_SHOW | EDIT_CI_DISABLED, 11);
		if (0 == TrendLegend)
			WM_HideWindow(hItem);
		EDIT_SetFocussable(hItem, 0);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);

		if (NULL != UserTrendsSettings[instance].pChannel[j])
		{
			if (UserTrendsSettings[instance].pChannel[j]->source.type)
			{
				hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_DESC + j);
				TEXT_SetText(hItem, UserTrendsSettings[instance].pChannel[j]->description);
				TEXT_SetTextColor(hItem, penColor[j]);

				hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_UNIT + j);
				TEXT_SetText(hItem, UserTrendsSettings[instance].pChannel[j]->unit);
				TEXT_SetTextColor(hItem, penColor[j]);

				hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_VALUE + j);
				SetChannelValue(hItem, UserTrendsSettings[instance].pChannel[j]);
				EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, penColor[j]);
			}
			else
			{
				hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_DESC + j);
				TEXT_SetText(hItem, UserTrendsSettings[instance].pChannel[j]->description);
				TEXT_SetTextColor(hItem, GUI_GRAY);

				hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_UNIT + j);
				TEXT_SetText(hItem, UserTrendsSettings[instance].pChannel[j]->unit);
				TEXT_SetTextColor(hItem, GUI_GRAY);

				hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_VALUE + j);
				EDIT_SetTextMode(hItem);
				EDIT_SetSkin(hItem, EDIT_CI_ENABLED);
				EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_GRAY);
				EDIT_SetText(hItem, "-------");
			}
		}
		else
		{
			hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_DESC + i);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_UNIT + i);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(hWin, ID_USER_TRENDS_VALUE + i);
			WM_HideWindow(hItem);
		}
	}

	if (TrendLegend)
	{
		hItem = WM_GetDialogItem(hWin, ID_GRAPH_USER_TRENDS);
		USER_GRAPH_Minimize(hItem);
		hItem = WM_GetDialogItem(hWin, ID_MAXIMIZE_BUTTON);
		WM_SetSize(hItem, 470, 310);
		hItem = WM_GetDialogItem(hWin, ID_SLIDER_TREND);
		WM_SetSize(hItem, 298, 32);
		SLIDER_SetRange(hItem, -112, 0);
		hItem = WM_GetDialogItem(hWin, ID_BUTTON_TREND_UP);
		WM_MoveChildTo(hItem, 398, 341);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_TIME_UNIT);
		WM_MoveChildTo(hItem, 425, 315);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_MAXIMIZE_BUTTON);
		WM_SetSize(hItem, 470, 310);
		hItem = WM_GetDialogItem(hWin, ID_GRAPH_USER_TRENDS);
		USER_GRAPH_Maximize(hItem);
		hItem = WM_GetDialogItem(hWin, ID_SLIDER_TREND);
		WM_SetSize(hItem, 540, 32);
		SLIDER_SetRange(hItem, -108, 0);
		hItem = WM_GetDialogItem(hWin, ID_BUTTON_TREND_UP);
		WM_MoveChildTo(hItem, 640, 341);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_TREND_TIME_UNIT);
		WM_MoveChildTo(hItem, 665, 315);

	}
	return hWin;
}

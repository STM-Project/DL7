/*
 * ArchiveReadDLG.c
 *
 *  Created on: 23.04.2021
 *      Author: TomaszSok
 */

#include "ArchiveReadDLG.h"
#include "archiveRead.h"
#include "skins.h"
#include "EDIT_double.h"
#include "draw.h"
#include "buzzer.h"
#include <string.h>
#include "UserMessage.h"
#include "ArchiveReadLegendDLG.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WIN_ARCHIVE_READ			(GUI_ID_USER + 300)
#define ID_BUTTON_CLOSE 				(GUI_ID_USER + 301)
#define ID_BUTTON_LEGEND 				(GUI_ID_USER + 302)
#define ID_GRAPH_ARCHIVE 				(GUI_ID_USER + 303)

#define ID_TEXT_START_PACKAGE		(GUI_ID_USER + 304)
#define ID_TEXT_END_PACKAGE			(GUI_ID_USER + 305)
#define ID_TEXT_ERROR						(GUI_ID_USER + 306)

#define ID_TEXT_MAX_Y						(GUI_ID_USER + 307)
#define ID_TEXT_MIN_Y						(GUI_ID_USER + 308)

#define ID_CURSOR_PREV 					(GUI_ID_USER + 309)
#define ID_CURSOR_NEXT 					(GUI_ID_USER + 310)
#define ID_PAGE_PREV 						(GUI_ID_USER + 311)
#define ID_PAGE_NEXT 						(GUI_ID_USER + 312)
#define ID_10_PAGE_PREV 				(GUI_ID_USER + 313)
#define ID_10_PAGE_NEXT 				(GUI_ID_USER + 314)

#define ID_ARCHIVE_TRENDS_DATE_TIME	(GUI_ID_USER + 315)
#define ID_ARCHIVE_TRENDS_VALUE			(GUI_ID_USER + 316)
#define ID_ARCHIVE_TRENDS_DESC			(GUI_ID_USER + 322)
#define ID_ARCHIVE_TRENDS_UNIT			(GUI_ID_USER + 328)

#define CURSOR_NEXT	1
#define CURSOR_PREV	2
#define PAGE_NEXT		3
#define PAGE_PREV		4

#define CURSOR_MIN_VALUE	0
#define CURSOR_MAX_VALUE	(NUMBER_OF_ARCHIVE_POINTS - 1)
#define CURSOR_SLOW_STEP	1
#define CURSOR_FAST_STEP	20
#define CURSOR_STEP_SWITCH_TIMER	5

static GRAPH_DATA_Handle hLineData[6];
static GRAPH_DATA_Handle hLineCursor;
static short cursorPosition = 0;
static int flagCursorTimer = 0;
static GUI_POINT cursorPoints[2] =
{
{ 0, 0 },
{ 0, TREND_Y_AXIS_SIZE } };
WM_HMEM hCursorTimer;

static const GUI_WIDGET_CREATE_INFO _aArchiveReadCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WIN_ARCHIVE_READ, 0, 0, 740, 420, 0, 0x0, 0 },

{ GRAPH_CreateIndirect, "", ID_GRAPH_ARCHIVE, 5, 5, 484, TREND_Y_AXIS_SIZE+5, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_START_PACKAGE, 10, 345, 215, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ERROR, 231, 345, 30, 30, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_END_PACKAGE, 269, 345, 215, 30, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "100%", ID_TEXT_MAX_Y, 7, 7, 55, 22, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "0%", ID_TEXT_MIN_Y, 7, 321, 34, 22, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ BUTTON_CreateIndirect, "◀◀  x10", ID_10_PAGE_PREV, 5, 377, 80, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "◀◀", ID_PAGE_PREV, 87, 377, 75, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "◀", ID_CURSOR_PREV, 164, 377, 75, 40, 0, 0x0, 0 },

{ BUTTON_CreateIndirect, "▶", ID_CURSOR_NEXT, 253, 377, 75, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "▶▶", ID_PAGE_NEXT, 330, 377, 75, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "x10 ▶▶", ID_10_PAGE_NEXT, 407, 377, 80, 40, 0, 0x0, 0 },

{ BUTTON_CreateIndirect, "", ID_BUTTON_LEGEND, 505, 377, 115, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CLOSE, 622, 377, 115, 40, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DATE_TIME, 490, 5, 237, 30, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_VALUE, 490, 60, 150, 25, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_VALUE + 1, 490, 110, 150, 25, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_VALUE + 2, 490, 160, 150, 25, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_VALUE + 3, 490, 210, 150, 25, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_VALUE + 4, 490, 260, 150, 25, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_VALUE + 5, 490, 310, 150, 25, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DESC, 490, 35, 247, 25, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DESC + 1, 490, 85, 247, 25, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DESC + 2, 490, 135, 247, 25, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DESC + 3, 490, 185, 247, 25, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DESC + 4, 490, 235, 247, 25, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_DESC + 5, 490, 285, 247, 25, 0, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_UNIT, 642, 60, 95, 25, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_UNIT + 1, 642, 110, 95, 25, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_UNIT + 2, 642, 160, 95, 25, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_UNIT + 3, 642, 210, 95, 25, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_UNIT + 4, 642, 260, 95, 25, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_ARCHIVE_TRENDS_UNIT + 5, 642, 310, 95, 25, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

};

static void CURSOR_UpdatePosition(GRAPH_Handle hGraph, short newPosition)
{
	GRAPH_DetachData(hGraph, hLineCursor);
	cursorPoints[0].x = cursorPosition;
	cursorPoints[1].x = cursorPoints[0].x;
	hLineCursor = GRAPH_DATA_XY_Create(SKINS_GetTextColor(), 2, cursorPoints, 2);
	GRAPH_DATA_XY_SetPenSize(hLineCursor, 1);
	GRAPH_AttachData(hGraph, hLineCursor);
}

static void CURSOR_CheckPosition(GRAPH_Handle hGraph)
{
	if ((numberOfRecords - 1) < cursorPosition)
	{
		cursorPosition = numberOfRecords - 1;
		CURSOR_UpdatePosition(hGraph, cursorPosition);
	}
}

static void ARCHIVE_READ_UpdateButtonsVisiblity(WM_HWIN hWin)
{

	WM_HWIN hItem;
	if(ArchiveFile->archiveEnd <= ArchiveFile->packageEnd)
	{
		hItem = WM_GetDialogItem(hWin, ID_PAGE_NEXT);
		WM_DisableWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_10_PAGE_NEXT);
		WM_DisableWindow(hItem);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_PAGE_NEXT);
		WM_EnableWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_10_PAGE_NEXT);
		WM_EnableWindow(hItem);
	}

	if(ArchiveFile->archiveStart == ArchiveFile->packageStart)
	{
		hItem = WM_GetDialogItem(hWin, ID_PAGE_PREV);
		WM_DisableWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_10_PAGE_PREV);
		WM_DisableWindow(hItem);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_PAGE_PREV);
		WM_EnableWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_10_PAGE_PREV);
		WM_EnableWindow(hItem);
	}
}

static void ARCHIVE_READ_UpdateLegend(WM_HWIN hWin)
{
	WM_HWIN hItem;
	char textBuffer[43] =
	{ 0 };
	strcpy(textBuffer, ArchiveFile->archiveRecords[cursorPosition].date);
	strcat(textBuffer, " ");
	strcat(textBuffer, ArchiveFile->archiveRecords[cursorPosition].time);
	strcat(textBuffer, "\0");
	hItem = WM_GetDialogItem(hWin, ID_ARCHIVE_TRENDS_DATE_TIME);
	TEXT_SetText(hItem, textBuffer);

	for (int i = 0; i < 6; ++i)
	{
		hItem = WM_GetDialogItem(hWin, ID_ARCHIVE_TRENDS_VALUE + i);
		if (0 <= ArchiveFile->channelsID[i])
		{
			switch(ArchiveFile->archiveRecords[cursorPosition].status[i])
			{
			case 0:
				TEXT_SetText(hItem, "-------");
				break;
			case 1:
				TEXT_SetFloatTirmmed(hItem, ArchiveFile->archiveRecords[cursorPosition].value[i], 4);
				break;
			case 0x0A:
				TEXT_SetText(hItem, "---W---");
				break;
			case 0x02:
				TEXT_SetText(hItem, "---||---");
				break;
			case 0x03:
				TEXT_SetText(hItem, "---E---");
				break;
			case 0x04:
				TEXT_SetText(hItem, "---R---");
				break;
			default:
				TEXT_SetText(hItem, "---ERR---");
				break;
			}
		}
	}
}

static void ARCHIVE_READ_UpdateGraph(WM_HWIN hWin)
{
	WM_HWIN hItem = WM_GetDialogItem(hWin, ID_GRAPH_ARCHIVE);
	for (int i = 0; i < 6; ++i)
	{
		if (0 <= ArchiveFile->channelsID[i])
		{
			GRAPH_DetachData(hItem, hLineData[i]);
			if (hLineData[i])
			{
				GRAPH_DATA_XY_Delete(hLineData[i]);
				hLineData[i] = 0;
			}
			hLineData[i] = GRAPH_DATA_XY_Create(penColor[i], NUMBER_OF_ARCHIVE_POINTS, archiveGraphPoints[i], numberOfRecords);
			GRAPH_DATA_XY_SetOffY(hLineData[i], trendOffsetY[i]);
			GRAPH_DATA_XY_SetLineStyle(hLineData[i], GUI_LS_SOLID);
			GRAPH_DATA_XY_SetPenSize(hLineData[i], 1);
			GRAPH_AttachData(hItem, hLineData[i]);
		}
	}
}

static void ARCHIVE_READ_UpdateAxisXRange(WM_HWIN hWin)
{
	char textBuffer[24]={0};
	WM_HWIN hItem = WM_GetDialogItem(hWin, ID_TEXT_START_PACKAGE);
	strcpy(textBuffer,"◀ ");
	strcat(textBuffer,ArchiveFile->archiveRecords[0].date);
	strcat(textBuffer," ");
	strcat(textBuffer,ArchiveFile->archiveRecords[0].time);
	TEXT_SetText(hItem, textBuffer);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_END_PACKAGE);
	if(NUMBER_OF_ARCHIVE_POINTS == numberOfRecords)
	{
		strcpy(textBuffer,ArchiveFile->archiveRecords[numberOfRecords-1].date);
		strcat(textBuffer," ");
		strcat(textBuffer,ArchiveFile->archiveRecords[numberOfRecords-1].time);
		strcat(textBuffer," ▶");
		TEXT_SetText(hItem, textBuffer);
	}
	else
	{
		TEXT_SetText(hItem, "----- ▶");
	}
}

static void ARCHIVE_READ_UpdateErrorStatus(WM_HWIN hWin)
{
	WM_HWIN hItem;
	if(0 == ArchiveFile->isErrorInPackage)
	{
		hItem = WM_GetDialogItem(hWin, ID_GRAPH_ARCHIVE);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_FRAME);
		GRAPH_SetColor(hItem, SKINS_GetBkColor(), GRAPH_CI_BORDER);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ERROR);
		WM_HideWindow(hItem);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_GRAPH_ARCHIVE);
		GRAPH_SetColor(hItem, GUI_RED, GRAPH_CI_FRAME);
		GRAPH_SetColor(hItem, GUI_RED, GRAPH_CI_BORDER);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ERROR);
		WM_ShowWindow(hItem);
	}
}


static void _cbArchiveRead(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		flagCursorTimer = 0;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLOSE);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 44, 7);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LEGEND);
		BUTTON_SetText(hItem, "Info");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MAX_Y);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetBkColor(hItem, SKINS_GetBkColor());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MIN_Y);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetBkColor(hItem, SKINS_GetBkColor());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERROR);
		WM_HideWindow(hItem);
		TEXT_SetText(hItem, "!");
		TEXT_SetBkColor(hItem, GUI_RED);
		TEXT_SetTextColor(hItem, GUI_WHITE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
		GRAPH_SetGridVis(hItem, 1);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_GRID);
		GRAPH_SetLineStyle(hItem, GUI_LS_DASH);
		GRAPH_SetSkinColors(hItem);
		GRAPH_SetBorder(hItem, 2, 2, 2, 2);
		GRAPH_SetColor(hItem, GUI_GRAY, GRAPH_CI_FRAME);
		GRAPH_SetColor(hItem, SKINS_GetBkColor(), GRAPH_CI_BORDER);
		GRAPH_SetAutoScrollbar(hItem, GUI_COORD_X, 0);
		GRAPH_SetGridDistX(hItem, 60);
		GRAPH_SetGridDistY(hItem, 40);
		GRAPH_SetGridOffY(hItem, 7);

		hLineCursor = GRAPH_DATA_XY_Create(SKINS_GetTextColor(), 2, cursorPoints, 2);
		GRAPH_DATA_XY_SetPenSize(hLineCursor, 1);
		GRAPH_AttachData(hItem, hLineCursor);

		cursorPosition = numberOfRecords - 1;
		CURSOR_UpdatePosition(hItem, cursorPosition);

		for (int i = 0; i < 6; ++i)
		{
			hLineData[i] = 0;
			if (0 <= ArchiveFile->channelsID[i])
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_TRENDS_VALUE + i);
				TEXT_SetTextColor(hItem, penColor[i]);
				WM_ShowWindow(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_TRENDS_DESC + i);
				TEXT_SetTextColor(hItem, penColor[i]);
				TEXT_SetText(hItem, ArchiveFile->recordsHead[i].name);
				WM_ShowWindow(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_TRENDS_UNIT + i);
				TEXT_SetTextColor(hItem, penColor[i]);
				TEXT_SetText(hItem, ArchiveFile->recordsHead[i].unit);
				WM_ShowWindow(hItem);
			}
			else
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_TRENDS_VALUE + i);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_TRENDS_DESC + i);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_TRENDS_UNIT + i);
				WM_HideWindow(hItem);
			}
		}

		WM_HWIN hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
		for (int i = 0; i < 6; ++i)
		{
			if (0 <= ArchiveFile->channelsID[i])
			{
				hLineData[i] = GRAPH_DATA_XY_Create(penColor[i], NUMBER_OF_ARCHIVE_POINTS, archiveGraphPoints[i], numberOfRecords);
				GRAPH_DATA_XY_SetOffY(hLineData[i], trendOffsetY[i]);
				GRAPH_DATA_XY_SetLineStyle(hLineData[i], GUI_LS_SOLID);
				GRAPH_DATA_XY_SetPenSize(hLineData[i], 1);
				GRAPH_AttachData(hItem, hLineData[i]);
			}
		}
		ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
		ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
		ARCHIVE_READ_UpdateLegend(pMsg->hWin);
		ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_CLOSE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_PAGE_PREV:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (ARCHIVE_READ_SelectPrevPackage(1))
				{
					ARCHIVE_ReadPackage();
					ARCHIVE_READ_UpdateGraph(pMsg->hWin);
					ARCHIVE_READ_UpdateLegend(pMsg->hWin);
					ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
					ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
					CURSOR_CheckPosition(hItem);
					ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
				}

				break;
			}
			break;
		case ID_10_PAGE_PREV:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (ARCHIVE_READ_SelectPrevPackage(10))
				{
					ARCHIVE_ReadPackage();
					ARCHIVE_READ_UpdateGraph(pMsg->hWin);
					ARCHIVE_READ_UpdateLegend(pMsg->hWin);
					ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
					ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
					CURSOR_CheckPosition(hItem);
					ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
				}

				break;
			}
			break;
		case ID_PAGE_NEXT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (ARCHIVE_READ_SelectNextPackage(1))
				{
					ARCHIVE_ReadPackage();
					ARCHIVE_READ_UpdateGraph(pMsg->hWin);
					ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
					ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
					CURSOR_CheckPosition(hItem);
					ARCHIVE_READ_UpdateLegend(pMsg->hWin);
					ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
				}

				break;
			}
			break;
		case ID_10_PAGE_NEXT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (ARCHIVE_READ_SelectNextPackage(10))
				{
					ARCHIVE_ReadPackage();
					ARCHIVE_READ_UpdateGraph(pMsg->hWin);
					ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
					ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
					CURSOR_CheckPosition(hItem);
					ARCHIVE_READ_UpdateLegend(pMsg->hWin);
					ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
				}
				break;
			}
			break;
		case ID_CURSOR_PREV:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hCursorTimer = WM_CreateTimer(pMsg->hWin, CURSOR_PREV, 1500, 0);
				break;
			case WM_NOTIFICATION_MOVED_OUT:
			case WM_NOTIFICATION_RELEASED:
				WM_DeleteTimer(hCursorTimer);

				if (0 < cursorPosition)
					cursorPosition-=CURSOR_SLOW_STEP;
				else
				{
					if (ARCHIVE_READ_SelectPrevPackage(1))
					{
						cursorPosition = CURSOR_MAX_VALUE;
						ARCHIVE_ReadPackage();
						ARCHIVE_READ_UpdateGraph(pMsg->hWin);
						ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
						ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
						hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
						CURSOR_CheckPosition(hItem);
						ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
					}
					else
						cursorPosition = CURSOR_MIN_VALUE;
				}
				hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
				CURSOR_UpdatePosition(hItem, cursorPosition);
				ARCHIVE_READ_UpdateLegend(pMsg->hWin);
				break;
			}
			break;
		case ID_CURSOR_NEXT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hCursorTimer = WM_CreateTimer(pMsg->hWin, CURSOR_NEXT, 1500, 0);
				break;
			case WM_NOTIFICATION_MOVED_OUT:
			case WM_NOTIFICATION_RELEASED:
				WM_DeleteTimer(hCursorTimer);
				if ((numberOfRecords - 1) > cursorPosition)
					cursorPosition+=CURSOR_SLOW_STEP;
				else
				{
					if (ARCHIVE_READ_SelectNextPackage(1))
					{
						cursorPosition = CURSOR_MIN_VALUE;
						ARCHIVE_ReadPackage();
						ARCHIVE_READ_UpdateGraph(pMsg->hWin);
						ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
						ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
						hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
						CURSOR_CheckPosition(hItem);
						ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
					}
					else
						cursorPosition = (numberOfRecords - 1);
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
				CURSOR_UpdatePosition(hItem, cursorPosition);
				ARCHIVE_READ_UpdateLegend(pMsg->hWin);
				break;
			}
			break;
			case ID_BUTTON_LEGEND:
				switch (NCode)
				{
				case WM_NOTIFICATION_CLICKED:
					BUZZER_Beep();
					break;
				case WM_NOTIFICATION_RELEASED:
					CreateArchiveReadLegend(pMsg->hWin);
					break;
				}
				break;
		}
		break;
	case WM_TIMER:
		Id = WM_GetTimerId(pMsg->Data.v);
		switch (Id)
		{
		case CURSOR_PREV:
			cursorPosition-=CURSOR_FAST_STEP;

			if (0 > cursorPosition)
			{
				if (ARCHIVE_READ_SelectPrevPackage(1))
				{
					cursorPosition = CURSOR_MAX_VALUE;
					ARCHIVE_ReadPackage();
					ARCHIVE_READ_UpdateGraph(pMsg->hWin);
					ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
					ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
					CURSOR_CheckPosition(hItem);
					ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
				}
				else
					cursorPosition = CURSOR_MIN_VALUE;
			}

			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
			CURSOR_UpdatePosition(hItem, cursorPosition);
			ARCHIVE_READ_UpdateLegend(pMsg->hWin);
			WM_RestartTimer(pMsg->Data.v, 750);

			break;
		case CURSOR_NEXT:
			cursorPosition+=CURSOR_FAST_STEP;
			if ((numberOfRecords - 1) < cursorPosition)
			{
				if (ARCHIVE_READ_SelectNextPackage(1))
				{
					cursorPosition = CURSOR_MIN_VALUE;
					ARCHIVE_ReadPackage();
					ARCHIVE_READ_UpdateGraph(pMsg->hWin);
					ARCHIVE_READ_UpdateAxisXRange(pMsg->hWin);
					ARCHIVE_READ_UpdateErrorStatus(pMsg->hWin);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
					CURSOR_CheckPosition(hItem);
					ARCHIVE_READ_UpdateButtonsVisiblity(pMsg->hWin);
				}
				else
					cursorPosition = (numberOfRecords - 1);
			}
			hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_ARCHIVE);
			CURSOR_UpdatePosition(hItem, cursorPosition);
			ARCHIVE_READ_UpdateLegend(pMsg->hWin);
			WM_RestartTimer(pMsg->Data.v, 750);
			break;
		}
		break;
		case WM_POST_PAINT:
			DRAW_Line(6,349,6,372,1,GUI_GRAY);
			DRAW_Line(487,349,487,372,1,GUI_GRAY);
			break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateArchiveRead(WM_HWIN hParent)
{
	return GUI_CreateDialogBox(_aArchiveReadCreate, GUI_COUNTOF(_aArchiveReadCreate), _cbArchiveRead, hParent, 0, 0);
}

/*
 * ArchiveReadLegendDLG.c
 *
 *  Created on: 23.08.2021
 *      Author: TomaszSokolowski
 */

#include "ArchiveReadLegendDLG.h"
#include "archiveRead.h"
#include "skins.h"
#include "draw.h"
#include "buzzer.h"
#include "dtos.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;
extern GUI_CONST_STORAGE GUI_BITMAP bmCLOSE;


#define ID_FRAMEWIN_ARCHIVE_READ_LEGEND  		(GUI_ID_USER + 400)
#define ID_LISTVIEW_ARCHIVE_READ_LEGEND			(GUI_ID_USER + 401)

static const GUI_WIDGET_CREATE_INFO _aArchiveReadLegendDialogCreate[] =
{
				{ FRAMEWIN_CreateIndirect, "Info_more", 				ID_FRAMEWIN_ARCHIVE_READ_LEGEND, 25, 20, 690, 320, 0, 0x0, 0 },
				{ LISTVIEW_CreateIndirect, "Listview", 					ID_LISTVIEW_ARCHIVE_READ_LEGEND, 20, 35, 650, 280, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", 										GUI_ID_CLOSE, 654, 0, 32, 34, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", 											GUI_ID_HELP, 0, 0, 654, 34, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
		};


static void _cbArchiveReadLegendDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char textBuffer[21]={0};

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetBarColor(hItem, 0, GUI_BLUE);
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_BLUE);
		FRAMEWIN_SetBorderSize(hItem, 1);
		FRAMEWIN_SetText(hItem, "");
		FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER);
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

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ARCHIVE_READ_LEGEND);
		LISTVIEW_AddColumn(hItem, 200, GUI_LANG_GetText(6), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 150, GUI_LANG_GetText(33), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 150, "0%", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 150, "100%", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 40);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		HEADER_SetTextColor(LISTVIEW_GetHeader(hItem), SKINS_GetTextColor());
		LISTVIEW_SetGridVis(hItem, 1);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		WM_DisableWindow(hItem);

		for(int i=0;i<6;i++)
		{
				if(-1 < ArchiveFile->channelsID[i])
				{
					LISTVIEW_SetItemText(hItem, 0, i, ArchiveFile->recordsHead[i].name);
					LISTVIEW_SetItemTextColor(hItem, 0, i, LISTVIEW_CI_UNSEL, penColor[i]);
					LISTVIEW_SetItemText(hItem, 1, i, ArchiveFile->recordsHead[i].unit);
					LISTVIEW_SetItemTextColor(hItem, 1, i, LISTVIEW_CI_UNSEL, penColor[i]);
					float2striTrimmed(textBuffer, ArchiveFile->recordsHead[i].trendMin, 6);
					LISTVIEW_SetItemText(hItem, 2, i, textBuffer);
					LISTVIEW_SetItemTextColor(hItem, 2, i, LISTVIEW_CI_UNSEL, penColor[i]);
					float2striTrimmed(textBuffer, ArchiveFile->recordsHead[i].trendMax, 6);
					LISTVIEW_SetItemText(hItem, 3, i, textBuffer);
					LISTVIEW_SetItemTextColor(hItem, 3, i, LISTVIEW_CI_UNSEL, penColor[i]);
				}
				else
				{
					LISTVIEW_SetItemText(hItem, 0, i, "");
					LISTVIEW_SetItemText(hItem, 1, i, "");
					LISTVIEW_SetItemText(hItem, 2, i, "");
					LISTVIEW_SetItemText(hItem, 3, i, "");
				}
		}

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
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
		}
		break;

	default:
		WM_DefaultProc(pMsg);
		break;
	}
}


void CreateArchiveReadLegend(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aArchiveReadLegendDialogCreate, GUI_COUNTOF(_aArchiveReadLegendDialogCreate),
			_cbArchiveReadLegendDialog, hParent, 0, 0);
	WM_MakeModal(hWin);
}

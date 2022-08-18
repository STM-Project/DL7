/*
 * ArchiveReadMenuDLG.c
 *
 *  Created on: 29.04.2021
 *      Author: TomaszSok
 */

#include "ArchiveReadMenuDLG.h"
#include "ArchiveReadDLG.h"
#include "archiveRead.h"
#include "skins.h"
#include "EDIT_double.h"
#include "draw.h"
#include "LISTVIEW_files.h"
#include "mini-printf.h"
#include "UserMessage.h"
#include "passwords.h"
#include "PopUpMessageDLG.h"
#include "fram.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WIN_ARCHIVE_READ_MENU	(GUI_ID_USER + 200)
#define ID_BUTTON_CLOSE 					(GUI_ID_USER + 201)
#define ID_BUTTON_READ_HEADER			(GUI_ID_USER + 202)
#define ID_BUTTON_VIEW_ARCHIVE		(GUI_ID_USER + 203)
#define ID_LISTVIEW_ARCHIVE_FILES	(GUI_ID_USER + 204)
#define ID_DROPDOWN_LINE_1				(GUI_ID_USER + 205)
#define ID_DROPDOWN_LINE_2 				(GUI_ID_USER + 206)
#define ID_DROPDOWN_LINE_3 				(GUI_ID_USER + 207)
#define ID_DROPDOWN_LINE_4 				(GUI_ID_USER + 208)
#define ID_DROPDOWN_LINE_5 				(GUI_ID_USER + 209)
#define ID_DROPDOWN_LINE_6 				(GUI_ID_USER + 210)
#define ID_TEXT_ARCHIVE_INFO			(GUI_ID_USER + 211)
#define ID_TEXT_ARCHIVE_INFO_DESC	(GUI_ID_USER + 212)

static const GUI_WIDGET_CREATE_INFO _aArchiveReadMenu[] =
{
{ WINDOW_CreateIndirect, "", ID_WIN_ARCHIVE_READ_MENU, 0, 0, 740, 420, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CLOSE, 577, 377, 160, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_READ_HEADER, 5, 377, 200, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_VIEW_ARCHIVE, 207, 377, 200, 40, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_ARCHIVE_FILES, 10, 10, 350, 355, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ARCHIVE_INFO_DESC, 400, 10, 160, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ARCHIVE_INFO, 570, 10, 160, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LINE_1, 420, 60, 300, 150, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LINE_2, 420, 110, 300, 150, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LINE_3, 420, 160, 300, 150, 0, 0x0, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LINE_4, 420, 210, 300, 150, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LINE_5, 420, 260, 300, 150, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LINE_6, 420, 310, 300, 150, 0, 0x0, 0 }

};

static void ARCHIVE_READ_UpdateChannelsList(WM_HWIN hWin)
{
	WM_HWIN hItem;
	char buffer[56] =	{ 0 };
	char archivePath[31] =	{ 0 };

	hItem = WM_GetDialogItem(hWin, ID_LISTVIEW_ARCHIVE_FILES);
	LISTVIEW_GetFileFullPath(hItem, archivePath);
	int readStatus = ARCHIVE_READ_ReadHeader(archivePath);
	if(0 == readStatus)
	{
		LISTVIEW_GetItemTextSorted(hItem, 1, LISTVIEW_GetSel(hItem), archivePath, 31);

		hItem = WM_GetDialogItem(hWin, ID_TEXT_ARCHIVE_INFO_DESC);
		WM_ShowWindow(hItem);

		hItem = WM_GetDialogItem(hWin, ID_TEXT_ARCHIVE_INFO);
		TEXT_SetText(hItem, archivePath);
		WM_ShowWindow(hItem);

		hItem = WM_GetDialogItem(hWin, ID_BUTTON_VIEW_ARCHIVE);
		WM_ShowWindow(hItem);

		for (int j = 0; j < 6; j++)
		{
			hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_LINE_1 + j);
			while (DROPDOWN_GetNumItems(hItem))
				DROPDOWN_DeleteItem(hItem, 0);
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		}

		for (int i = 0; i < ArchiveFile->numberOfChannels; i++)
		{
			mini_snprintf(buffer, 56, "%d. %s", channelHeader[i]->ID, channelHeader[i]->name);
			for (int j = 0; j < 6; j++)
			{
				hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_LINE_1 + j);
				DROPDOWN_AddString(hItem, buffer);
			}
		}

		for (int j = 0; j < 6; j++)
		{
			hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_LINE_1 + j);
			DROPDOWN_SetSel(hItem, selectedChannelsID[j]);
			WM_ShowWindow(hItem);
		}
	}
	else if(2 == readStatus)
	{
		CreateMessage(GUI_LANG_GetText(264), GUI_YELLOW, GUI_BLACK);
	}
	else
	{
		CreateMessage(GUI_LANG_GetText(265), GUI_YELLOW, GUI_BLACK);
	}
}

static int ARCHIVE_READ_UpdateChannelsSelection(WM_HWIN hWin)
{
	WM_HWIN hItem;
	int ret = 0;
	for (int j = 0; j < 6; j++)
	{
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_LINE_1 + j);
		selectedChannelsID[j] = (int8_t) DROPDOWN_GetSel(hItem);
		ret += selectedChannelsID[j];
		ArchiveFile->channelsID[j] = selectedChannelsID[j] - 1;
	}
	return ret;
}

static void HEADER_CallbackFileHeader(WM_MESSAGE * pMsg)
{
	int Notification;

	switch (pMsg->MsgId)
	{
	case WM_TOUCH:
		Notification = WM_NOTIFICATION_MOVED_OUT;
		WM_NotifyParent(pMsg->hWin, Notification);
		break;
	default:
		HEADER_Callback(pMsg);
	}
}

static void _cbArchiveReadMenu(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	int NCode;
	int Id;

	char archivePath[31] =
	{ 0 };

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_READ_HEADER);
		BUTTON_SetText(hItem, GUI_LANG_GetText(256));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_VIEW_ARCHIVE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(257));
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLOSE);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 67, 7);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ARCHIVE_FILES);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_BLUE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetHeaderHeight(hItem, 30);
		LISTVIEW_SetRowHeight(hItem, 30);
		LISTVIEW_AddColumn(hItem, 1, "", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 150, GUI_LANG_GetText(4), GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 160, GUI_LANG_GetText(274), GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_PopulateWithDataArchiveFiles(hItem, "0:/archive");
		LISTVIEW_SetCompareFunc(hItem, 1, LISTVIEW_CompareText);
		LISTVIEW_SetCompareFunc(hItem, 2, LISTVIEW_CompareText);
		LISTVIEW_EnableSort(hItem);
		LISTVIEW_SetSort(hItem, 2, 0);
		LISTVIEW_SetRBorder(hItem, 5);
		LISTVIEW_SetSelUnsorted(hItem, 1);
		LISTVIEW_SetSel(hItem, 0);

		HEADER_Handle hHeader = LISTVIEW_GetHeader(hItem);
		HEADER_GetNumItems(hHeader);
		WM_SetCallback(hHeader, HEADER_CallbackFileHeader);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE_INFO_DESC);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(159));
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE_INFO);
		WM_HideWindow(hItem);


		for (int j = 0; j < 3; j++)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_LINE_1 + j);
			DROPDOWN_SetTextSkin(hItem);
			DROPDOWN_SetAutoScroll(hItem, 1);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_LINE_4 + j);
			DROPDOWN_SetTextSkin(hItem);
			DROPDOWN_SetAutoScroll(hItem, 1);
			DROPDOWN_SetUpMode(hItem, 1);
			WM_HideWindow(hItem);
		}

		ARCHIVE_READ_UpdateChannelsList(pMsg->hWin);
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
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_VIEW_ARCHIVE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if(ARCHIVE_READ_UpdateChannelsSelection(pMsg->hWin))
				{
					ARCHIVE_READ_UpdateChannelsHeader();
					ARCHIVE_READ_SelectLastPackage();
					ARCHIVE_ReadPackage();
					if(numberOfRecords == 0)
					{
						CreateMessage(GUI_LANG_GetText(259), GUI_YELLOW, GUI_BLACK);
					}
					else
					{
						CreateArchiveRead(pMsg->hWin);
					}
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(260), GUI_YELLOW, GUI_BLACK);
				}
				break;
			}
			break;

		case ID_BUTTON_READ_HEADER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ARCHIVE_FILES);
				LISTVIEW_GetFileFullPath(hItem, archivePath);
				LISTVIEW_GetItemTextSorted(hItem, 1, LISTVIEW_GetSel(hItem), archivePath, 31);
				ARCHIVE_READ_UpdateChannelsList(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(5, 5, 359, 364, 4, 2, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_VIEW_ARCHIVE);
		if (WM_IsVisible(hItem))
		{
			for (int i = 0; i < 6; i++)
			{
				DRAW_Rect(375, 62 + 50 * i, 40, 28, 1, penColor[i], penColor[i]);
			}
		}
		break;
	case WM_DELETE:
		for (int j = 0; j < 6; j++)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_LINE_1 + j);
			selectedChannelsID[j] = DROPDOWN_GetSel(hItem);
		}
		FRAM_WriteMultiple(READ_ARCHIVE_SELECTION, (uint8_t*) selectedChannelsID, 6);
		ARCHIVE_READ_Deinitalize();
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateArchiveReadMenu(WM_HWIN hParent)
{
	ARCHIVE_READ_Initialize();
	return GUI_CreateDialogBox(_aArchiveReadMenu, GUI_COUNTOF(_aArchiveReadMenu), _cbArchiveReadMenu, hParent, 0, 0);
}

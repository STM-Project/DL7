/*
 * testDLG.c
 *
 *  Created on: 11 wrz 2015
 *      Author: Tomaszs
 */

#include "PopUpMessageDLG.h"
#include "buzzer.h"
#include "draw.h"
#include "skins.h"
#include "rtc.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define MESSAGE_BUFFER_SIZE 10

#define ID_POPUP_WINDOW     	(GUI_ID_USER + 0x20)
#define ID_POPUP_TEXT    			(GUI_ID_USER + 0x21)
#define ID_POPUP_ACCEPT 			(GUI_ID_USER + 0x22)
#define ID_POPUP_CANCEL 			(GUI_ID_USER + 0x23)

#define ID_POPUP_DATE 				(GUI_ID_USER + 0x24)

#define ID_BACKGROUND 				(GUI_ID_USER + 0x25)

static WM_HWIN hWinBk=0;

static WM_HWIN hWinMessage[MESSAGE_BUFFER_SIZE] = {0};
static WM_HWIN hWinBackground[MESSAGE_BUFFER_SIZE] = {0};
static uint8_t WinMessageIdx = 0;

static MESSAGE_FUNCTION *ackFun=NULL;
static MESSAGE_FUNCTION *cancelFun=NULL;

static const GUI_WIDGET_CREATE_INFO _aPopUpMessageCreate[] =
{
		{ WINDOW_CreateIndirect, "", ID_POPUP_WINDOW, 0, 170, 800, 140, 0, 0, 0 },
		{ TEXT_CreateIndirect, "", ID_POPUP_DATE, 10, 5, 200, 30, TEXT_CF_LEFT | TEXT_CF_VCENTER, 0, 0 },
		{ TEXT_CreateIndirect, "", ID_POPUP_TEXT, 20, 35, 760, 60, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0 },
		{ BUTTON_CreateIndirect, "", ID_POPUP_ACCEPT, 638, 95, 160, 40, 0, 0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aPopUpConfirmMessageCreate[] =
{
		{ WINDOW_CreateIndirect, "", ID_POPUP_WINDOW, 0, 170, 800, 140, 0, 0, 0 },
		{ TEXT_CreateIndirect, "", ID_POPUP_TEXT, 20, 5, 760, 90, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0 },
		{ BUTTON_CreateIndirect, "", ID_POPUP_CANCEL, 638, 95, 160, 40, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0 },
		{ BUTTON_CreateIndirect, "", ID_POPUP_ACCEPT, 476, 95, 160, 40, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0 },
};

static void _cbPopUpMessage(WM_MESSAGE * pMsg)
{
	int NCode;
	int Id;

	WM_HWIN hItem;

	char textBuffer[20];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_POPUP_ACCEPT);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 61, 7);

		RTC_GetTimeAndDate(&RTCTime, &RTCDate);
		mini_snprintf(textBuffer,20,"%02d-%02d-%02d %02d:%02d:%02d",RTCDate.Year,RTCDate.Month,RTCDate.Date,RTCTime.Hours,RTCTime.Minutes,RTCTime.Seconds);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_POPUP_DATE);
		TEXT_SetText(hItem, textBuffer);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_POPUP_ACCEPT:
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
		}
		break;
	case WM_POST_PAINT:
		DRAW_Line(0,1,800,1,3,GUI_BLUE);
		DRAW_Line(0,138,800,138,3,GUI_BLUE);
		break;
	case WM_DELETE:
		for (int i = 0; i < MESSAGE_BUFFER_SIZE; ++i)
		{
			if(pMsg->hWin == hWinMessage[i])
			{
				WM_DeleteWindow(hWinBackground[i]);
				hWinBackground[i] = 0;
				hWinMessage[i] = 0;
				break;
			}
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbPopUpConfirmMessage(WM_MESSAGE * pMsg)
{
	int NCode;
	int Id;

	WM_HWIN hItem;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_POPUP_ACCEPT);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 61, 7);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_POPUP_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 67, 7);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_POPUP_ACCEPT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				if(ackFun)
					ackFun();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
			case ID_POPUP_CANCEL:
				switch (NCode)
				{
				case WM_NOTIFICATION_CLICKED:
					WM_MakeModal(0);
					break;
				case WM_NOTIFICATION_RELEASED:
					BUZZER_Beep();
					if(cancelFun)
						cancelFun();
					WM_DeleteWindow(pMsg->hWin);
					break;
				}
				break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_Line(0,1,800,1,3,GUI_BLUE);
		DRAW_Line(0,138,800,138,3,GUI_BLUE);
		break;
	case WM_DELETE:
		if(hWinBk)
		{
			WM_DeleteWindow(hWinBk);
			hWinBk = 0;
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

void CreateMessage(const char * pText, GUI_COLOR bkColor, GUI_COLOR textColor)
{
	WM_HWIN hItem;

	if(hWinMessage[WinMessageIdx])
	{
		WM_DeleteWindow(hWinMessage[WinMessageIdx]);
		hWinMessage[WinMessageIdx] = 0;
	}

	if(0 == hWinBackground[WinMessageIdx])
		hWinBackground[WinMessageIdx] = IMAGE_CreateEx(0,0,800,480,WM_HBKWIN, WM_CF_SHOW | WM_CF_HASTRANS,0,ID_BACKGROUND);

	hWinMessage[WinMessageIdx] = GUI_CreateDialogBox(_aPopUpMessageCreate, GUI_COUNTOF(_aPopUpMessageCreate), _cbPopUpMessage, WM_HBKWIN, 0, 0);
	WM_SetStayOnTop(hWinMessage[WinMessageIdx], 1);

	if(DEFAULT_COLOR != bkColor)
		WINDOW_SetBkColor(hWinMessage[WinMessageIdx], bkColor);
	else
		WINDOW_SetBkColor(hWinMessage[WinMessageIdx], SKINS_GetBkColor());

	hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_TEXT);
	TEXT_SetText(hItem, pText);

	if(DEFAULT_COLOR != bkColor)
	{
		hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_DATE);
		TEXT_SetTextColor(hItem,textColor);

		hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_TEXT);
		TEXT_SetTextColor(hItem,textColor);

		hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_ACCEPT);
		TEXT_SetTextColor(hItem,textColor);

	}
	else
	{
		hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_DATE);
		TEXT_SetTextColor(hItem,SKINS_GetTextColor());

		hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_TEXT);
		TEXT_SetTextColor(hItem,SKINS_GetTextColor());

		hItem = WM_GetDialogItem(hWinMessage[WinMessageIdx], ID_POPUP_ACCEPT);
		TEXT_SetTextColor(hItem,SKINS_GetTextColor());
	}

	if(++WinMessageIdx >= MESSAGE_BUFFER_SIZE)
		WinMessageIdx = 0;

}

WM_HWIN CreateConfirmMessage(const char * pText, GUI_COLOR bkColor, GUI_COLOR textColor, MESSAGE_FUNCTION *accept,MESSAGE_FUNCTION *cancel)
{
	ackFun = NULL;
	cancelFun = NULL;

	ackFun = accept;
	cancelFun = cancel;

	if(0==hWinBk)
		hWinBk = IMAGE_CreateEx(0,0,800,480,WM_HBKWIN, WM_CF_SHOW | WM_CF_HASTRANS,0,ID_BACKGROUND);

	WM_HWIN hWinMess = GUI_CreateDialogBox(_aPopUpConfirmMessageCreate, GUI_COUNTOF(_aPopUpConfirmMessageCreate), _cbPopUpConfirmMessage, WM_HBKWIN, 0, 0);
	WM_SetStayOnTop(hWinMess, 1);

	if(DEFAULT_COLOR != bkColor)
		WINDOW_SetBkColor(hWinMess, bkColor);
	else
		WINDOW_SetBkColor(hWinMess, SKINS_GetBkColor());

	WM_HWIN hItem = WM_GetDialogItem(hWinMess, ID_POPUP_TEXT);
	if(DEFAULT_COLOR != bkColor)
		TEXT_SetTextColor(hItem,textColor);
	else
		TEXT_SetTextColor(hItem,SKINS_GetTextColor());
	TEXT_SetText(hItem, pText);

	return hWinMess;
}

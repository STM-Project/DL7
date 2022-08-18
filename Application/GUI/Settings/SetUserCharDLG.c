/*
 * UserCharWin.c
 *
 *  Created on: 05.07.2017
 *      Author: MagdalenaRyb
 */

#include <channels.h>
#include <ExpansionCards.h>
#include "SetChannelDLG.h"
#include <string.h>
#include <stdlib.h>
#include <SetUserCharDLG.h>
#include "UserChar.h"
#include "dtos.h"
#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "parameters.h"
#include "ExpansionCards_descriptions.h"
#include "mini-printf.h"
#include "EDIT_double.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"

#include "DIALOG.h"
#include "SetConfirmDLG.h"

#include "math.h"

#define ID_WINDOW_0  (GUI_ID_USER + 0x00)
#define ID_EDIT_0  (GUI_ID_USER + 0x01)
#define ID_BUTTON_ADD  (GUI_ID_USER + 0x02)
#define ID_LISTVIEW_0  (GUI_ID_USER + 0x0B)
#define ID_TEXT_0  (GUI_ID_USER + 0x0C)
#define ID_BUTTON_OK  (GUI_ID_USER + 0x0D)
#define ID_BUTTON_CANCEL  (GUI_ID_USER + 0x0E)
#define ID_BUTTON_REMOVE  (GUI_ID_USER + 0x08)
#define ID_BUTTON_EDIT  (GUI_ID_USER + 0x09)

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

extern USER_CHAR tempUserCharacteristics[10];

int currCharacteristic;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
		{WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 740, 420, 0, 0x0, 0 },
		{ EDIT_CreateIndirect, "Edit", ID_EDIT_0, 159, 10, 333, 35, GUI_TA_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
		{ LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 15, 65, 474, 288, 0, 0x0, 0 },
		{ TEXT_CreateIndirect, "NAME", ID_TEXT_0, 0, 10, 145, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x0, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 496, 360, 120, 58, 0,0x0, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 618,360, 120, 58, 0, 0x0, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_ADD, 496, 60, 242, 58, 0, 0x0, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_REMOVE, 496, 120,242, 58, 0, 0x0, 0 },
		{ BUTTON_CreateIndirect, "",ID_BUTTON_EDIT, 496, 180, 242, 58, 0, 0x0, 0 },
		};

int              LISTVIEW_CompareFloat          (const void * p0, const void * p1) {
	if (atoff(p0)>atoff(p1))
		return -1;
	else
		return 1;
}

static void _cbHeader(WM_MESSAGE * pMsg) {
  int Notification;

  switch (pMsg->MsgId) {
  case WM_TOUCH:
      Notification = WM_NOTIFICATION_MOVED_OUT;
      WM_NotifyParent(pMsg->hWin, Notification);
      break;
  default:
    HEADER_Callback(pMsg);
  }
}

static uint8_t CheckCorectness(WM_HWIN hListview, WM_HWIN hEdit);

static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		hItem = pMsg->hWin;
		WM_SetStayOnTop(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
		EDIT_SetMaxLen(hItem, 20);
		EDIT_SetText(hItem, tempUserCharacteristics[currCharacteristic].name);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		LISTVIEW_AddColumn(hItem, 237, "x", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 237, "y", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_WHITE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_DARKGRAY);
		LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_SEL, GUI_WHITE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_DARKGRAY);
		LISTVIEW_SetCompareFunc(hItem, 0, LISTVIEW_CompareFloat);
		LISTVIEW_SetSort(hItem, 0, 1);
		LISTVIEW_EnableSort(hItem);

		char buffer[18];
		for (int i = 0; i<tempUserCharacteristics[currCharacteristic].numberOfPoints; i++)
		{
			LISTVIEW_AddRow(hItem, NULL);
			float2stri(buffer, tempUserCharacteristics[currCharacteristic].pointsX[i], 5);
			LISTVIEW_SetItemText(hItem, 0, i, buffer);
			float2stri(buffer, tempUserCharacteristics[currCharacteristic].pointsY[i], 5);
			LISTVIEW_SetItemText(hItem, 1, i, buffer);
		}
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_EnableCellSelect(hItem, 1);

		HEADER_SetBkColor(LISTVIEW_GetHeader(hItem), GUI_BLACK);
		HEADER_Handle hHeader = LISTVIEW_GetHeader(hItem);
		HEADER_GetNumItems(hHeader);
		WM_SetCallback(hHeader, _cbHeader);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		BUTTON_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD), BUTTON_CI_DISABLED, GUI_WHITE);
		BUTTON_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE), BUTTON_CI_DISABLED, GUI_WHITE);
		BUTTON_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT), BUTTON_CI_DISABLED, GUI_WHITE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetText(hItem, GUI_LANG_GetText(23));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(192));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(193));
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT);
		BUTTON_SetText(hItem, GUI_LANG_GetText(194));
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		if (LISTVIEW_GetNumRows(hItem) > 0) {
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT));
		}
		if (LISTVIEW_GetNumRows(hItem) >= 100) {
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD));
		}

		 if (PASSWORDS_GetCurrentLevel() <= USER_LEVEL)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
				WM_DisableWindow(hItem);
			}
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;

		switch (Id) {
		case ID_EDIT_0:
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (EDIT_GetBkColor(pMsg->hWinSrc, EDIT_CI_ENABLED) == GUI_RED)
					EDIT_SetBkColor(pMsg->hWinSrc, EDIT_CI_ENABLED, EDIT_GetDefaultBkColor(EDIT_CI_ENABLED));
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 20);
				break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				break;
			}
			break;
		case ID_BUTTON_ADD:
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
				if (LISTVIEW_GetBkColor(hItem, LISTVIEW_CI_UNSEL) == GUI_RED)
					LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_UNSEL, LISTVIEW_GetBkColor(hItem, LISTVIEW_CI_DISABLED));
				GUI_ConstString string[3] = {"0.00000",  "0.00000", NULL};
				LISTVIEW_AddRow(hItem, string);
				char bufferX[17];
				char bufferY[17];
				for (int i = 0; i < LISTVIEW_GetNumRows(hItem); i++) {
					LISTVIEW_GetItemTextSorted(hItem, 0, i, bufferX, 17);
					LISTVIEW_GetItemTextSorted(hItem, 1, i, bufferY, 17);
					if (strcmp(bufferX, "0.00000") == 0 && strcmp(bufferY, "0.00000") == 0) {
						LISTVIEW_EnableCellSelect(hItem, 0);
						LISTVIEW_SetSel(hItem, i);
						break;
					}
				}
				break;
			}
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
			if (LISTVIEW_GetNumRows(hItem) > 0)
			{
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE));
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT));
			}
			if (LISTVIEW_GetNumRows(hItem) >= 100)
			{
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD));
			}
			break;
		case ID_LISTVIEW_0:
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
				LISTVIEW_EnableCellSelect(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0) ,1);
				LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_DARKGRAY);
				break;
			case WM_NOTIFICATION_RELEASED:
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				break;
			}

			break;
		case ID_BUTTON_OK:
			switch (NCode) {
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if(CheckCorectness(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),WM_GetDialogItem(pMsg->hWin, ID_EDIT_0)))
				{
					LISTVIEW_EnableCellSelect(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0) ,1);

					hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
					WM_HWIN hEdit = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
					uint8_t num = LISTVIEW_GetNumRows(hItem);
					tempUserCharacteristics[currCharacteristic].numberOfPoints = num;
					EDIT_GetText(hEdit, tempUserCharacteristics[currCharacteristic].name, 20);
					char buffer[17];
					for(int i = 0; i < num; i++)
					{
							LISTVIEW_GetItemTextSorted(hItem, 0, i, buffer, 17);
							tempUserCharacteristics[currCharacteristic].pointsX[i] = atoff(buffer);
							LISTVIEW_GetItemTextSorted(hItem, 1, i, buffer, 17);
							tempUserCharacteristics[currCharacteristic].pointsY[i] = atoff(buffer);
					}
					NewUserChar = 1;
					WM_DeleteWindow(pMsg->hWin);
					UserMessage.MsgId = WM_REFRESH_CHAR;
					UserMessage.hWin = 0;
					UserMessage.hWinSrc = 0;
					UserMessage.Data.v= currCharacteristic;
					WM_BroadcastMessage(&UserMessage);
				}
				break;
			}
			break;
		case ID_BUTTON_CANCEL:
			switch (NCode) {
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CopyUserCharSettings(tempUserCharacteristics, bkUserCharacteristics);
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_REMOVE:
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:

				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
				LISTVIEW_DeleteRow(hItem, LISTVIEW_GetSelUnsorted(hItem));
				WM_InvalidateWindow(hItem);
				break;
			}
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
			if (LISTVIEW_GetNumRows(hItem) <= 0) {
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE));
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT));
			}
			if (LISTVIEW_GetNumRows(hItem) < 100) {
				WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD));
			}
			break;
		case ID_BUTTON_EDIT:
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
				int a = LISTVIEW_GetSel(hItem);
				int b = LISTVIEW_GetSelCol(hItem);
				if (a >= 0 && (b== 0 || b==1)) {
					setNumKeyboard(hItem, LISTVIEW, 15);
				}
				break;
			case WM_NOTIFICATION_RELEASED:
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		DRAW_RoundedFrame(10, 60, 483, 297, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(155, 7, 338, 40, 4, 2, GUI_BLUE);
		break;
	 case WM_USER_LOGIN:
		 if (PASSWORDS_GetCurrentLevel() <= USER_LEVEL)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EDIT);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REMOVE);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADD);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
			}
		 break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateWindowUserChar(WM_HWIN hParent, int characteristic) {
	WM_HWIN hWin;
	currCharacteristic = characteristic;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate),_cbDialog, hParent, 0, 0);
	return hWin;
}

static uint8_t CheckCorectness(WM_HWIN hListview, WM_HWIN hEdit) {
	char checkBuffer[17];
	char tempCheckBuffer[17];
	float value, tempValue;
	uint8_t flag = 1;
	uint8_t row, col, maxRow, tempRow;
	LISTVIEW_EnableCellSelect(hListview, 0);
	maxRow = LISTVIEW_GetNumRows(hListview);
	if (maxRow < 2) {
		flag = 0;
		LISTVIEW_SetBkColor(hListview,LISTVIEW_CI_UNSEL, GUI_RED);
	}
	if(EDIT_GetNumChars(hEdit)<=1) {
		EDIT_SetBkColor(hEdit, EDIT_CI_ENABLED, GUI_RED);
		flag = 0;
	}

	for(col=0; col<2; col++) {
		for (row = 0; row < maxRow; row++) {
			LISTVIEW_GetItemTextSorted(hListview, col, row, checkBuffer, 17);
			for (int i = 0; i < 17; i++) {
				if (checkBuffer[i] == 195 || checkBuffer[i] == 61 || (i>0 && (checkBuffer[i]==43 || checkBuffer[i]==45))) {
					LISTVIEW_SetBkColor(hListview, LISTVIEW_CI_SEL, GUI_RED);
					LISTVIEW_SetSel(hListview, row);
					flag = 0;
					return flag;
				}
			}
		}
	}

	for (row = 0; row<maxRow; row++) {
		LISTVIEW_GetItemTextSorted(hListview, 0, row, checkBuffer, 17);
		value = atoff(checkBuffer);
		float2stri(checkBuffer, value, 5);
		for (tempRow = row+1; tempRow<maxRow; tempRow++) {
			LISTVIEW_GetItemTextSorted(hListview, 0, tempRow, tempCheckBuffer, 17);
			tempValue = atoff(tempCheckBuffer);
			float2stri(tempCheckBuffer, tempValue, 5);
			if(strcmp(checkBuffer, tempCheckBuffer)==0) {
				LISTVIEW_SetBkColor(hListview, LISTVIEW_CI_SEL, GUI_RED);
				LISTVIEW_SetSel(hListview, tempRow);
				flag = 0;
				return flag;
			}
		}
	}
	return flag;
}


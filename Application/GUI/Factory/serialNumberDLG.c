/*
 * serialNumberDLG.c
 *
 *  Created on: 14 paź 2015
 *      Author: Tomaszs
 */

#include <NSMAC.h>
#include "serialNumberDLG.h"
#include <string.h>
#include <stdlib.h>
#include "draw.h"
#include "KeyboardDLG.h"
#include "PopUpMessageDLG.h"
#include "buzzer.h"
#include "mini-printf.h"

#include "passwords.h"
#include "UserMessage.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;

#define ID_WINDOW_SERIAL_NUMBER_AND_MAC   (GUI_ID_USER + 0x301)
#define ID_BUTTON_EXIT       							(GUI_ID_USER + 0x302)
#define ID_BUTTON_SAVE_TO_FLASH       		(GUI_ID_USER + 0x303)
#define ID_TEXT_SERIAL_NUMBER         		(GUI_ID_USER + 0x304)
#define ID_TEXT_MAC         							(GUI_ID_USER + 0x305)
#define ID_EDIT_SERIAL_NUMBER							(GUI_ID_USER + 0x306)
#define ID_EDIT_MAC_1        							(GUI_ID_USER + 0x307)
#define ID_EDIT_MAC_2        							(GUI_ID_USER + 0x308)
#define ID_EDIT_MAC_3        							(GUI_ID_USER + 0x309)
#define ID_EDIT_MAC_4        							(GUI_ID_USER + 0x30A)
#define ID_EDIT_MAC_5        							(GUI_ID_USER + 0x30B)
#define ID_EDIT_MAC_6        							(GUI_ID_USER + 0x30C)

const char cset[] = "1234567890ABCDEFabcdef";

char GetNewMACAddress(WM_HWIN hWin, unsigned char *NewMACAddress)
{
	WM_HWIN hItem;
	char GUITextBuffer[3];

	for (int i = 0; i < 6; i++)
	{
		hItem = WM_GetDialogItem(hWin, ID_EDIT_MAC_1 + i);
		if (EDIT_GetNumChars(hItem) == 2)
		{
			EDIT_GetText(hItem, GUITextBuffer, 3);
			if (strspn(GUITextBuffer, cset) == 2)
			{
				NewMACAddress[i] = strtoul(GUITextBuffer,NULL,16);
			}
			else
			{
				CreateMessage("ERROR", GUI_YELLOW, GUI_BLACK);
				EDIT_SetText(hItem, "");
				return 0;
			}
		}
		else
		{
			CreateMessage("WRONG VALUE", GUI_YELLOW, GUI_BLACK);
			return 0;
		}
	}
	return 1;
}

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "", ID_WINDOW_SERIAL_NUMBER_AND_MAC, 0, 0, 740, 420, 0, 0x0, 0 },

				{ TEXT_CreateIndirect, "", ID_TEXT_SERIAL_NUMBER, 10, 10, 250, 35, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", ID_TEXT_MAC, 10, 55, 250, 35, 0, 0x0, 0 },

				{ EDIT_CreateIndirect, "", ID_EDIT_SERIAL_NUMBER, 265, 10, 160, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_MAC_1, 265, 55, 40, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_MAC_2, 325, 55, 40, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_MAC_3, 385, 55, 40, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_MAC_4, 445, 55, 40, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_MAC_5, 505, 55, 40, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
				{ EDIT_CreateIndirect, "", ID_EDIT_MAC_6, 565, 55, 40, 35, EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },

				{ BUTTON_CreateIndirect, "", ID_BUTTON_EXIT, 618, 360, 120, 58, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", ID_BUTTON_SAVE_TO_FLASH, 260, 98, 170, 38, 0, 0x0, 0 },
		};

void _cbDialogSerialNumber(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	unsigned char tempMACAddress[6];

	unsigned int tempSerialNumber = 0;

	char GUITextBuffer[10];

	switch (pMsg->MsgId)
	{
	case WM_POST_PAINT:
		DRAW_RoundedFrame(260, 8, 170, 38, 4, 2, GUI_BLUE);

		DRAW_RoundedFrame(260, 53, 50, 38, 4, 2, GUI_BLUE);
		DRAW_Text(":", 315, 55, GUI_BLACK, GUI_TRANSPARENT, &GUI_FontLato30);
		DRAW_RoundedFrame(320, 53, 50, 38, 4, 2, GUI_BLUE);
		DRAW_Text(":", 375, 55, GUI_BLACK, GUI_TRANSPARENT, &GUI_FontLato30);
		DRAW_RoundedFrame(380, 53, 50, 38, 4, 2, GUI_BLUE);
		DRAW_Text(":", 435, 55, GUI_BLACK, GUI_TRANSPARENT, &GUI_FontLato30);
		DRAW_RoundedFrame(440, 53, 50, 38, 4, 2, GUI_BLUE);
		DRAW_Text(":", 495, 55, GUI_BLACK, GUI_TRANSPARENT, &GUI_FontLato30);
		DRAW_RoundedFrame(500, 53, 50, 38, 4, 2, GUI_BLUE);
		DRAW_Text(":", 555, 55, GUI_BLACK, GUI_TRANSPARENT, &GUI_FontLato30);
		DRAW_RoundedFrame(560, 53, 50, 38, 4, 2, GUI_BLUE);

		break;
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERIAL_NUMBER);
		TEXT_SetText(hItem, GUI_LANG_GetText(77));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MAC);
		TEXT_SetText(hItem, GUI_LANG_GetText(199));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERIAL_NUMBER);

		mini_snprintf(GUITextBuffer, 9, "%u", GetSerialNumber());
		EDIT_SetText(hItem, GUITextBuffer);

		GetMACAddress(tempMACAddress);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAC_1);
		mini_snprintf(GUITextBuffer, 3, "%02X", tempMACAddress[0]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAC_2);
		mini_snprintf(GUITextBuffer, 3, "%02X", tempMACAddress[1]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAC_3);
		mini_snprintf(GUITextBuffer, 3, "%02X", tempMACAddress[2]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAC_4);
		mini_snprintf(GUITextBuffer, 3, "%02X", tempMACAddress[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAC_5);
		mini_snprintf(GUITextBuffer, 3, "%02X", tempMACAddress[4]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MAC_6);
		mini_snprintf(GUITextBuffer, 3, "%02X", tempMACAddress[5]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXIT);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SAVE_TO_FLASH);
		BUTTON_SetText(hItem, GUI_LANG_GetText(200));
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_EXIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_SAVE_TO_FLASH:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERIAL_NUMBER);
				EDIT_GetText(hItem, GUITextBuffer, 9);
				tempSerialNumber = (unsigned int) atoi(GUITextBuffer);

				if (GetNewMACAddress(pMsg->hWin, tempMACAddress))
				{
					SaveSerialNumberToFRAM(&tempSerialNumber);
					SaveMACToFRAM(tempMACAddress);
				}
				break;
			}
			break;
		case ID_EDIT_SERIAL_NUMBER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_MAC_1:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF,2);
				break;
			}
			break;
		case ID_EDIT_MAC_2:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF,2);
				break;
			}
			break;
		case ID_EDIT_MAC_3:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF,2);
				break;
			}
			break;
		case ID_EDIT_MAC_4:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF,2);
				break;
			}
			break;
		case ID_EDIT_MAC_5:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF,2);
				break;
			}
			break;
		case ID_EDIT_MAC_6:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() == FACTORY_LEVEL)
					setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF,2);
				break;
			}
			break;
		}
		break;
		case WM_USER_LOGIN:
			if(FACTORY_LEVEL > PASSWORDS_GetCurrentLevel())
			{
				WM__DeleteSecure(pMsg->hWin);
			}
			break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateserialnNumberWin(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialogSerialNumber, hParent, 0, 0);
	return hWin;
}

/*************************** End of file ****************************/


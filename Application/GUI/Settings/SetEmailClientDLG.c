/*
 * SetEmailClientDLG.c
 *
 *  Created on: 26.02.2019
 *      Author: TomaszSok
 */

#include "SetEmailClientDLG.h"
#include <string.h>
#include <stdlib.h>

#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "mini-printf.h"
#include "passwords.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

SDRAM EMAIL_SETTINGS tempEmailSettings;

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

#define ID_WINDOW_SET_EMAIL 			(GUI_ID_USER + 0x300)
#define ID_TEXT_EMAIL							(GUI_ID_USER + 0x301)
#define ID_TEXT_LOGIN     				(GUI_ID_USER + 0x302)
#define ID_TEXT_PASSWORD					(GUI_ID_USER + 0x303)
#define ID_TEXT_SMTP_SERVER				(GUI_ID_USER + 0x304)
#define ID_TEXT_SMTP_PORT					(GUI_ID_USER + 0x305)

#define ID_EDIT_EMAIL							(GUI_ID_USER + 0x306)
#define ID_EDIT_LOGIN     				(GUI_ID_USER + 0x307)
#define ID_MULTIEDIT_PASSWORD			(GUI_ID_USER + 0x308)
#define ID_EDIT_SMTP_SERVER				(GUI_ID_USER + 0x309)
#define ID_EDIT_SMTP_PORT					(GUI_ID_USER + 0x30A)

#define ID_BUTTON_TEST_EMAIL  		(GUI_ID_USER + 0x30B)

#define ID_WINDOW_SET_RECIVERS		(GUI_ID_USER + 0x30C)
#define ID_LISTVIEW_RECIVERS			(GUI_ID_USER + 0x30D)

#define ID_WINDOW_SET_EMAIL_REPORT	(GUI_ID_USER + 0x310)
#define ID_TEXT_MODE								(GUI_ID_USER + 0x311)
#define ID_TEXT_HOUR     						(GUI_ID_USER + 0x312)
#define ID_TEXT_DAY									(GUI_ID_USER + 0x313)
#define ID_DROPDOWN_MODE						(GUI_ID_USER + 0x314)
#define ID_DROPDOWN_HOUR						(GUI_ID_USER + 0x315)
#define ID_DROPDOWN_DAY							(GUI_ID_USER + 0x316)

SDRAM char mailMessage[MAIL_MESSAGE_BUFFER];

static const GUI_WIDGET_CREATE_INFO _aDialogEmailParametersCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_EMAIL, 0, 0, 730, 290, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_EMAIL, 10, 10, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LOGIN, 10, 55, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_PASSWORD, 10, 100, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SMTP_SERVER, 10, 145, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SMTP_PORT, 10, 190, 220, 35, 0, 0x64, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_EMAIL, 255, 10, 430, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LOGIN, 255, 55, 430, 35, 0, 0x64, 0 },
{ MULTIEDIT_CreateIndirect, "", ID_MULTIEDIT_PASSWORD, 255, 100, 230, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SMTP_SERVER, 255, 145, 430, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SMTP_PORT, 255, 190, 130, 35, 0, 0x64, 0 } };

static const GUI_WIDGET_CREATE_INFO _aDialogEmailReciversCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_RECIVERS, 0, 0, 730, 290, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_RECIVERS, 5, 5, 720, 205, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_TEST_EMAIL, 5, 225, 240, 50, 0, 0x0, 0 } };

static const GUI_WIDGET_CREATE_INFO _aDialogEmailReportCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_EMAIL_REPORT, 0, 0, 730, 290, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_MODE, 10, 10, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_HOUR, 10, 55, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DAY, 10, 100, 220, 35, 0, 0x64, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_MODE, 250, 10, 240, 120, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_HOUR, 250, 55, 240, 120, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_DAY, 250, 100, 240, 120, 0, 0x0, 0 }, };

int EMAIL_CheckEmailAddressCorrectness(char *address)
{
	int charCounter = 0;
	char* pch = strpbrk(address,"@");
	while(pch != NULL)
	{
		charCounter++;
		pch = strpbrk (pch+1,"@");
	}

	if(1!=charCounter)
		return 1;
	else
	{
		pch = NULL;
		pch = strpbrk(address,"\r\n");
		if(pch != NULL)
			return 1;
		else
			return 0;
	}
}

int EMAIL_UpdateEmailParameters(MULTIPAGE_Handle hObj)
{
	WM_HWIN hItem,hPage;
	char GUITextBuffer[10];

	hPage = MULTIPAGE_GetWindow(hObj, 0);

	hItem = WM_GetDialogItem(hPage, ID_EDIT_EMAIL);
	EDIT_GetText(hItem, tempEmailSettings.client.email, 64);
	if (tempEmailSettings.client.email[0] == ' ' && tempEmailSettings.client.email[1] == '\0')
		EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());
	else
	{
		if(EMAIL_CheckEmailAddressCorrectness(tempEmailSettings.client.email))
		{
			MULTIPAGE_SelectPage(hObj, 0);
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
			return 1;
		}
		else
			EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());
	}

	hItem = WM_GetDialogItem(hPage, ID_EDIT_LOGIN);
	EDIT_GetText(hItem, tempEmailSettings.client.login, 64);

	hItem = WM_GetDialogItem(hPage, ID_MULTIEDIT_PASSWORD);
	MULTIEDIT_GetText(hItem, tempEmailSettings.client.password, 32);

	hItem = WM_GetDialogItem(hPage, ID_EDIT_SMTP_SERVER);
	EDIT_GetText(hItem, tempEmailSettings.server.name, 64);

	hItem = WM_GetDialogItem(hPage, ID_EDIT_SMTP_PORT);
	EDIT_GetText(hItem, GUITextBuffer, 10);
	tempEmailSettings.server.port = (uint16_t) atoi(GUITextBuffer);

	hPage = MULTIPAGE_GetWindow(hObj, 1);

	hItem = WM_GetDialogItem(hPage, ID_LISTVIEW_RECIVERS);
	LISTVIEW_SetSel(hItem,-1);
	for(int i=0;i<5;++i)
	{
		LISTVIEW_GetItemText(hItem, 1, i, &tempEmailSettings.client.mailList[i][0], 64);
		if (tempEmailSettings.client.mailList[i][0] == ' ' && tempEmailSettings.client.mailList[i][1] == '\0')
			LISTVIEW_SetItemTextColor(hItem, 1, i, LISTVIEW_CI_UNSEL, SKINS_GetTextColor());
		else
		{
			if (EMAIL_CheckEmailAddressCorrectness(&tempEmailSettings.client.mailList[i][0]))
			{
				MULTIPAGE_SelectPage(hObj, 1);

				LISTVIEW_SetItemTextColor(hItem, 1, i, LISTVIEW_CI_UNSEL, GUI_RED);
				return 1;
			}
			else
				LISTVIEW_SetItemTextColor(hItem, 1, i, LISTVIEW_CI_UNSEL, SKINS_GetTextColor());
		}

	}

	hPage = MULTIPAGE_GetWindow(hObj, 2);

	hItem = WM_GetDialogItem(hPage, ID_DROPDOWN_MODE);
	tempEmailSettings.event.mode = (uint8_t) DROPDOWN_GetSel(hItem);

	hItem = WM_GetDialogItem(hPage, ID_DROPDOWN_HOUR);
	tempEmailSettings.event.hour = (uint8_t) DROPDOWN_GetSel(hItem);

	hItem = WM_GetDialogItem(hPage, ID_DROPDOWN_DAY);
	tempEmailSettings.event.day = (uint8_t) DROPDOWN_GetSel(hItem);

	return 0;
}

static void _cbDialogEmailParameters(WM_MESSAGE * pMsg)
{
	char GUITextBuffer[10];

	WM_HWIN hItem;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		CopyEmailClientSettings(&tempEmailSettings.client, &bkEmailSettings.client);
		CopyEmailServerSettings(&tempEmailSettings.server, &bkEmailSettings.server);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_EMAIL);
		TEXT_SetText(hItem, "E-mail");
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOGIN);
		TEXT_SetText(hItem, "Login");
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PASSWORD);
		TEXT_SetText(hItem, GUI_LANG_GetText(17));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SMTP_SERVER);
		TEXT_SetText(hItem, GUI_LANG_GetText(204));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SMTP_PORT);
		TEXT_SetText(hItem, GUI_LANG_GetText(205));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_EMAIL);
		EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		EDIT_SetText(hItem, tempEmailSettings.client.email);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOGIN);
		EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		EDIT_SetText(hItem, tempEmailSettings.client.login);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_PASSWORD);
		MULTIEDIT_SetPasswordMode(hItem, 1);
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		MULTIEDIT_SetSkin(hItem, MULTIEDIT_CI_EDIT);
		MULTIEDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		MULTIEDIT_SetText(hItem, tempEmailSettings.client.password);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SMTP_SERVER);
		EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		EDIT_SetText(hItem, tempEmailSettings.server.name);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SMTP_PORT);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 10, "%u", tempEmailSettings.server.port);
		EDIT_SetText(hItem, GUITextBuffer);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_EMAIL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, 0, 64);
				break;
			}
			break;
		case ID_EDIT_LOGIN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, 0, 64);
				break;
			}
			break;
		case ID_MULTIEDIT_PASSWORD:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (PASSWORDS_GetCurrentLevel() > USER_LEVEL)
					setKeyboard(pMsg->hWinSrc, MULTIEDIT, 0, 24);
				else
					setKeyboard(pMsg->hWinSrc, MULTIEDIT, PASSON, 24);
				break;
			}
			break;
		case ID_EDIT_SMTP_SERVER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, 0, 64);
				break;
			}
			break;
		case ID_EDIT_SMTP_PORT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 7);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(250, 8, 440, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 53, 440, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 98, 240, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 143, 440, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 188, 140, 38, 4, 2, GUI_BLUE);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetDialogEmailRecivers(WM_MESSAGE * pMsg)
{
	char GUITextBuffer[10];
	WM_HWIN hItem;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_RECIVERS);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		LISTVIEW_AddColumn(hItem, 40, "#", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 680, "E-mail", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 40);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_GRAY);
		LISTVIEW_SetWrapMode(hItem,GUI_WRAPMODE_NONE);
		LISTVIEW_EnableCellSelect(hItem, 1);
		for (int i = 0; i < 5; i++)
		{
			LISTVIEW_AddRow(hItem, NULL);
			itoa(i + 1, GUITextBuffer, 10);
			LISTVIEW_SetItemText(hItem, 0, i, GUITextBuffer);
			LISTVIEW_SetItemText(hItem, 1, i, &tempEmailSettings.client.mailList[i][0]);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_TEST_EMAIL);
		BUTTON_SetText(hItem, GUI_LANG_GetText(206));

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTVIEW_RECIVERS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (LISTVIEW_GetSelCol(pMsg->hWinSrc))
					setKeyboard(pMsg->hWinSrc, LISTVIEW, 0, 64);
				break;
			}
			break;
		case ID_BUTTON_TEST_EMAIL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (0 == EMAIL_UpdateEmailParameters(WM_GetParent(WM_GetParent(pMsg->hWin))))
				{
					SMTP_ConvertLoginDataToBase64(&tempEmailSettings.client);
					CreateTestEMAILTask(&tempEmailSettings);
				}
				break;
			}
			break;
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbDialogEmailReportParameters(WM_MESSAGE * pMsg)
{
	char GUITextBuffer[10];

	WM_HWIN hItem;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		CopyEmailEventSettings(&tempEmailSettings.event,&bkEmailSettings.event);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(39));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HOUR);
		TEXT_SetText(hItem, GUI_LANG_GetText(208));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		if(tempEmailSettings.event.mode == 0)
			WM_HideWindow(hItem);
		else
			WM_ShowWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DAY);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(43));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(44));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(45));
		DROPDOWN_SetSel(hItem, tempEmailSettings.event.mode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_HOUR);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for(int i=0;i<24;++i)
		{
			itoa(i,GUITextBuffer,10);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		DROPDOWN_SetSel(hItem, tempEmailSettings.event.hour);
		if(tempEmailSettings.event.mode == 0)
			WM_HideWindow(hItem);
		else
			WM_ShowWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DAY);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);

		while(DROPDOWN_GetNumItems(hItem))
			DROPDOWN_DeleteItem(hItem, 0);

		if(tempEmailSettings.event.mode == 2)
		{
			WM_ShowWindow(hItem);
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(146));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(147));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(148));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(149));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(150));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(144));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(145));
			DROPDOWN_SetSel(hItem, tempEmailSettings.event.day);
		}
		else if(tempEmailSettings.event.mode == 3)
		{
			WM_ShowWindow(hItem);
			for (int i = 1; i <= 28; ++i)
			{
				itoa(i, GUITextBuffer, 10);
				DROPDOWN_AddString(hItem, GUITextBuffer);
			}
			DROPDOWN_SetSel(hItem, tempEmailSettings.event.day);
		}
		else
			WM_HideWindow(hItem);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_MODE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				tempEmailSettings.event.mode = DROPDOWN_GetSel(pMsg->hWinSrc);

				if(tempEmailSettings.event.mode == 0)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HOUR);
					WM_HideWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_HOUR);
					WM_HideWindow(hItem);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HOUR);
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_HOUR);
					WM_ShowWindow(hItem);
				}

				if(tempEmailSettings.event.mode == 2)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DAY);
					TEXT_SetText(hItem,GUI_LANG_GetText(209));
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DAY);
					while(DROPDOWN_GetNumItems(hItem))
						DROPDOWN_DeleteItem(hItem, 0);
					WM_ShowWindow(hItem);

					DROPDOWN_AddString(hItem, GUI_LANG_GetText(146));
					DROPDOWN_AddString(hItem, GUI_LANG_GetText(147));
					DROPDOWN_AddString(hItem, GUI_LANG_GetText(148));
					DROPDOWN_AddString(hItem, GUI_LANG_GetText(149));
					DROPDOWN_AddString(hItem, GUI_LANG_GetText(150));
					DROPDOWN_AddString(hItem, GUI_LANG_GetText(144));
					DROPDOWN_AddString(hItem, GUI_LANG_GetText(145));
					DROPDOWN_SetSel(hItem, tempEmailSettings.event.day);
				}
				else if(tempEmailSettings.event.mode == 3)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DAY);
					TEXT_SetText(hItem,GUI_LANG_GetText(210));
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DAY);
					while(DROPDOWN_GetNumItems(hItem))
						DROPDOWN_DeleteItem(hItem, 0);
					WM_ShowWindow(hItem);
					for (int i = 1; i <= 28; ++i)
					{
						itoa(i, GUITextBuffer, 10);
						DROPDOWN_AddString(hItem, GUITextBuffer);
					}
					DROPDOWN_SetSel(hItem, tempEmailSettings.event.day);
				}
				else
					{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DAY);
					WM_HideWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DAY);
					WM_HideWindow(hItem);
					}
				break;
			}
			break;
		case ID_DROPDOWN_HOUR:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				break;
			}
			break;
		case ID_DROPDOWN_DAY:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSetEmailParameters(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogEmailParametersCreate, GUI_COUNTOF(_aDialogEmailParametersCreate), _cbDialogEmailParameters, hParent, 0, 0);
	return hWin;
}

WM_HWIN CreateSetEmailRecivers(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogEmailReciversCreate, GUI_COUNTOF(_aDialogEmailReciversCreate), _cbSetDialogEmailRecivers, hParent, 0, 0);
	return hWin;
}

WM_HWIN CreateSetEmailReportParameters(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogEmailReportCreate, GUI_COUNTOF(_aDialogEmailReportCreate), _cbDialogEmailReportParameters, hParent, 0, 0);
	return hWin;
}

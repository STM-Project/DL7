/*
 * HartConfigurationDLG.c
 *
 *  Created on: 03.05.2020
 *      Author: Rafał Markielowski
 */
#include "HartConfigurationDLG.h"
#include "ExpansionCards.h"
#include "ExpansionCards_typedef.h"
#include "CardCalibrationDLG.h"
#include <stdlib.h>
#include <string.h>
#include "draw.h"
#include "dtos.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "skins.h"

#include "ExpansionCards_descriptions.h"
#include "EDIT_double.h"
#include "mini-printf.h"
#include "stdio.h"

#define ID_WINDOW_0       							(GUI_ID_USER + 0x401)
#define ID_BUTTON_GET_LONG_ADDR					(GUI_ID_USER + 0x402)
#define ID_TEXT_SLOT										(GUI_ID_USER + 0x403)
#define ID_TEXT_SHORT_ADDR							(GUI_ID_USER + 0x404)
#define ID_BUTTON_CHANGE_SHORT_ADDRESS	(GUI_ID_USER + 0x405)
#define ID_TEXT_CARD_DESCRIPTION				(GUI_ID_USER + 0x406)
#define ID_EDIT_NEW_SHORT_ADDRESS				(GUI_ID_USER + 0x407)
#define ID_VALUE_ADDR										(GUI_ID_USER + 0x408)
#define ID_TEXT_NEW_SHORT_ADDRESS				(GUI_ID_USER + 0x409)
#define ID_MULTIEDIT_RESPONSE						(GUI_ID_USER + 0x40A)
#define ID_BUTTON_EXIT									(GUI_ID_USER + 0x40B)
#define ID_EDIT_LONG_ADDRESS						(GUI_ID_USER + 0x40C)
#define ID_TEXT_LONG_ADDR								(GUI_ID_USER + 0x40D)
#define ID_TEXT_RESISTOR				(GUI_ID_USER + 0x40E)
#define ID_CHECKBOX_RESISTOR			(GUI_ID_USER + 0x40F)

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

static EXPANSION_CARD localExpansionCard;
static uint8_t localBoardNumber = 0;

static uint8_t shortAddress = 0;
static int requestSendCmd = 0;
static int timerForRequestSendCmd = 0;
static uint8_t HARTLongAddrBuffer[6];
static int visibility = 0;
static char hartFrameHex[240]={0};
static int resistor220ohmState;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate_HART[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_0, 0, 0, 740, 420, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SLOT, 10, 10, 120, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CARD_DESCRIPTION, 140, 10, 310, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RESISTOR, 10, 60, 290, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_RESISTOR, 200, 60, 60, 30, GUI_TA_CENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_SHORT_ADDR, 10, 100, 300, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LONG_ADDRESS, 310, 100, 60, 30,GUI_TA_CENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_GET_LONG_ADDR, 390, 95, 262, 40, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LONG_ADDR, 10, 150, 262, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_VALUE_ADDR, 270, 150, 230, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ MULTIEDIT_CreateIndirect, "", ID_MULTIEDIT_RESPONSE, 10, 200, 720, 90, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_NEW_SHORT_ADDRESS, 10, 310, 290, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_NEW_SHORT_ADDRESS, 310, 310, 60, 30, GUI_TA_CENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CHANGE_SHORT_ADDRESS, 390, 305, 170, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_EXIT, 618, 360, 120, 58, 0, 0x0, 0 }, };

static int StringToHartAddr(char *buf, uint64_t *addr)
{
	uint64_t val = 0;

	val = strtoull(buf, NULL, 16);
	if (val == 0)
	{
		if (strstr(buf, "0"))
		{
			*addr = 0;
			return 1;
		}
		else
			return 0;
	}
	else
	{
		*addr = val;
		if (val <= 0x3fffffffff)
			return 1;
		else
			return 0;
	}
}

void _cbDialogCAL_HART(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char HARTResponseBuffer[720] = { 0 };
	char buf[150] =	{ 0 };

	char GUITextBuffer[10];
	uint64_t longAddress = 0;
	uint8_t newShortAddress = 0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SLOT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		mini_snprintf(GUITextBuffer, 10, "Slot %c", 'A' + localBoardNumber);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_DESCRIPTION);
		TEXT_SetText(hItem, GetExpansionCardDescription(localExpansionCard.type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SHORT_ADDR);
		TEXT_SetText(hItem, GUI_LANG_GetText(237));
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LONG_ADDRESS);
		EDIT_SetText(hItem, "0");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_VALUE_ADDR);
		EDIT_SetTextColor(hItem, 1, SKINS_GetTextColor());
		EDIT_SetText(hItem, "-- -- -- -- -- -- -- -- -- ");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET_LONG_ADDR);
		BUTTON_SetText(hItem, GUI_LANG_GetText(236));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LONG_ADDR);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(225));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_SHORT_ADDRESS);
		TEXT_SetText(hItem, GUI_LANG_GetText(238));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_NEW_SHORT_ADDRESS);
		EDIT_SetText(hItem, "0");
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RESISTOR);
		TEXT_SetText(hItem, GUI_LANG_GetText(222));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_RESISTOR);
		switch(localExpansionCard.settings.HARTcard.isResistor)
		{
		case 0:
			CHECKBOX_SetState(hItem, 0);
			resistor220ohmState=0;
			break;
		case 1:
			CHECKBOX_SetState(hItem, 1);
			resistor220ohmState=1;
			break;
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_SHORT_ADDRESS);
		BUTTON_SetText(hItem, GUI_LANG_GetText(22));
		WM_HideWindow(hItem);
		visibility = 0;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_RESPONSE);
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		MULTIEDIT_SetWrapWord(hItem);
		MULTIEDIT_SetReadOnly(hItem, 1);
		MULTIEDIT_SetSkin(hItem, MULTIEDIT_CI_READONLY);
		MULTIEDIT_SetAutoScrollV(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXIT);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);
		memset(HARTLongAddrBuffer, 0, 6);
		break;

	case WM_POST_PAINT:
		DRAW_RoundedFrame(304, 95, 70, 40, 4, 2, GUI_BLUE);
		DRAW_Line(20, 295, 720, 295, 2, GUI_GRAY);
		if (1 == visibility)
			DRAW_RoundedFrame(304, 305, 70, 39, 4, 2, GUI_BLUE);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_LONG_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 2);

				memset(HARTLongAddrBuffer, 0, 6);
				HART_Init2SendCommandGetLongAddr(localBoardNumber, 0);
				visibility = 0;

				hItem = WM_GetDialogItem(pMsg->hWin, ID_VALUE_ADDR);
				EDIT_SetTextColor(hItem, 1, SKINS_GetTextColor());
				EDIT_SetText(hItem, "-- -- -- -- -- -- -- -- -- ");

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_SHORT_ADDRESS);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_NEW_SHORT_ADDRESS);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_SHORT_ADDRESS);
				WM_HideWindow(hItem);
				break;
			}
			break;
		case ID_EDIT_NEW_SHORT_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 2);
				break;
			}
			break;
		case ID_BUTTON_EXIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_CHECKBOX_RESISTOR:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				resistor220ohmState=1-resistor220ohmState;
				HART_SendCmdResistorOnOff(localBoardNumber, resistor220ohmState);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_SHORT_ADDRESS);
				TEXT_SetText(hItem, GUI_LANG_GetText(238));
				TEXT_SetTextColor(hItem, GUI_BLUE);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_NEW_SHORT_ADDRESS);
				EDIT_SetText(hItem, "0");
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_SHORT_ADDRESS);
				BUTTON_SetText(hItem, GUI_LANG_GetText(22));
				WM_HideWindow(hItem);
				visibility = 0;
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_GET_LONG_ADDR:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_VALUE_ADDR);
				EDIT_SetTextColor(hItem, 1, SKINS_GetTextColor());
				EDIT_SetText(hItem, "-- -- -- -- -- -- -- -- -- ");
				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_LONG_ADDRESS), (char*) buf, 10);
				shortAddress = (uint8_t) atof((const char *) buf);
				HART_SendRequestReadID(localBoardNumber, shortAddress);
				memset(HARTLongAddrBuffer, 0, 6);
				requestSendCmd = 1;
				timerForRequestSendCmd = 1;
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_CHANGE_SHORT_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				mini_snprintf(GUITextBuffer, 11, "%02X%02X%02X%02X%02X", (HARTLongAddrBuffer[0] & 0x7F), HARTLongAddrBuffer[1], HARTLongAddrBuffer[2],
						HARTLongAddrBuffer[3], HARTLongAddrBuffer[4]);
				int id_txt;
				id_txt = StringToHartAddr(GUITextBuffer, &longAddress);
				if (0 < id_txt)
				{
					EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_NEW_SHORT_ADDRESS), (char*) buf, 10);
					newShortAddress = (uint8_t) atof((const char *) buf);
					HART_ChangeAddress(localBoardNumber, longAddress, newShortAddress);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_VALUE_ADDR);
					EDIT_SetTextColor(hItem, 1, SKINS_GetTextColor());
					EDIT_SetText(hItem, "-- -- -- -- -- -- -- -- -- ");
					memset(HARTLongAddrBuffer, 0, 6);
					requestSendCmd = 2;
					timerForRequestSendCmd = 1;
				}
				break;
			}
			break;
		}
		break;

	case WM_TIMER:
		if (0 == WM_GetTimerId(pMsg->Data.v))
		{
			if (timerForRequestSendCmd == 3)
			{
				if ((HARTLongAddrBuffer[0] == 0) && (HARTLongAddrBuffer[1] == 0) && (HARTLongAddrBuffer[2] == 0) && (HARTLongAddrBuffer[3] == 0)
						&& (HARTLongAddrBuffer[4] == 0))
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_RESPONSE);
					memset(buf, 0x20, 63);
					buf[63] = 0;
					MULTIEDIT_SetText(hItem, buf);
				}
				else
				{
					HART_ServiceReadValue(hartFrameHex, localBoardNumber);
					int len = 0;
					for (int i = 0; i < (hartFrameHex[3] + 5); ++i)
						len += mini_snprintf(HARTResponseBuffer + len, 4, "%02X ", hartFrameHex[i]);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_RESPONSE);
					MULTIEDIT_SetText(hItem, HARTResponseBuffer);
				}
				timerForRequestSendCmd=0;
			}
			else if (timerForRequestSendCmd == 1)
				timerForRequestSendCmd++;
			else if (timerForRequestSendCmd == 2)
			{
				if (requestSendCmd == 1)
					EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_LONG_ADDRESS), buf, 10);
				else
					EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_NEW_SHORT_ADDRESS), buf, 10);
				shortAddress = (uint8_t) atof((const char *) buf);

				if (requestSendCmd == 2)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LONG_ADDRESS);
					EDIT_SetText(hItem, buf);
				}

				HART_GetLongAddr(localBoardNumber, shortAddress, HARTLongAddrBuffer);
				mini_snprintf((char *) buf, 21, (const char *) " %02X%02X%02X%02X%02X  (Rev.%d)", (HARTLongAddrBuffer[0] & 0x7F), HARTLongAddrBuffer[1],
						HARTLongAddrBuffer[2], HARTLongAddrBuffer[3], HARTLongAddrBuffer[4], HARTLongAddrBuffer[5]);

				if ((HARTLongAddrBuffer[0] == 0) && (HARTLongAddrBuffer[1] == 0) && (HARTLongAddrBuffer[2] == 0) && (HARTLongAddrBuffer[3] == 0)
						&& (HARTLongAddrBuffer[4] == 0))
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_VALUE_ADDR);
					EDIT_SetTextColor(hItem, 1, GUI_RED);
					EDIT_SetText(hItem, GUI_LANG_GetText(226));
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_VALUE_ADDR);
					EDIT_SetTextColor(hItem, 1, SKINS_GetTextColor());
					EDIT_SetText(hItem, buf);
					visibility = 1;
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_SHORT_ADDRESS);
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_NEW_SHORT_ADDRESS);
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_SHORT_ADDRESS);
					WM_ShowWindow(hItem);
				}
				timerForRequestSendCmd = 3;
			}
			WM_RestartTimer(pMsg->Data.v, 1000);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateHARTConfigurationWin(WM_HWIN hParent, uint8_t boardNumber, uint8_t InputsMode)
{
	WM_HWIN hWin;
	localBoardNumber = boardNumber;

	GetExpansionBoard(localBoardNumber, &localExpansionCard);

	hWin = GUI_CreateDialogBox(_aDialogCreate_HART, GUI_COUNTOF(_aDialogCreate_HART), _cbDialogCAL_HART, hParent, 0, 0);
	WM_CreateTimer(hWin, 0, 1, 0);

	return hWin;
}

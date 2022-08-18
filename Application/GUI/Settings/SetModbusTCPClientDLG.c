/*
 * SetModbusTCPClientDLG.c
 *
 *  Created on: 29.05.2018
 *      Author: TomaszSok
 */
#include "SetModbusTCPClientDLG.h"
#include <string.h>
#include <stdlib.h>
#include "parameters.h"
#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato60;

int ParsingIP(char *strIP, uint8_t *IP);

#define ID_WINDOW_SERVERS     			(GUI_ID_USER + 0x300)
#define ID_LISTVIEW_SERVERS					(GUI_ID_USER + 0x301)

#define ID_FRAMEWIN_SERVER						(GUI_ID_USER + 0x400)
#define ID_TEXT_SERVER_ID      				(GUI_ID_USER + 0x401)
#define ID_TEXT_SERVER_MODE						(GUI_ID_USER + 0x402)
#define ID_TEXT_SERVER_IP   					(GUI_ID_USER + 0x403)
#define ID_TEXT_SERVER_PORT      			(GUI_ID_USER + 0x404)
#define ID_TEXT_SERVER_TIMEOUT   			(GUI_ID_USER + 0x405)
#define ID_TEXT_SERVER_FREQUENCY 			(GUI_ID_USER + 0x406)
#define ID_DROPDOWN_SERVER_MODE  			(GUI_ID_USER + 0x407)
#define ID_EDIT_SERVER_IP							(GUI_ID_USER + 0x408)
#define ID_EDIT_SERVER_PORT     			(GUI_ID_USER + 0x409)
#define ID_EDIT_SERVER_TIMEOUT				(GUI_ID_USER + 0x40A)
#define ID_EDIT_SERVER_FREQUENCY 			(GUI_ID_USER + 0x40B)
#define ID_BUTTON_SERVER_CONFIRM			(GUI_ID_USER + 0x40C)
#define ID_BUTTON_SERVER_ANNUL				(GUI_ID_USER + 0x40D)
#define ID_TEXT_SERVER_TIMEOUT_UNIT		(GUI_ID_USER + 0x40E)
#define ID_TEXT_SERVER_FREQUENCY_UNIT	(GUI_ID_USER + 0x40F)

#define ID_WINDOW_REGISTERS    			(GUI_ID_USER + 0x302)
#define ID_LISTVIEW_REGISTERS  			(GUI_ID_USER + 0x303)

#define FRAMEWIN_REGISTER								(GUI_ID_USER + 0x400)
#define ID_TEXT_REGISTER_SERVER    			(GUI_ID_USER + 0x401)
#define ID_TEXT_REGISTER_DEVICE_ADDRESS	(GUI_ID_USER + 0x402)
#define ID_TEXT_REGISTER_ADDRESS    		(GUI_ID_USER + 0x403)
#define ID_TEXT_REGISTER_TYPE      			(GUI_ID_USER + 0x404)
#define ID_DROPDOWN_REGISTER_SERVER  		(GUI_ID_USER + 0x405)
#define ID_EDIT_DEVICE_ADDRESS					(GUI_ID_USER + 0x406)
#define ID_EDIT_REGISTER_ADDRESS    		(GUI_ID_USER + 0x407)
#define ID_DROPDOWN_REGISTER_TYPE   		(GUI_ID_USER + 0x408)
#define ID_BUTTON_REGISTER_CONFIRM			(GUI_ID_USER + 0x409)
#define ID_BUTTON_REGISTER_ANNUL				(GUI_ID_USER + 0x40A)
#define ID_TEXT_REGISTER_INFO      			(GUI_ID_USER + 0x40B)

MODBUS_TCP_REGISTER tempModbusTCPregisters[MAX_MODBUS_TCP_REGISTERS] __attribute__ ((section(".sdram")));
MODBUS_TCP_SERVER tempMBserver[MAX_MODBUS_TCP_CONNECTIONS] __attribute__ ((section(".sdram")));

static MODBUS_TCP_SERVER *tempServer;
static MODBUS_TCP_REGISTER *tempRegister;

static WM_HWIN hWinServers;
static WM_HWIN hWinRegisters;

static const char MBTCPRegisterDesc[10][20] =
{"uint (16bit)","int (16bit)","uint (32bit)","uint (32bit) sw","int (32bit)","int (32bit) sw","float (32bit)","float (32bit) sw","int (64bit)","double (64bit)"};

static const GUI_WIDGET_CREATE_INFO _aSetModbusTCPServers[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SERVERS, 0, 0, 730, 255, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_SERVERS, 5, 10, 720, 235, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aEditModbusTCPServer[] =
{
{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_SERVER, 5, 10, 720, 245, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SERVER_MODE, 10, 10, 150, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SERVER_IP, 10, 60, 150, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SERVER_PORT, 380, 60, 150, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SERVER_TIMEOUT, 10, 110, 150, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SERVER_FREQUENCY, 380, 10, 150, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SERVER_MODE, 170, 10, 220, 90, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SERVER_IP, 175, 60, 210, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SERVER_PORT, 545, 60, 150, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SERVER_TIMEOUT, 175, 110, 175, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SERVER_FREQUENCY, 545, 10, 130, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "ms", ID_TEXT_SERVER_TIMEOUT_UNIT, 355, 110, 30, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "s", ID_TEXT_SERVER_FREQUENCY_UNIT, 680, 10, 10, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

{ BUTTON_CreateIndirect, "", ID_BUTTON_SERVER_CONFIRM, 472, 181, 120, 58, 2, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_SERVER_ANNUL, 594, 181, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetModbusTCPRegisters[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_REGISTERS, 0, 0, 730, 255, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_REGISTERS, 5, 10, 720, 235, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aEditModbusTCPRegister[] =
{
{ FRAMEWIN_CreateIndirect, "", FRAMEWIN_REGISTER, 5, 10, 720, 245, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_SERVER, 10, 10, 140, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_DEVICE_ADDRESS, 410, 10, 150, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_ADDRESS, 10, 60, 140, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_TYPE, 370, 60, 130, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_REGISTER_SERVER, 160, 10, 270, 90, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_DEVICE_ADDRESS, 575, 10, 120, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_REGISTER_ADDRESS, 165, 60, 130, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_REGISTER_TYPE, 510, 60, 190, 90, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_INFO, 25, 110, 450, 80, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_REGISTER_CONFIRM, 472, 181, 120, 58, 2, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_REGISTER_ANNUL, 594, 181, 120, 58, 0, 0x0, 0 }, };

static void LISTVIEW_RefreshMBTCPRegister(LISTVIEW_Handle hObj, unsigned Row, MODBUS_TCP_REGISTER *reg);
static void LISTVIEW_InitMBTCPRegisters(LISTVIEW_Handle hObj);
static void LISTVIEW_RefreshMBTCPServer(LISTVIEW_Handle hObj, unsigned Row, MODBUS_TCP_SERVER *server);
static void LISTVIEW_InitMBTCPServers(LISTVIEW_Handle hObj);
static WM_HWIN CreateEditModbusTCPServer(WM_HWIN hParent, int regNo);
static WM_HWIN CreateEditModbusTCPRegister(WM_HWIN hParent, int regNo);

static int CheckParameterLimits(EDIT_Handle hObj, int min, int max)
{
	char GUITextBuffer[10];
	EDIT_GetText(hObj, GUITextBuffer, 10);
	int temp = atoi(GUITextBuffer);
	if (temp < min || max < temp)
	{
		EDIT_SetTextColor(hObj, EDIT_CI_ENABLED, GUI_RED);
		return 0;
	}
	else
	{
		EDIT_SetTextColor(hObj, EDIT_CI_ENABLED, SKINS_GetTextColor());
		return 1;
	}
}

static void _cbSetModbusTCPServers(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		for (int i=0;i<MAX_MODBUS_TCP_CONNECTIONS;i++)
			tempMBserver[i]=bkMBserver[i];

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SERVERS);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		LISTVIEW_AddColumn(hItem, 145, GUI_LANG_GetText(39), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 240, GUI_LANG_GetText(186), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 160, GUI_LANG_GetText(73), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 145, GUI_LANG_GetText(182), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 47);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_GRAY);
		LISTVIEW_InitMBTCPServers(hItem);
		break;
	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SERVERS);
		unsigned row = LISTVIEW_GetSel(hItem);
		LISTVIEW_RefreshMBTCPServer(hItem, row, &tempMBserver[row]);

		char GUITextBuffer[22];
		mini_snprintf(GUITextBuffer, 22, "%d.%d.%d.%d:%d",tempMBserver[row].ip[0], tempMBserver[row].ip[1],
				tempMBserver[row].ip[2],tempMBserver[row].ip[3],tempMBserver[row].port);
		hItem = WM_GetDialogItem(hWinRegisters, ID_LISTVIEW_REGISTERS);
		for(int i=0;i<100;i++)
		{
			if(tempMBserver[row].connectionID == tempModbusTCPregisters[i].connectionID)
				LISTVIEW_SetItemText(hItem, 1, i, GUITextBuffer);
		}
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTVIEW_SERVERS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				CreateEditModbusTCPServer(pMsg->hWin,LISTVIEW_GetSel(pMsg->hWinSrc));
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

static void _cbSetModbusTCPRegisters(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		for(int i=0;i<MAX_MODBUS_TCP_REGISTERS;i++)
			tempModbusTCPregisters[i] = bkModbusTCPregisters[i];

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_REGISTERS);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		LISTVIEW_AddColumn(hItem, 40, "#", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 240, GUI_LANG_GetText(202), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 130, GUI_LANG_GetText(185), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 120, GUI_LANG_GetText(186), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 160, GUI_LANG_GetText(105), GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 47);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_GRAY);
		LISTVIEW_InitMBTCPRegisters(hItem);
		break;
	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_REGISTERS);
		unsigned row = LISTVIEW_GetSel(hItem);
		LISTVIEW_RefreshMBTCPRegister(hItem, row, &tempModbusTCPregisters[row]);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTVIEW_REGISTERS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				CreateEditModbusTCPRegister(pMsg->hWin,LISTVIEW_GetSel(pMsg->hWinSrc));
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

WM_HWIN CreateSetModbusTCPServers(WM_HWIN hParent)
{
	hWinServers = GUI_CreateDialogBox(_aSetModbusTCPServers, GUI_COUNTOF(_aSetModbusTCPServers),	_cbSetModbusTCPServers, hParent, 0, 0);
	return hWinServers;
}

WM_HWIN CreateSetModbusTCPRegisters(WM_HWIN hParent)
{
	hWinRegisters = GUI_CreateDialogBox(_aSetModbusTCPRegisters, GUI_COUNTOF(_aSetModbusTCPRegisters),	_cbSetModbusTCPRegisters, hParent, 0, 0);
	return hWinRegisters;
}

static void LISTVIEW_RefreshMBTCPRegister(LISTVIEW_Handle hObj, unsigned Row, MODBUS_TCP_REGISTER *reg)
{
	char GUITextBuffer[22];
	if(0==reg->connectionID)
		LISTVIEW_SetItemText(hObj, 1, Row, GUI_LANG_GetText(27));
	else
	{
		mini_snprintf(GUITextBuffer, 22, "%d.%d.%d.%d:%d",tempMBserver[reg->connectionID-1].ip[0], tempMBserver[reg->connectionID-1].ip[1],
				tempMBserver[reg->connectionID-1].ip[2],tempMBserver[reg->connectionID-1].ip[3],tempMBserver[reg->connectionID-1].port);
		LISTVIEW_SetItemText(hObj, 1, Row, GUITextBuffer);
	}
	itoa(reg->deviceAddress , GUITextBuffer,10);
	LISTVIEW_SetItemText(hObj, 2, Row, GUITextBuffer);
	itoa(reg->number , GUITextBuffer,10);
	LISTVIEW_SetItemText(hObj, 3, Row, GUITextBuffer);
	LISTVIEW_SetItemText(hObj, 4, Row, MBTCPRegisterDesc[reg->registerType]);
}

static void LISTVIEW_InitMBTCPRegisters(LISTVIEW_Handle hObj)
{
	char GUITextBuffer[4];
	for (int i = 0; i < MAX_MODBUS_TCP_REGISTERS; i++)
	{
		LISTVIEW_AddRow(hObj, NULL);
		itoa(i + 1, GUITextBuffer, 10);
		LISTVIEW_SetItemText(hObj, 0, i, GUITextBuffer);
		LISTVIEW_RefreshMBTCPRegister(hObj,i, &tempModbusTCPregisters[i]);
	}
}

static void LISTVIEW_RefreshMBTCPServer(LISTVIEW_Handle hObj, unsigned Row, MODBUS_TCP_SERVER *server)
{
	char GUITextBuffer[22];
	if(1==server->mode)
		LISTVIEW_SetItemText(hObj, 0, Row, GUI_LANG_GetText(154));
	else
		LISTVIEW_SetItemText(hObj, 0, Row, GUI_LANG_GetText(27));
	mini_snprintf(GUITextBuffer, 22, "%d.%d.%d.%d:%d", server->ip[0], server->ip[1],server->ip[2],server->ip[3],server->port);
	LISTVIEW_SetItemText(hObj, 1, Row, GUITextBuffer);
	mini_snprintf(GUITextBuffer, 10, "%d", server->responseTimeout);
	LISTVIEW_SetItemText(hObj, 2, Row, GUITextBuffer);
	mini_snprintf(GUITextBuffer, 10, "%d", server->frequency);
	LISTVIEW_SetItemText(hObj, 3, Row, GUITextBuffer);
}

static void LISTVIEW_InitMBTCPServers(LISTVIEW_Handle hObj)
{
	for (int i = 0; i < MAX_MODBUS_TCP_CONNECTIONS; i++)
	{
		LISTVIEW_AddRow(hObj, NULL);
		LISTVIEW_RefreshMBTCPServer(hObj,i, &tempMBserver[i]);
	}
}

static void _cbEditModbusTCPServer(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[20];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_LIGHTBLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetTitleHeight(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_MODE);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(39));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_IP);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(66));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_PORT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(188));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_TIMEOUT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(73));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_TIMEOUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_FREQUENCY);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(182));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SERVER_FREQUENCY_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SERVER_MODE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));
		DROPDOWN_SetSel(hItem, tempServer->mode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_IP);
		mini_snprintf(GUITextBuffer,17, "%d.%d.%d.%d", tempServer->ip[0], tempServer->ip[1],tempServer->ip[2],tempServer->ip[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_PORT);
		mini_snprintf(GUITextBuffer,6, "%d", tempServer->port);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_TIMEOUT);
		mini_snprintf(GUITextBuffer,10, "%d", tempServer->responseTimeout);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_FREQUENCY);
		mini_snprintf(GUITextBuffer,10, "%d", tempServer->frequency);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SERVER_CONFIRM);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SERVER_ANNUL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_SERVER_CONFIRM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				int WrongValue = 0;
				uint8_t ip[4];

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SERVER_MODE);
				tempServer->mode = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_IP);
				EDIT_GetText(hItem, GUITextBuffer, 17);

				if (ParsingIP(GUITextBuffer, ip))
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					WrongValue = 1;
				}
				else
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());
					for(int i=0;i<4;i++)
						tempServer->ip[i]=ip[i];
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_PORT);
				EDIT_GetText(hItem, GUITextBuffer, 6);
				uint16_t port = (uint16_t) atoi(GUITextBuffer);

				if (port > 65535)
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					WrongValue = 1;
				}
				else
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());
					tempServer->port = port;
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_TIMEOUT);
				EDIT_GetText(hItem, GUITextBuffer, 6);
				uint32_t responseTimeout = (uint32_t) atoi(GUITextBuffer);

				if (responseTimeout > 60000 || responseTimeout < 1000)
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					WrongValue = 1;
				}
				else
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());
					tempServer->responseTimeout = (uint16_t)responseTimeout;
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SERVER_FREQUENCY);
				EDIT_GetText(hItem, GUITextBuffer, 6);
				uint32_t frequency = (uint32_t) atoi(GUITextBuffer);

				if (frequency > 3600 || frequency < 1)
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					WrongValue = 1;
				}
				else
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());
					tempServer->frequency = (uint16_t) frequency;
				}

				if(0==WrongValue)
				{
					UserMessage.MsgId = WM_USER_REFRESH;
					UserMessage.hWinSrc = pMsg->hWin;
					UserMessage.Data.v = 0;
					WM_SendToParent(pMsg->hWin, &UserMessage);

					WM_DeleteWindow(pMsg->hWin);
				}
				break;
			}
			break;
		case ID_BUTTON_SERVER_ANNUL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_EDIT_SERVER_IP:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 16);
				break;
			}
			break;
		case ID_EDIT_SERVER_PORT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 6);
				break;
			}
			break;
		case ID_EDIT_SERVER_TIMEOUT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 5);
				break;
			}
			break;
		case ID_EDIT_SERVER_FREQUENCY:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 4);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(540, 8, 159, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(170, 58, 219, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(540, 58, 159, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(170, 108, 219, 38, 4, 2, GUI_BLUE);
		break;
	}
}

static WM_HWIN CreateEditModbusTCPServer(WM_HWIN hParent, int regNo)
{
	WM_HWIN hWin;
	tempServer = &tempMBserver[regNo];
	hWin = GUI_CreateDialogBox(_aEditModbusTCPServer, GUI_COUNTOF(_aEditModbusTCPServer), _cbEditModbusTCPServer, hParent, 0, 0);
	return hWin;
}

static void _cbEditModbusTCPRegister(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[22];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_LIGHTBLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetTitleHeight(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_SERVER);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, "SERVER");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_DEVICE_ADDRESS);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(185));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_ADDRESS);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(186));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_TYPE);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(105));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_SERVER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		for(int i = 0;i<MAX_MODBUS_TCP_CONNECTIONS;i++)
		{
			mini_snprintf(GUITextBuffer, 22, "%d.%d.%d.%d:%d",tempMBserver[i].ip[0], tempMBserver[i].ip[1],
					tempMBserver[i].ip[2],tempMBserver[i].ip[3],tempMBserver[i].port);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		DROPDOWN_SetSel(hItem, tempRegister->connectionID);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ADDRESS);
		itoa(tempRegister->deviceAddress, GUITextBuffer, 10);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_REGISTER_ADDRESS);
		itoa(tempRegister->number, GUITextBuffer, 10);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_TYPE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for(int i =0;i<10;i++)
			DROPDOWN_AddString(hItem, MBTCPRegisterDesc[i]);
		DROPDOWN_SetSel(hItem, tempRegister->registerType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_INFO);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, "300000-365535 - Input Registers\n400000-465535 - Holding Registers");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REGISTER_CONFIRM);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_REGISTER_ANNUL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_REGISTER_CONFIRM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_SERVER);
				tempRegister->connectionID = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ADDRESS);
				if (0 == CheckParameterLimits(hItem, 1, 247))
				{
					break;
				}
				else
				{
					EDIT_GetText(hItem, GUITextBuffer, 4);
					tempRegister->deviceAddress = atoi(GUITextBuffer);
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_REGISTER_ADDRESS);
				if (0 == CheckParameterLimits(hItem, 30000, 49999) &&
						0 == CheckParameterLimits(hItem, 300000, 365535) &&
						0 == CheckParameterLimits(hItem, 400000, 465535))
				{
					break;
				}
				else
				{
					EDIT_GetText(hItem, GUITextBuffer, 7);
					tempRegister->number = atoi(GUITextBuffer);
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_TYPE);
				tempRegister->registerType = DROPDOWN_GetSel(hItem);


				UserMessage.MsgId = WM_USER_REFRESH;
				UserMessage.hWinSrc = pMsg->hWin;
				UserMessage.Data.v = 0;
				WM_SendToParent(pMsg->hWin, &UserMessage);

				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_REGISTER_ANNUL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_EDIT_DEVICE_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 3);
				break;
			}
			break;
		case ID_EDIT_REGISTER_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 6);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(570, 8, 129, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(160, 58, 139, 38, 4, 2, GUI_BLUE);
		break;
	}
}

static WM_HWIN CreateEditModbusTCPRegister(WM_HWIN hParent, int regNo)
{
	WM_HWIN hWin;
	tempRegister = &tempModbusTCPregisters[regNo];
	hWin = GUI_CreateDialogBox(_aEditModbusTCPRegister, GUI_COUNTOF(_aEditModbusTCPRegister), _cbEditModbusTCPRegister, hParent, 0, 0);
	return hWin;
}

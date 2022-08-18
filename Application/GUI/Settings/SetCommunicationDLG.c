#include "SetCommunicationDLG.h"
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
#include "SetEmailClientDLG.h"
#include "smtp_netconn.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_SET_COMMUNICATION	(GUI_ID_USER + 0x201)
#define ID_BUTTON_OK     						(GUI_ID_USER + 0x202)
#define ID_MULTIPAGE_COMMUNICATION  (GUI_ID_USER + 0x203)
#define ID_BUTTON_CANCLE     				(GUI_ID_USER + 0x204)

#define ID_WINDOW_SET_ETH (GUI_ID_USER + 0x205)
#define ID_MULTIPAGE_ETH  (GUI_ID_USER + 0x206)

#define ID_WINDOW_SET_LOCAL_ETH (GUI_ID_USER + 0x305)
#define ID_TEXT_IP 				(GUI_ID_USER + 0x306)
#define ID_TEXT_MASK 			(GUI_ID_USER + 0x307)
#define ID_TEXT_GATE 			(GUI_ID_USER + 0x308)
#define ID_TEXT_PORT 			(GUI_ID_USER + 0x309)
#define ID_EDIT_IP 				(GUI_ID_USER + 0x30A)
#define ID_EDIT_MASK 			(GUI_ID_USER + 0x30B)
#define ID_EDIT_GATE 			(GUI_ID_USER + 0x30C)
#define ID_EDIT_PORT 			(GUI_ID_USER + 0x30D)
#define ID_TEXT_DNS_1			(GUI_ID_USER + 0x30E)
#define ID_TEXT_DNS_2			(GUI_ID_USER + 0x30F)
#define ID_EDIT_DNS_1			(GUI_ID_USER + 0x310)
#define ID_EDIT_DNS_2			(GUI_ID_USER + 0x311)

#define ID_WINDOW_SET_RS     		(GUI_ID_USER + 0x200)
#define ID_TEXT_BAUDRATE				(GUI_ID_USER + 0x208)
#define ID_TEXT_PARITY     			(GUI_ID_USER + 0x209)
#define ID_TEXT_MODBUS_ADDRESS	(GUI_ID_USER + 0x20A)
#define ID_DROPDOWN_BAUDRATE    (GUI_ID_USER + 0x20B)
#define ID_DROPDOWN_PARITY     	(GUI_ID_USER + 0x20C)
#define ID_EDIT_MODBUS_ADDRESS  (GUI_ID_USER + 0x20D)

#define ID_WINDOW_SET_EMAIL 			(GUI_ID_USER + 0x20E)
#define ID_MULTIPAGE_EMAIL 				(GUI_ID_USER + 0x20F)

static WM_HWIN CreateSetModbusTCP(WM_HWIN hParent);
static WM_HWIN CreateSetComETH(WM_HWIN hParent);
static WM_HWIN CreateSetComRS(WM_HWIN hParent);
static WM_HWIN CreateSetEmail(WM_HWIN hParent);

static int CheckETHSettings(ETH_SETTINGS *Dst, ETH_SETTINGS *Src);
static int CheckRSSettings(COM_SETTINGS *Dst, COM_SETTINGS *Src);
static int CheckModbusTCPRegisterSettings(MODBUS_TCP_REGISTER *Dst, MODBUS_TCP_REGISTER *Src);
static int CheckModbusTCPServerSettings(MODBUS_TCP_SERVER *Dst, MODBUS_TCP_SERVER *Src);
static int CheckEmailSettings(EMAIL_SETTINGS *Dst, EMAIL_SETTINGS *Src);

COM_SETTINGS tempComSettings __attribute__ ((section(".sdram")));
ETH_SETTINGS tempEthSettings __attribute__ ((section(".sdram")));
COM_SETTINGS ComSettings __attribute__ ((section(".sdram")));

int ParsingIP(char *strIP, uint8_t *IP)
{

	char *saveptr;
	int i, ile_krop;
	char key[] = "1234567890.";
	uint buff[4] = { 0 };
	i = strspn(strIP, key);
	if ((i == strlen(strIP)) & (i < 16) & (i > 6))
	{
		ile_krop = 0;
		for (i = 0; i < strlen(strIP); i++)
			if (strIP[i] == '.')
				ile_krop++;

		if (ile_krop == 3)
		{
			buff[0] = atoi(strtok_r(strIP, ".", &saveptr));
			buff[1] = atoi(strtok_r(NULL, ".", &saveptr));
			buff[2] = atoi(strtok_r(NULL, ".", &saveptr));
			buff[3] = atoi(strtok_r(NULL, ".", &saveptr));

			if (((buff[0] <= 255) & (buff[0] >= 0)) & ((buff[1] <= 255) & (buff[1] >= 0)) & ((buff[2] <= 255) & (buff[2] >= 0)) & ((buff[3] <= 255) & (buff[3] >= 0)))
			{
				for (i = 0; i < 4; i++, IP++)
					*IP = (uint8_t) buff[i];
				return 0;
			}
			else
				return 1;
		}
		else
			return 1;
	}
	else
		return 1;
}

int CheckIPCorrectness(EDIT_Handle hObj,uint8_t *IP)
{
	char GUITextBuffer[17];
	EDIT_GetText(hObj, GUITextBuffer, 17);
	if (ParsingIP(GUITextBuffer, IP))
	{
		EDIT_SetTextColor(hObj, EDIT_CI_ENABLED, GUI_RED);
		return 1;
	}
	else
	{
		EDIT_SetTextColor(hObj, EDIT_CI_ENABLED, SKINS_GetTextColor());
		return 0;
	}
}

static const GUI_WIDGET_CREATE_INFO _aSetCommunicationDialogCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_COMMUNICATION, 0, 0, 740, 420, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 496, 360, 120, 58, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_COMMUNICATION, 5, 2, 730, 355, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCLE, 618, 360, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aDialogModbusTCPCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_ETH, 0, 0, 730, 310, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_ETH, 0, 0, 730, 310, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aDialogETHCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_LOCAL_ETH, 0, 0, 730, 315, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_IP, 10, 10, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_PORT, 10, 55, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_MASK, 10, 100, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_GATE, 10, 145, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DNS_1, 10, 190, 220, 35, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DNS_2, 10, 235, 220, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_IP, 255, 10, 230, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_MASK, 255, 100, 230, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_GATE, 255, 145, 230, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_PORT, 255, 55, 230, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_DNS_1, 255, 190, 230, 35, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_DNS_2, 255, 235, 230, 35, 0, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aDialogCOMCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_RS, 0, 0, 730, 315, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_BAUDRATE, 10, 10, 220, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_PARITY, 10, 55, 220, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_MODBUS_ADDRESS, 10, 100, 220, 35, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_BAUDRATE, 250, 10, 240, 240, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_PARITY, 250, 55, 240, 90, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_MODBUS_ADDRESS, 255, 100, 230, 35, 0, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aDialogEmailCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_EMAIL, 0, 0, 730, 315, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_EMAIL, 0, 0, 730, 315, 0, 0x0, 0 }, };

void _cbSetCommunicationDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem, hPage;
	int NCode;
	int Id;
	char GUITextBuffer[17];
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		CopyEthSettings(&tempEthSettings, &bkEthSettings);
		CopyComSettings(&tempComSettings, &bkComSettings);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_COMMUNICATION);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		MULTIPAGE_AddPage(hItem, CreateSetComETH(pMsg->hWin), "Ethernet");
		MULTIPAGE_AddEmptyPage(hItem, CreateSetEmail(pMsg->hWin), "E-mail");
		MULTIPAGE_AddPage(hItem, CreateSetModbusTCP(pMsg->hWin), "Modbus TCP");
		MULTIPAGE_AddPage(hItem, CreateSetComRS(pMsg->hWin), "RS485 COM");

		MULTIPAGE_SetTabWidth(hItem, 182, 0);
		MULTIPAGE_SetTabWidth(hItem, 182, 1);
		MULTIPAGE_SetTabWidth(hItem, 182, 2);
		MULTIPAGE_SetTabWidth(hItem, 182, 3);
		MULTIPAGE_SelectPage(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCLE);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				int WrongValue = 0;

				hPage = MULTIPAGE_GetWindow(WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_COMMUNICATION), 0);

				hItem = WM_GetDialogItem(hPage, ID_EDIT_IP);
				if(CheckIPCorrectness(hItem, tempEthSettings.IPAddress))
					WrongValue = 1;

				hItem = WM_GetDialogItem(hPage, ID_EDIT_MASK);
				if(CheckIPCorrectness(hItem, tempEthSettings.NetMask))
					WrongValue = 1;

				hItem = WM_GetDialogItem(hPage, ID_EDIT_GATE);
				if(CheckIPCorrectness(hItem, tempEthSettings.IPGateway))
					WrongValue = 1;

				hItem = WM_GetDialogItem(hPage, ID_EDIT_DNS_1);
				if(CheckIPCorrectness(hItem, tempEthSettings.primaryDNS))
					WrongValue = 1;

				hItem = WM_GetDialogItem(hPage, ID_EDIT_DNS_2);
				if(CheckIPCorrectness(hItem, tempEthSettings.secondaryDNS))
					WrongValue = 1;

				hItem = WM_GetDialogItem(hPage, ID_EDIT_PORT);
				EDIT_GetText(hItem, GUITextBuffer, 6);
				tempEthSettings.IPport = (uint) atoi(GUITextBuffer);

				if (tempEthSettings.IPport > 65535 || tempEthSettings.IPport == 80)
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					WrongValue = 1;
				}
				else
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());

				hPage = MULTIPAGE_GetWindow(WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_COMMUNICATION), 1);
				hItem = WM_GetDialogItem(hPage, ID_MULTIPAGE_EMAIL);
				if(EMAIL_UpdateEmailParameters(hItem))
					WrongValue = 2;

				hPage = MULTIPAGE_GetWindow(WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_COMMUNICATION), 3);
				hItem = WM_GetDialogItem(hPage, ID_EDIT_MODBUS_ADDRESS);
				EDIT_GetText(hItem, GUITextBuffer, 4);
				tempComSettings.MBAddress = (short) atoi(GUITextBuffer);

				if (tempComSettings.MBAddress > 247 || tempComSettings.MBAddress < 1)
				{
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					WrongValue = 3;
				}
				else
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, SKINS_GetTextColor());

				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_COMMUNICATION);
				if (WrongValue == 1)
					MULTIPAGE_SelectPage(hItem, 0);
				else if (WrongValue == 2)
					MULTIPAGE_SelectPage(hItem, 1);
				else if (WrongValue == 3)
					MULTIPAGE_SelectPage(hItem, 3);
				else
				{
					if (CheckETHSettings(&bkEthSettings, &tempEthSettings))
					{
						CopyEthSettings(&bkEthSettings, &tempEthSettings);
						NewSettings = 1;
						RestartDevice = 1;
					}

					if (CheckRSSettings(&bkComSettings, &tempComSettings))
					{
						CopyComSettings(&bkComSettings, &tempComSettings);
						NewSettings = 1;
						RestartDevice = 1;
					}

					for (int i = 0; i < MAX_MODBUS_TCP_CONNECTIONS; i++)
					{
						if (CheckModbusTCPServerSettings(&bkMBserver[i], &tempMBserver[i]))
						{
							bkMBserver[i] = tempMBserver[i];
							NewSettings = 1;
							RestartDevice = 1;
						}
					}

					for (int i = 0; i < MAX_MODBUS_TCP_REGISTERS; i++)
					{
						if (CheckModbusTCPRegisterSettings(&bkModbusTCPregisters[i], &tempModbusTCPregisters[i]))
						{
							bkModbusTCPregisters[i] = tempModbusTCPregisters[i];
							NewSettings = 1;
							RestartDevice = 1;
						}
					}

					if(CheckEmailSettings(&bkEmailSettings, &tempEmailSettings))
					{
						NewSettings = 1;
						RestartDevice = 1;
						CopyEmailClientSettings(&bkEmailSettings.client,&tempEmailSettings.client);
						CopyEmailServerSettings(&bkEmailSettings.server,&tempEmailSettings.server);
						CopyEmailEventSettings(&bkEmailSettings.event,&tempEmailSettings.event);
					}
					WM_DeleteWindow(pMsg->hWin);
				}
				break;
			}
			break;
		case ID_MULTIPAGE_COMMUNICATION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_CANCLE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_USER_LOGIN:
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
			WM_HideWindow(hItem);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

void _cbDialogModbusTCP(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_ETH);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		MULTIPAGE_AddPage(hItem, CreateSetModbusTCPServers(pMsg->hWin), GUI_LANG_GetText(201));
		MULTIPAGE_AddPage(hItem, CreateSetModbusTCPRegisters(pMsg->hWin), GUI_LANG_GetText(187));
		MULTIPAGE_SetTabWidth(hItem, 364, 0);
		MULTIPAGE_SetTabWidth(hItem, 364, 1);
		MULTIPAGE_SelectPage(hItem, 0);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_MULTIPAGE_ETH:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
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

static void _cbDialogETH(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[17];
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_IP);
		TEXT_SetText(hItem, GUI_LANG_GetText(66));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MASK);
		TEXT_SetText(hItem, GUI_LANG_GetText(68));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_GATE);
		TEXT_SetText(hItem, GUI_LANG_GetText(69));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PORT);
		TEXT_SetText(hItem, GUI_LANG_GetText(67));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DNS_1);
		TEXT_SetText(hItem, GUI_LANG_GetText(212));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DNS_2);
		TEXT_SetText(hItem, GUI_LANG_GetText(213));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_IP);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 17, "%d.%d.%d.%d", tempEthSettings.IPAddress[0], tempEthSettings.IPAddress[1], tempEthSettings.IPAddress[2],
				tempEthSettings.IPAddress[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MASK);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 17, "%d.%d.%d.%d", tempEthSettings.NetMask[0], tempEthSettings.NetMask[1], tempEthSettings.NetMask[2],
				tempEthSettings.NetMask[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GATE);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 17, "%d.%d.%d.%d", tempEthSettings.IPGateway[0], tempEthSettings.IPGateway[1], tempEthSettings.IPGateway[2],
				tempEthSettings.IPGateway[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_PORT);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 10, "%u", tempEthSettings.IPport);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DNS_1);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 17, "%d.%d.%d.%d", tempEthSettings.primaryDNS[0], tempEthSettings.primaryDNS[1], tempEthSettings.primaryDNS[2],
				tempEthSettings.primaryDNS[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DNS_2);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 17, "%d.%d.%d.%d", tempEthSettings.secondaryDNS[0], tempEthSettings.secondaryDNS[1], tempEthSettings.secondaryDNS[2],
				tempEthSettings.secondaryDNS[3]);
		EDIT_SetText(hItem, GUITextBuffer);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_IP:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:

				setNumKeyboard(pMsg->hWinSrc, EDIT, 17);
				break;
			}
			break;
		case ID_EDIT_MASK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 17);
				break;
			}
			break;
		case ID_EDIT_GATE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 17);
				break;
			}
			break;
		case ID_EDIT_PORT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 7);
				break;
			}
			break;
		case ID_EDIT_DNS_1:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 17);
				break;
			}
			break;
		case ID_EDIT_DNS_2:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 17);
				break;
			}
			break;
		}
		break;

	case WM_POST_PAINT:
		DRAW_RoundedFrame(250, 8, 240, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 53, 240, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 98, 240, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 143, 240, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 188, 240, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(250, 233, 240, 38, 4, 2, GUI_BLUE);
		break;

	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbDialogCOM(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[5];
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BAUDRATE);
		TEXT_SetText(hItem, GUI_LANG_GetText(70));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_BAUDRATE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, "2400");
		DROPDOWN_AddString(hItem, "4800");
		DROPDOWN_AddString(hItem, "9600");
		DROPDOWN_AddString(hItem, "19200");
		DROPDOWN_AddString(hItem, "38400");
		DROPDOWN_AddString(hItem, "57600");
		DROPDOWN_AddString(hItem, "115200");
		DROPDOWN_SetSel(hItem, tempComSettings.ComBaudrate);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PARITY);
		TEXT_SetText(hItem, GUI_LANG_GetText(71));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_PARITY);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, "ODD");
		DROPDOWN_AddString(hItem, "EVEN");
		DROPDOWN_AddString(hItem, "NONE");
		DROPDOWN_SetSel(hItem, tempComSettings.ComParity);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MODBUS_ADDRESS);
		TEXT_SetText(hItem, GUI_LANG_GetText(72));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MODBUS_ADDRESS);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		mini_snprintf(GUITextBuffer, 5, "%u", tempComSettings.MBAddress);
		EDIT_SetText(hItem, GUITextBuffer);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_BAUDRATE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				tempComSettings.ComBaudrate = DROPDOWN_GetSel(pMsg->hWinSrc);
				break;
			}
			break;
		case ID_DROPDOWN_PARITY:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				tempComSettings.ComParity = DROPDOWN_GetSel(pMsg->hWinSrc);
				break;
			}
			break;
		case ID_EDIT_MODBUS_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(250, 98, 239, 38, 4, 2, GUI_BLUE);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbDialogEMAIL(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_EMAIL);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		MULTIPAGE_AddPage(hItem, CreateSetEmailParameters(pMsg->hWin), GUI_LANG_GetText(10));
		MULTIPAGE_AddPage(hItem, CreateSetEmailRecivers(pMsg->hWin), GUI_LANG_GetText(203));
		MULTIPAGE_AddPage(hItem, CreateSetEmailReportParameters(pMsg->hWin), GUI_LANG_GetText(207));
		MULTIPAGE_SetTabWidth(hItem, 243, 0);
		MULTIPAGE_SetTabWidth(hItem, 242, 1);
		MULTIPAGE_SetTabWidth(hItem, 243, 2);
		MULTIPAGE_SelectPage(hItem, 0);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_MULTIPAGE_EMAIL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
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

WM_HWIN CreateSetCommunication(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aSetCommunicationDialogCreate, GUI_COUNTOF(_aSetCommunicationDialogCreate), _cbSetCommunicationDialog, hParent, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(12));
	return hWin;
}

static WM_HWIN CreateSetModbusTCP(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogModbusTCPCreate, GUI_COUNTOF(_aDialogModbusTCPCreate), _cbDialogModbusTCP, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetComETH(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogETHCreate, GUI_COUNTOF(_aDialogETHCreate), _cbDialogETH, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetComRS(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogCOMCreate, GUI_COUNTOF(_aDialogCOMCreate), _cbDialogCOM, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetEmail(WM_HWIN hParent)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogEmailCreate, GUI_COUNTOF(_aDialogEmailCreate), _cbDialogEMAIL, hParent, 0, 0);
	return hWin;
}

static int CheckETHSettings(ETH_SETTINGS *Dst, ETH_SETTINGS *Src)
{
	if (Dst->IPAddress[0] != Src->IPAddress[0] || Dst->IPAddress[1] != Src->IPAddress[1] || Dst->IPAddress[2] != Src->IPAddress[2]
			|| Dst->IPAddress[3] != Src->IPAddress[3] || Dst->IPGateway[0] != Src->IPGateway[0] || Dst->IPGateway[1] != Src->IPGateway[1]
			|| Dst->IPGateway[2] != Src->IPGateway[2] || Dst->IPGateway[3] != Src->IPGateway[3] || Dst->NetMask[0] != Src->NetMask[0]
			|| Dst->NetMask[1] != Src->NetMask[1] || Dst->NetMask[2] != Src->NetMask[2] || Dst->NetMask[3] != Src->NetMask[3]
			|| Dst->primaryDNS[0] != Src->primaryDNS[0] || Dst->primaryDNS[1] != Src->primaryDNS[1] || Dst->primaryDNS[2] != Src->primaryDNS[2]
			|| Dst->primaryDNS[3] != Src->primaryDNS[3] || Dst->primaryDNS[0] != Src->primaryDNS[0]	|| Dst->secondaryDNS[1] != Src->secondaryDNS[1]
			|| Dst->secondaryDNS[2] != Src->secondaryDNS[2] || Dst->secondaryDNS[3] != Src->secondaryDNS[3] || Dst->IPport != Src->IPport)
	{
		return 1;
	}
	else
		return 0;
}

static int CheckRSSettings(COM_SETTINGS *Dst, COM_SETTINGS *Src)
{
	if (Dst->ComBaudrate != Src->ComBaudrate || Dst->ComParity != Src->ComParity || Dst->ComStopBits != Src->ComStopBits || Dst->MBAddress != Src->MBAddress)
	{
		return 1;
	}
	else
		return 0;
}

static int CheckModbusTCPRegisterSettings(MODBUS_TCP_REGISTER *Dst, MODBUS_TCP_REGISTER *Src)
{
	if (Dst->connectionID != Src->connectionID || Dst->deviceAddress != Src->deviceAddress || Dst->number != Src->number
			|| Dst->registerType != Src->registerType)
	{
		return 1;
	}
	else
		return 0;
}

static int CheckModbusTCPServerSettings(MODBUS_TCP_SERVER *Dst, MODBUS_TCP_SERVER *Src)
{
	if (Dst->ip[0] != Src->ip[0] || Dst->ip[1] != Src->ip[1] || Dst->ip[2] != Src->ip[2] || Dst->ip[3] != Src->ip[3] || Dst->mode != Src->mode
			|| Dst->port != Src->port || Dst->mode != Src->mode || Dst->port != Src->port || Dst->frequency != Src->frequency
			|| Dst->responseTimeout != Src->responseTimeout)
	{
		return 1;
	}
	else
		return 0;
}

static int CheckEmailSettings(EMAIL_SETTINGS *Dst, EMAIL_SETTINGS *Src)
{
	if (strcmp(Dst->client.email,Src->client.email))
		return 1;
	if (strcmp(Dst->client.login,Src->client.login))
		return 1;
	if (strcmp(Dst->client.password,Src->client.password))
		return 1;
	if (strcmp(Dst->server.name,Src->server.name))
		return 1;
	if(Dst->server.port != Src->server.port)
		return 1;

	for(int i=0; i<5; ++i)
	{
		if (strcmp(&Dst->client.mailList[i][0],&Src->client.mailList[i][0]))
			return 1;
	}

	if(Dst->event.mode != Src->event.mode || Dst->event.day != Src->event.day || Dst->event.hour != Src->event.hour)
		return 1;

	return 0;
}

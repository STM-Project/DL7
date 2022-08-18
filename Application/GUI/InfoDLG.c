#include <ExpansionCards_typedef.h>
#include <NSMAC.h>
#include "InfoDLG.h"
#include <string.h>
#include "parameters.h"
#include "dtos.h"
#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "usb_host.h"
#include "ExpansionCards.h"
#include "version.h"

#include "ExpansionCards_descriptions.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato60;

extern GUI_CONST_STORAGE GUI_BITMAP bmCLOSE;

#define ID_WINDOW_INFO     				(GUI_ID_USER + 100)
#define ID_TEXT_DEV_NAME     			(GUI_ID_USER + 101)
#define ID_TEXT_DEV_DESCRIPTION   (GUI_ID_USER + 102)
#define ID_TEXT_DEV_DESC_FW     	(GUI_ID_USER + 103)
#define ID_TEXT_DEV_DESC_SERIAL   (GUI_ID_USER + 104)
#define ID_TEXT_DEV_DESC_MAC   		(GUI_ID_USER + 105)
#define ID_TEXT_DEV_DESC_IP     	(GUI_ID_USER + 106)
#define ID_TEXT_DEV_DESC_COM     	(GUI_ID_USER + 107)
#define ID_TEXT_DEV_DESC_MB    		(GUI_ID_USER + 108)
#define ID_TEXT_DEV_DESC_ID     	(GUI_ID_USER + 109)
#define ID_TEXT_DEV_ID     				(GUI_ID_USER + 110)
#define ID_TEXT_DEV_FW     				(GUI_ID_USER + 111)
#define ID_TEXT_DEV_SERIAL     		(GUI_ID_USER + 112)
#define ID_TEXT_DEV_MAC     			(GUI_ID_USER + 113)
#define ID_TEXT_DEV_IP     				(GUI_ID_USER + 114)
#define ID_TEXT_DEV_COM     			(GUI_ID_USER + 115)
#define ID_TEXT_DEV_MB     				(GUI_ID_USER + 116)
#define ID_BUTTON_INFO_MORE   		(GUI_ID_USER + 117)
#define ID_BUTTON_INFO_HARDWARE   (GUI_ID_USER + 118)#define ID_TEXT_DEV_DESC_PORT     (GUI_ID_USER + 119)
#define ID_TEXT_DEV_PORT    			(GUI_ID_USER + 120)

#define ID_FRAMEWIN_INFO_MORE  		(GUI_ID_USER + 121)
#define ID_MULTIEDIT_INFO_MORE   	(GUI_ID_USER + 122)

#define ID_FRAMEWIN_INFO_HARDWARE (GUI_ID_USER + 123)
#define ID_LISTVIEW_INFO_HARDWARE (GUI_ID_USER + 124)

extern uint32_t ComSetBaudrate(short Baudrate);

void CreateInfoMore(WM_HWIN hParent);
void CreateInfoHardware(WM_HWIN hParent);

char InfoMoreText[600] __attribute__ ((section(".sdram")));
unsigned int bytesreadFromDescFile;

void loadDeviceDescpyptionToRAM(void)
{
	FIL descFile;
	FILINFO fno;

	if (f_open(&descFile, "0:config/desc.txt", FA_OPEN_ALWAYS | FA_READ) == FR_OK)
	{
		f_stat("0:config/desc.txt",&fno);
		memset(InfoMoreText, 0, 600);
		f_read(&descFile, InfoMoreText, fno.fsize, &bytesreadFromDescFile);
		f_close(&descFile);
	}
}

void saveDeviceDescpyptionToSD(void)
{
	FIL descFile;
	if (f_open(&descFile, "0:config/desc.txt", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
	{
		bytesreadFromDescFile = 0;
		f_write(&descFile, InfoMoreText, strlen(InfoMoreText), &bytesreadFromDescFile);
		f_close(&descFile);
	}
}

/***********************************************************************/

static const GUI_WIDGET_CREATE_INFO _aInfoMoreDialogCreate[] =
		{
				{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_INFO_MORE, 25, 25, 690, 370, 0, 0x0, 0 },
				{ MULTIEDIT_CreateIndirect, "", ID_MULTIEDIT_INFO_MORE, 5, 34, 682, 332, 0, 0x64, 0 },
				{ BUTTON_CreateIndirect, "", GUI_ID_CLOSE, 654, 0, 32, 34, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", GUI_ID_HELP, 0, 0, 654, 34, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
		};

static const GUI_WIDGET_CREATE_INFO _aInfoHardwareDialogCreate[] =
		{
				{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_INFO_HARDWARE, 25, 25, 690, 370, 0, 0x0, 0 },
				{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_INFO_HARDWARE, 3, 35, 680, 335, 0, 0x0, 0 },
				{ BUTTON_CreateIndirect, "", GUI_ID_CLOSE, 654, 0, 32, 34, 0, 0x0, 0 },
				{ TEXT_CreateIndirect, "", GUI_ID_HELP, 0, 0, 654, 34, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
		};

static const GUI_WIDGET_CREATE_INFO _aInfoDialogCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_INFO, 0, 60, 740, 420, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_NAME, 280, 10, 140, 60, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_DESCRIPTION, 10, 70, 680, 30, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "Firmware", ID_TEXT_DEV_DESC_FW, 10, 190, 230, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_FW, 250, 190, 140, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_DESC_SERIAL, 10, 160, 230, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_SERIAL, 250, 160, 120, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "MAC", ID_TEXT_DEV_DESC_MAC, 410, 160, 50, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_MAC, 470, 160, 200, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_DESC_ID, 10, 130, 230, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_ID, 250, 130, 100, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_DESC_IP, 10, 240, 230, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_IP, 250, 240, 170, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_DESC_PORT, 430, 240, 200, 30, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_PORT, 645, 240, 70, 30, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "COM (RS485)", ID_TEXT_DEV_DESC_COM, 10, 290, 230, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_COM, 250, 290, 200, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_DESC_MB, 10, 330, 230, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEV_MB, 250, 330, 100, 30, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_INFO_MORE, 558, 360, 180, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "Hardware", ID_BUTTON_INFO_HARDWARE, 376, 360, 180, 58, 0, 0x0, 0 }, };

/***********************************************************************/
static void _cbInfoMoreDialog(WM_MESSAGE * pMsg)
{
	static WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetBarColor(hItem, 0, GUI_BLUE);
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_BLUE);
		FRAMEWIN_SetBorderSize(hItem, 1);
		FRAMEWIN_SetTitleVis(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CLOSE);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED, GUI_BLUE);
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmapEx(hItem, 0, &bmCLOSE, 0, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_HELP);
		TEXT_SetText(hItem, "Info+");
		TEXT_SetTextColor(hItem, GUI_WHITE);
		TEXT_SetBkColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_INFO_MORE);
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		MULTIEDIT_SetReadOnly(hItem, 1);
		MULTIEDIT_SetWrapWord(hItem);
		MULTIEDIT_SetSkin(hItem, MULTIEDIT_CI_READONLY);
		MULTIEDIT_SetText(hItem, InfoMoreText);
		MULTIEDIT_SetCursorOffset(hItem, MULTIEDIT_GetTextSize(hItem));
		MULTIEDIT_SetInsertMode(hItem, 0);
		MULTIEDIT_SetAutoScrollV(hItem, 1);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case GUI_ID_CLOSE:

			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_MULTIEDIT_INFO_MORE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() >= ADMIN_LEVEL)
				{
					BUZZER_Beep();
					setKeyboard(pMsg->hWinSrc, MULTIEDIT, PASSOFF, 300);
				}
				break;
			}
			break;
		}
		break;

	case WM_DELETE:
		BUZZER_Beep();
		if (PASSWORDS_GetCurrentLevel() >= ADMIN_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_INFO_MORE);
			memset(InfoMoreText, 0, 600);
			MULTIEDIT_GetText(hItem, InfoMoreText, MULTIEDIT_GetTextSize(hItem));
			saveDeviceDescpyptionToSD();
		}
		break;

	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbInfoHardwareDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int i;
	int NCode;
	int Id;
	char GUITextBuffer[20];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetBarColor(hItem, 0, GUI_BLUE);
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_BLUE);
		FRAMEWIN_SetBorderSize(hItem, 1);
		FRAMEWIN_SetText(hItem, "Hardware");
		FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER);
		FRAMEWIN_SetTitleVis(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CLOSE);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED, GUI_BLUE);
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmapEx(hItem, 0, &bmCLOSE, 0, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_HELP);
		TEXT_SetText(hItem, "Hardware");
		TEXT_SetTextColor(hItem, GUI_WHITE);
		TEXT_SetBkColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_INFO_HARDWARE);
		LISTVIEW_AddColumn(hItem, 22, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 110, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 426, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 100, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 22, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 47);
		LISTVIEW_SetHeaderHeight(hItem, 0);
		LISTVIEW_SetGridVis(hItem, 0);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_AddRow(hItem, NULL);
		WM_DisableWindow(hItem);

		for (i = 0; i < 7; i++)
		{
			mini_snprintf(GUITextBuffer, 20, "SLOT %c", 'A' + i);
			LISTVIEW_SetItemText(hItem, 1, i, GUITextBuffer);
			LISTVIEW_SetItemTextColor(hItem, 0, i, 0, GUI_BLUE);
			LISTVIEW_SetItemText(hItem, 2, i, GetExpansionCardDescription(ExpansionCards[i].type));

			if (ExpansionCards[i].status == 1)
			{
				LISTVIEW_SetItemBkColor(hItem, 0, i, 0, 0x0000c000);
				LISTVIEW_SetItemBkColor(hItem, 4, i, 0, 0x0000c000);
				float2stri(GUITextBuffer,(float) ExpansionCards[i].firmwareRevision / 100,2);
				LISTVIEW_SetItemText(hItem, 3, i, GUITextBuffer);
			}
			else if (ExpansionCards[i].status == 2 || ExpansionCards[i].status == 3)
			{
				LISTVIEW_SetItemBkColor(hItem, 4, i, 0, GUI_YELLOW);
				LISTVIEW_SetItemBkColor(hItem, 0, i, 0, GUI_YELLOW);
				float2stri(GUITextBuffer,(float) ExpansionCards[i].firmwareRevision / 100,2);
				LISTVIEW_SetItemText(hItem, 3, i, GUITextBuffer);
			}
			else
			{
				LISTVIEW_SetItemBkColor(hItem, 4, i, 0, GUI_RED);
				LISTVIEW_SetItemBkColor(hItem, 0, i, 0, GUI_RED);
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

static void _cbInfoDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[31];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_NAME);
		TEXT_SetFont(hItem, &GUI_FontLato60);
		TEXT_SetText(hItem, NAME);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESCRIPTION);
		TEXT_SetText(hItem, GeneralSettings.DeviceDescription);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_FW);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_FW);
		TEXT_SetText(hItem, VERSION);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_SERIAL);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(77));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_SERIAL);
		mini_snprintf(GUITextBuffer, 9, "%u", deviceSerialNumber);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_MAC);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_MAC);
		mini_snprintf(GUITextBuffer, 31, "%02X:%02X:%02X:%02X:%02X:%02X", deviceMACAddress[0], deviceMACAddress[1], deviceMACAddress[2], deviceMACAddress[3],
				deviceMACAddress[4], deviceMACAddress[5]);
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetText(hItem, GUITextBuffer);


		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_IP);
		mini_snprintf(GUITextBuffer, 21, "%d.%d.%d.%d", EthSettings.IPAddress[0], EthSettings.IPAddress[1], EthSettings.IPAddress[2], EthSettings.IPAddress[3]);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_PORT);
		TEXT_SetText(hItem, GUI_LANG_GetText(67));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_PORT);
		mini_snprintf(GUITextBuffer, 9, "%u", EthSettings.IPport);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_COM);
		switch (ComSettings.ComParity)
		{
		case 0:
			mini_snprintf(GUITextBuffer, 21, "%ubps (ODD)", (unsigned int) ComSetBaudrate(ComSettings.ComBaudrate));
			break;
		case 1:
			mini_snprintf(GUITextBuffer, 21, "%ubps (EVEN)", (unsigned int) ComSetBaudrate(ComSettings.ComBaudrate));
			break;
		case 2:
			mini_snprintf(GUITextBuffer, 21, "%ubps (NONE)", (unsigned int) ComSetBaudrate(ComSettings.ComBaudrate));
			break;
		}
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_IP);
		TEXT_SetText(hItem, GUI_LANG_GetText(66));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_COM);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_MB);
		TEXT_SetText(hItem, GUI_LANG_GetText(72));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_DESC_ID);
		TEXT_SetText(hItem, GUI_LANG_GetText(51));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_ID);
		mini_snprintf(GUITextBuffer,3, "%d", GeneralSettings.DeviceID);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEV_MB);
		mini_snprintf(GUITextBuffer,4, "%d", ComSettings.MBAddress);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_INFO_MORE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(13));
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_INFO_MORE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateInfoMore(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_INFO_HARDWARE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateInfoHardware(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_Line(20, 120, 720, 120, 1, GUI_GRAY);
		DRAW_Line(20, 230, 720, 230, 1, GUI_GRAY);
		DRAW_Line(20, 280, 720, 280, 1, GUI_GRAY);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

void CreateInfoMore(WM_HWIN hParent)
{
	loadDeviceDescpyptionToRAM();
	GUI_CreateDialogBox(_aInfoMoreDialogCreate, GUI_COUNTOF(_aInfoMoreDialogCreate), _cbInfoMoreDialog, hParent, 0, 0);
}

void CreateInfoHardware(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aInfoHardwareDialogCreate, GUI_COUNTOF(_aInfoHardwareDialogCreate),
			_cbInfoHardwareDialog, hParent, 0, 0);
	WM_MakeModal(hWin);
}

WM_HWIN CreateInfo(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aInfoDialogCreate, GUI_COUNTOF(_aInfoDialogCreate), _cbInfoDialog, WM_HBKWIN, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(1));
	return hWin;
}

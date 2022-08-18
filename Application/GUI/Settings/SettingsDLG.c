#include "SettingsDLG.h"
#include "graphics.h"
#include "skins.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "parameters.h"
#include "fram.h"

#include "SetLoginDLG.h"
#include "SetGeneralDLG.h"
#include "SetIOBoardsDLG.h"
#include "SetCommunicationDLG.h"
#include "SetChannelDLG.h"
#include "SetUserScreensDLG.h"
#include "SetArchiveDLG.h"
#include "SetUSBDLG.h"
#include "SetConfirmDLG.h"
#include "WindowManagement.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmERR;

#define ID_WINDOW_SETTINGS 		(GUI_ID_USER + 101)
#define ID_ICONVIEW_SETTINGS 	(GUI_ID_USER + 102)

WM_HWIN hSetWindow;

BYTE bufferForLoginSetDTAGraphic[2072] __attribute__ ((section(".sdram")));
UINT bytesreadFromLoginSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForChannelSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromChannelSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForIOSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromIOSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForGeneralSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromGeneralSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForArchiveSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromArchiveSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForScreenSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromScreenSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForCommunicationSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromCommunicationSetDTA __attribute__ ((section(".sdram")));

BYTE bufferForUSBSetDTAGraphic[32416] __attribute__ ((section(".sdram")));
UINT bytesreadFromUSBSetDTA __attribute__ ((section(".sdram")));

static const GUI_WIDGET_CREATE_INFO _aSettingsDialogCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SETTINGS, 0, 60, 740, 420, 0, 0x0, 0 }, };

static void _cbSettingsDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	uint8_t tempWindowsState =0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		NewSettings = 0;
		RestartDevice = 0;
		NewArchive = 0;
		NewUserChar = 0;

		ICONVIEW_CreateEx(0, 0, 740, 420, pMsg->hWin, WM_CF_SHOW, 0, ID_ICONVIEW_SETTINGS, 185, 180);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_SETTINGS);
		ICONVIEW_SetSpace(hItem, GUI_COORD_Y, 30);
		ICONVIEW_SetSpace(hItem, GUI_COORD_X, 0);
		ICONVIEW_SetFrame(hItem, GUI_COORD_X, 0);
		ICONVIEW_SetFrame(hItem, GUI_COORD_Y, 0);
		ICONVIEW_SetFont(hItem, &GUI_FontLato30);
		ICONVIEW_SetSkinColors(hItem);
		ICONVIEW_EnableStreamAuto();

		if (bytesreadFromGeneralSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForGeneralSetDTAGraphic, GUI_LANG_GetText(10));
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromIOSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForIOSetDTAGraphic, "I/O");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromCommunicationSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForCommunicationSetDTAGraphic, GUI_LANG_GetText(12));
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromChannelSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForChannelSetDTAGraphic, GUI_LANG_GetText(62));
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromScreenSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForScreenSetDTAGraphic, GUI_LANG_GetText(11));
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromArchiveSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForArchiveSetDTAGraphic, GUI_LANG_GetText(4));
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromUSBSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForUSBSetDTAGraphic, "USB");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromLoginSetDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForLoginSetDTAGraphic, "EXIT");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (NCode)
		{
		case WM_NOTIFICATION_RELEASED:
			switch (Id)
			{
			case ID_ICONVIEW_SETTINGS:
				BUZZER_Beep();
				switch (ICONVIEW_GetSel(pMsg->hWinSrc))
				{
				case 0:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetGeneral(pMsg->hWin);
					break;
				case 1:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetIOBoardsWin(pMsg->hWin);
					break;
				case 2:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetCommunication(pMsg->hWin);
					break;
				case 3:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetChannels(pMsg->hWin);
					break;
				case 4:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetUserScreens(pMsg->hWin);
					break;
				case 5:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetArchive(pMsg->hWin);
					break;
				case 6:
					if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
						CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
					else
						hSetWindow = CreateSetUSB(pMsg->hWin);
					break;
				case 7:
					tempWindowsState = FRAM_Read(WINDOWS_STATE_ADDR);

					if (NewSettings)
					{
						CreateSetConfirm(WM_HBKWIN, tempWindowsState);
					}
					else
					{
						ChangeWindow(pMsg->hWin, tempWindowsState);
					}
					break;
				}
				break;
			}
			break;

		case WM_NOTIFICATION_CHILD_DELETED:
			if(hSetWindow == pMsg->hWinSrc)
			{
				USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(5));
				hSetWindow = 0;
			}

			break;
		}
		break;
	case WM_USER_LOGIN:
		if (pMsg->Data.v == NO_USER && hSetWindow != 0)
		{
			WM_DeleteWindow(hSetWindow);
			hSetWindow = 0;
		}
		break;
	case WM_USER_REFRESH_LANG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_SETTINGS);
		ICONVIEW_SetItemText(hItem, 0, GUI_LANG_GetText(7));
		ICONVIEW_SetItemText(hItem, 1, GUI_LANG_GetText(10));
		ICONVIEW_SetItemText(hItem, 3, GUI_LANG_GetText(12));
		ICONVIEW_SetItemText(hItem, 4, GUI_LANG_GetText(62));
		ICONVIEW_SetItemText(hItem, 5, GUI_LANG_GetText(11));
		ICONVIEW_SetItemText(hItem, 6, GUI_LANG_GetText(4));
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());
		hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_SETTINGS);
		ICONVIEW_SetSkinColors(hItem);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSettings(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSettingsDialogCreate, GUI_COUNTOF(_aSettingsDialogCreate), _cbSettingsDialog, WM_HBKWIN, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(5));
	return hWin;
}

void loadSettingsWindowLightGraphicsToRAM()
{
	bytesreadFromLoginSetDTA = LoadGraphicFileToRAM("0:image/exit.dta", bufferForLoginSetDTAGraphic);
	bytesreadFromChannelSetDTA = LoadGraphicFileToRAM("0:image/lch.dta", bufferForChannelSetDTAGraphic);
	bytesreadFromIOSetDTA = LoadGraphicFileToRAM("0:image/lio.dta", bufferForIOSetDTAGraphic);
	bytesreadFromGeneralSetDTA = LoadGraphicFileToRAM("0:image/lgeneral.dta", bufferForGeneralSetDTAGraphic);
	bytesreadFromArchiveSetDTA = LoadGraphicFileToRAM("0:image/larch.dta", bufferForArchiveSetDTAGraphic);
	bytesreadFromScreenSetDTA = LoadGraphicFileToRAM("0:image/ltabs.dta", bufferForScreenSetDTAGraphic);
	bytesreadFromCommunicationSetDTA = LoadGraphicFileToRAM("0:image/lcom.dta", bufferForCommunicationSetDTAGraphic);
	bytesreadFromUSBSetDTA = LoadGraphicFileToRAM("0:image/lusb.dta", bufferForUSBSetDTAGraphic);
}

void loadSettingsWindowDarkGraphicsToRAM()
{
	bytesreadFromLoginSetDTA = LoadGraphicFileToRAM("0:image/exit.dta", bufferForLoginSetDTAGraphic);
	bytesreadFromChannelSetDTA = LoadGraphicFileToRAM("0:image/dch.dta", bufferForChannelSetDTAGraphic);
	bytesreadFromIOSetDTA = LoadGraphicFileToRAM("0:image/dio.dta", bufferForIOSetDTAGraphic);
	bytesreadFromGeneralSetDTA = LoadGraphicFileToRAM("0:image/dgeneral.dta", bufferForGeneralSetDTAGraphic);
	bytesreadFromArchiveSetDTA = LoadGraphicFileToRAM("0:image/darch.dta", bufferForArchiveSetDTAGraphic);
	bytesreadFromScreenSetDTA = LoadGraphicFileToRAM("0:image/dtabs.dta", bufferForScreenSetDTAGraphic);
	bytesreadFromCommunicationSetDTA = LoadGraphicFileToRAM("0:image/dcom.dta", bufferForCommunicationSetDTAGraphic);
	bytesreadFromUSBSetDTA = LoadGraphicFileToRAM("0:image/dusb.dta", bufferForUSBSetDTAGraphic);
}

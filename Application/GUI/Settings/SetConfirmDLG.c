#include <channels.h>
#include <ExpansionCards.h>
#include "SetConfirmDLG.h"
#include <string.h>
#include "parameters.h"
#include "skins.h"
#include "archive.h"

#include "WindowManagement.h"
#include "buzzer.h"
#include "fram.h"
#include "passwords.h"
#include "backlight.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;
extern GUI_CONST_STORAGE GUI_BITMAP bmCLOSE;

#define ID_FRAMEWIN_0  		(GUI_ID_USER + 0x302)
#define ID_MESSAGE_TEXT 	(GUI_ID_USER + 0x303)
#define ID_BUTTON_OK    	(GUI_ID_USER + 0x304)
#define ID_BUTTON_CANCEL  (GUI_ID_USER + 0x305)

uint8_t SelectedNewWindow;

static void selectNewWindow(int selection, WM_HWIN hWin)
{
	switch (selection)
	{
	case 0:
		WindowInstance = 99;
		while (CheckChannelType(WindowInstance) == 0)
		{
			WindowInstance--;
			if (WindowInstance < 0)
				hActualWindow = ChangeWindow(hActualWindow, INFO);
			break;
		}
		if (WindowInstance >= 0)
		{
			WM_DeleteWindow(hWin);
			hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
		}
		break;
	case 1:
		WM_DeleteWindow(hWin);
		hActualWindow = ChangeWindow(hActualWindow, INFO);
		break;
	case 2:
		WM_DeleteWindow(hWin);
		hActualWindow = ChangeWindow(hActualWindow, USERVALUES);
		break;
	case 3:
		WM_DeleteWindow(hWin);
		hActualWindow = ChangeWindow(hActualWindow, USERTRENDS);
		break;
	case 4:
		WM_DeleteWindow(hWin);
		hActualWindow = ChangeWindow(hActualWindow, ARCHIVE);
		break;
	case 6:
		WindowInstance = 0;
		while (CheckChannelType(WindowInstance) == 0)
		{
			WindowInstance++;
			if (WindowInstance > 99)
				hActualWindow = ChangeWindow(hActualWindow, INFO);
			break;
		}
		if (WindowInstance <= 99)
		{
			WM_DeleteWindow(hWin);
			hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
		}
		break;
	case 7:
		WM_DeleteWindow(hWin);
		hActualWindow = ChangeWindow(hActualWindow, ALARMS);
		break;
	default:
		break;
	}
}

/***********************************************************************/
static const GUI_WIDGET_CREATE_INFO _aSetConfirmDialog[] =
{
{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_0, 100, 110, 540, 200, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_MESSAGE_TEXT, 2, 40, 532, 90, GUI_TA_HCENTER | GUI_TA_VERTICAL, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 2, 137, 265, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 269, 137, 265, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", GUI_ID_CLOSE, 504, 0, 32, 34, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", GUI_ID_HELP, 0, 0, 504, 34, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 }, };

/***********************************************************************/
static void _cbSetConfirmDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[150];
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetBarColor(hItem, 0, GUI_BLUE);
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_BLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);
		FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CLOSE);
		BUTTON_SetSkinClassic(hItem);
		BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED, GUI_BLUE);
		BUTTON_SetBitmapEx(hItem, 0, &bmCLOSE, 0, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_HELP);
		TEXT_SetText(hItem, GUI_LANG_GetText(75));
		TEXT_SetTextColor(hItem, GUI_WHITE);
		TEXT_SetBkColor(hItem, GUI_BLUE);

		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MESSAGE_TEXT);
			mini_snprintf(GUITextBuffer, 150, "%s\n%s", GUI_LANG_GetText(179), GUI_LANG_GetText(180));
			TEXT_SetText(hItem, GUITextBuffer);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
			WM_MoveChildTo(hItem, 136, 137);
			BUTTON_SetBitmapEx(hItem, 0, &bmOK, 113, 16);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MESSAGE_TEXT);

			if (RestartDevice && NewArchive)
				mini_snprintf(GUITextBuffer, 150, "%s\n%s\n%s", GUI_LANG_GetText(76), GUI_LANG_GetText(170), GUI_LANG_GetText(176));
			else if (RestartDevice)
				mini_snprintf(GUITextBuffer, 150, "%s\n%s", GUI_LANG_GetText(76), GUI_LANG_GetText(170));
			else if (NewArchive)
				mini_snprintf(GUITextBuffer, 150, "%s\n%s", GUI_LANG_GetText(76), GUI_LANG_GetText(176));
			else
				mini_snprintf(GUITextBuffer, 150, "%s", GUI_LANG_GetText(76));
			TEXT_SetText(hItem, GUITextBuffer);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
			BUTTON_SetBitmapEx(hItem, 0, &bmOK, 113, 16);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
			BUTTON_SetBitmapEx(hItem, 0, &bmNO, 119, 16);
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
				BUZZER_Beep();
				WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;

		case ID_BUTTON_OK:

			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				if (PASSWORDS_GetCurrentLevel() >= ADMIN_LEVEL)
					WM_MakeModal(0);
				break;

			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() >= ADMIN_LEVEL)
				{
					BUZZER_Beep();
					ARCHIVE_SaveBuffersToSDCard();

					FRAM_Write(NEW_PARAMETERS_ADDR, 1);
					saveParameters();
					ARCHIVE_SendEvent("SYS:NEW SETTINGS");

					if (RestartDevice)
					{
						if (NewArchive)
							FRAM_Write(NEW_ARCHIVE_FLAG_ADDR, 1);

						GUI_Delay(500);
						HAL_NVIC_SystemReset();
					}
					else
					{
						strncpy(GeneralSettings.DeviceDescription, bkGeneralSettings.DeviceDescription, 81);
						GeneralSettings.Brightness = bkGeneralSettings.Brightness;
						GeneralSettings.ScreenSaverBrightness = bkGeneralSettings.ScreenSaverBrightness;
						GeneralSettings.ScreenSaverTime = bkGeneralSettings.ScreenSaverTime;
						GeneralSettings.BeeperMode = bkGeneralSettings.BeeperMode;
						GeneralSettings.AlarmBeeperMode = bkGeneralSettings.AlarmBeeperMode;

						ArchiveSettings.ArchivizationFrequency1 = bkArchiveSettings.ArchivizationFrequency1;
						ArchiveSettings.ArchivizationFrequency2 = bkArchiveSettings.ArchivizationFrequency2;
						ArchiveSettings.TotalizerArchivizationFrequency = bkArchiveSettings.TotalizerArchivizationFrequency;
						ArchiveSettings.ArchiveMaxSize = bkArchiveSettings.ArchiveMaxSize;
						ArchiveSettings.canUserStopArchive = bkArchiveSettings.canUserStopArchive;

						if (NewArchive)
						{
							if (ARCHIVE_GetState())
							{
								ARCHIVE_SafeStop();
								ARCHIVE_CreateNewArchives();
								ARCHIVE_SafeStart();
							}
							else
								ARCHIVE_CreateNewArchives();
						}
						for (int i = 0; i < 6; ++i)
							CopyUserTrendsSettings(&UserTrendsSettings[i], &bkUserTrendsSettings[i]);
						CopyUserTablesSettings(&UserTablesSettings, &bkUserTablesSettings);

						for (int i = 0; i < 100; ++i)
						{
							Channels[i].graph_max_value = bkChannels[i].graph_max_value;
							Channels[i].graph_min_value = bkChannels[i].graph_min_value;
							Channels[i].alarm[0].color = bkChannels[i].alarm[0].color;
							Channels[i].alarm[1].color = bkChannels[i].alarm[1].color;
						}
						selectNewWindow(SelectedNewWindow, pMsg->hWin);
					}
				}
				break;
			}
			break;
		case ID_BUTTON_CANCEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
					WM_MakeModal(0);
				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				CopyGeneralSettings(&bkGeneralSettings, &GeneralSettings);
				CopyArchiveSettings(&bkArchiveSettings, &ArchiveSettings);

				CopyEthSettings(&bkEthSettings, &EthSettings);
				CopyComSettings(&bkComSettings, &ComSettings);

				for (int i = 0; i < 6; ++i)
					CopyUserTrendsSettings(&bkUserTrendsSettings[i], &UserTrendsSettings[i]);
				CopyUserTablesSettings(&bkUserTablesSettings, &UserTablesSettings);

				CopyUserCharSettings(bkUserCharacteristics, UserCharacteristcs);

				for (int i = 0; i < 7; ++i)
					bkExpansionCards[i] = ExpansionCards[i];

				for (int i = 0; i < 100; ++i)
					bkChannels[i] = Channels[i];

				BACKLIGHT_Set(bkGeneralSettings.Brightness);
				SetLanguage(bkGeneralSettings.DeviceLanguage);
				SKINS_UpdateCalendar();

				selectNewWindow(SelectedNewWindow, pMsg->hWin);
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

void CreateSetConfirm(WM_HWIN hParent, uint8_t SelectedWindow)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetConfirmDialog, GUI_COUNTOF(_aSetConfirmDialog), _cbSetConfirmDialog, hParent, 0, 60);
	WM_MakeModal(hWin);
	WM_SetFocus(hWin);
	SelectedNewWindow = SelectedWindow;
}

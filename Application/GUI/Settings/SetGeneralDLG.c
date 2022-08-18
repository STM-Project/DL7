#include "SetGeneralDLG.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parameters.h"
#include "rtc.h"
#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "backlight.h"
#include "archive.h"
#include "passwords.h"

#include "CalibrationMenuDLG.h"
#include "serialNumberDLG.h"
#include "WindowManagement.h"
#include "mini-printf.h"
#include "fram.h"
#include "EDIT_double.h"

#include "SetFirmwareUpdateDLG.h"
#include "SetTouchPanelCalibrationDLG.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_SET_GENERAL 			(GUI_ID_USER + 500)
#define ID_MULTIPAGE_SET_GENRAL 		(GUI_ID_USER + 501)
#define ID_BUTTON_OK 								(GUI_ID_USER + 502)
#define ID_BUTTON_CANCEL						(GUI_ID_USER + 503)

#define ID_WINDOW_GENERAL 						(GUI_ID_USER + 504)
#define ID_TEXT_LANG 									(GUI_ID_USER + 505)
#define ID_DROPDOWN_LANG 							(GUI_ID_USER + 506)
#define ID_TEXT_DEVICE_DESCRIPTION 		(GUI_ID_USER + 507)
#define ID_EDIT_DEVICE_DESCRIPTION 		(GUI_ID_USER + 508)
#define ID_TEXT_DEVICE_ID 						(GUI_ID_USER + 509)
#define ID_EDIT_DEVICE_ID 						(GUI_ID_USER + 510)
#define ID_TEXT_BEEPER 								(GUI_ID_USER + 511)
#define ID_CHECKBOX_BEEPER 						(GUI_ID_USER + 512)
#define ID_TEXT_ALARM 								(GUI_ID_USER + 513)
#define ID_CHECKBOX_ALARM 						(GUI_ID_USER + 514)
#define ID_TEXT_DST 									(GUI_ID_USER + 515)
#define ID_CHECKBOX_DST 							(GUI_ID_USER + 516)

#define ID_WINDOW_SCREEN 						(GUI_ID_USER + 517)
#define ID_TEXT_BRIGHTNESS 					(GUI_ID_USER + 518)
#define ID_TEXT_BRIGHTNESS_UNIT 		(GUI_ID_USER + 519)
#define ID_TEXT_SCREENSAVER_MODE 		(GUI_ID_USER + 520)
#define ID_TEXT_SCREENSAVER_TIME 		(GUI_ID_USER + 521)
#define ID_TEXT_SCREENSAVER_UNIT 		(GUI_ID_USER + 522)
#define ID_RADIO_SCREENSAVER_MODE  	(GUI_ID_USER + 523)
#define ID_SLIDER_BRIGHTNESS 				(GUI_ID_USER + 524)
#define ID_SLIDER_SCREENSAVER_TIME 	(GUI_ID_USER + 525)
#define ID_EDIT_BRIGHTNESS 					(GUI_ID_USER + 526)
#define ID_EDIT_SCREENSAVER 				(GUI_ID_USER + 527)
#define ID_TEXT_BACKGROUND_COLOR 		(GUI_ID_USER + 528)
#define ID_RADIO_BACKGROUND_COLOR 	(GUI_ID_USER + 529)

#define ID_WINDOW_TIME_AND_DATE 		(GUI_ID_USER + 530)
#define ID_LISTWHEEL_HOURS 					(GUI_ID_USER + 531)
#define ID_LISTWHEEL_MINUTES 				(GUI_ID_USER + 532)
#define ID_LISTWHEEL_SECONDS 				(GUI_ID_USER + 533)
#define ID_BUTTON_SET_TIME 					(GUI_ID_USER + 534)
#define ID_BUTTON_SET_DATE 					(GUI_ID_USER + 535)
#define ID_CALENDAR 								(GUI_ID_USER + 536)

#define ID_TEXT_TIME_CORRECTION					(GUI_ID_USER + 537)
#define ID_EDIT_TIME_CORRECTION					(GUI_ID_USER + 538)
#define ID_BUTTON_SET_TIME_CORRECTION		(GUI_ID_USER + 539)
#define ID_TEXT_TIME_CORRECTION_UNIT		(GUI_ID_USER + 540)

#define ID_WINDOW_SERVICE 					(GUI_ID_USER + 541)
#define ID_BUTTON_RESTART 					(GUI_ID_USER + 542)
#define ID_BUTTON_SET_CALIBRATION 	(GUI_ID_USER + 543)
#define ID_BUTTON_RESTORE_FACTORY 	(GUI_ID_USER + 544)
#define ID_BUTTON_SET_SNMAC 				(GUI_ID_USER + 545)

#define ID_BUTTON_HART_CONFIG 						(GUI_ID_USER + 546)
#define ID_BUTTON_BOARD_FIRMWARE_UPDATE	 	(GUI_ID_USER + 547)

#define ID_BUTTON_SET_FIRMWARE_UPDATE			(GUI_ID_USER + 548)
#define ID_BUTTON_SET_TP_CALIBRATION	(GUI_ID_USER + 549)

#define TIME_WHEELS_SNAP_POSITION	45

static WM_HWIN CreateLanguageWindow(WM_HWIN hParent);
static WM_HWIN CreateTimeAndDateWindow(WM_HWIN hParent);
static WM_HWIN CreateDisplayWindow(WM_HWIN hParent);
static WM_HWIN CreateServiceWindow(WM_HWIN hParent);

static GENERAL_SETTINGS tempGeneralSettings __attribute__ ((section(".sdram")));
static CALENDAR_DATE data_set __attribute__ ((section(".sdram")));

static WM_HWIN hSetGeneral;

const char *time[] =
{ "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25",
		"26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51",
		"52", "53", "54", "55", "56", "57", "58", "59" };

static const GUI_WIDGET_CREATE_INFO _aGeneralMainWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_GENERAL, 0, 0, 740, 420, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_SET_GENRAL, 5, 2, 730, 340, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 496, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 618, 360, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aGeneralLanguageWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_GENERAL, 0, 0, 730, 305, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_LANG, 10, 10, 310, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_LANG, 320, 10, 400, 210, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_DEVICE_DESCRIPTION, 10, 55, 300, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_DEVICE_DESCRIPTION, 325, 55, 390, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 40 },

{ TEXT_CreateIndirect, "", ID_TEXT_DEVICE_ID, 10, 100, 310, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_DEVICE_ID, 325, 100, 100, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_BEEPER, 10, 145, 310, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_BEEPER, 320, 145, 35, 35, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM, 10, 190, 310, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_ALARM, 320, 190, 35, 35, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_DST, 10, 235, 310, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_DST, 320, 235, 35, 35, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aGeneralDisplayWindow[] =
{

{ WINDOW_CreateIndirect, "", ID_WINDOW_SCREEN, 0, 0, 730, 305, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_BRIGHTNESS, 10, 10, 350, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_BRIGHTNESS, 360, 10, 50, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "%", ID_TEXT_BRIGHTNESS_UNIT, 410, 10, 50, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ SLIDER_CreateIndirect, "", ID_SLIDER_BRIGHTNESS, 20, 45, 460, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SCREENSAVER_MODE, 10, 100, 350, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ RADIO_CreateIndirect, "", ID_RADIO_SCREENSAVER_MODE, 10, 135, 120, 60, 0, 0x1E02, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SCREENSAVER_TIME, 10, 190, 340, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_SCREENSAVER, 360, 190, 50, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "min", ID_TEXT_SCREENSAVER_UNIT, 410, 190, 50, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ SLIDER_CreateIndirect, "", ID_SLIDER_SCREENSAVER_TIME, 20, 235, 460, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_BACKGROUND_COLOR, 500, 100, 270, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ RADIO_CreateIndirect, "", ID_RADIO_BACKGROUND_COLOR, 500, 135, 270, 60, 0, 0x1E02, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aGeneralTimeAndDateWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_TIME_AND_DATE, 0, 0, 730, 305, 0, 0x0, 0 },
{ LISTWHEEL_CreateIndirect, "", ID_LISTWHEEL_HOURS, 10, 10, 90, 120, 0, 0x0, 0 },
{ LISTWHEEL_CreateIndirect, "", ID_LISTWHEEL_MINUTES, 105, 10, 90, 120, 0, 0x0, 0 },
{ LISTWHEEL_CreateIndirect, "", ID_LISTWHEEL_SECONDS, 200, 10, 90, 120, 0, 0x0, 0 },

{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_TIME, 10, 130, 280, 40, 0, 0x0, 0 },

{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_DATE, 430, 260, 280, 40, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TIME_CORRECTION, 15, 180, 275, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_TIME_CORRECTION, 15, 215, 128, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TIME_CORRECTION_UNIT, 148, 215, 137, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_TIME_CORRECTION, 10, 260, 280, 40, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aServiceWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SERVICE, 0, 0, 730, 305, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "Restart", ID_BUTTON_RESTART, 5, 10, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_RESTORE_FACTORY, 370, 10, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_HART_CONFIG, 5, 60, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_TP_CALIBRATION, 370, 60, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_FIRMWARE_UPDATE, 5, 110, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_BOARD_FIRMWARE_UPDATE,  370, 110, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_CALIBRATION, 5, 160, 355, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_SET_SNMAC, 370, 160, 355, 40, 0, 0x0, 0 },


};

void SERVICE_RestartDevice(void)
{
	ARCHIVE_SaveBuffersToSDCard();
	GUI_Delay(500);
	HAL_NVIC_SystemReset();
}

void SERVICE_RestoreFactoryDefaults(void)
{
	ARCHIVE_SaveBuffersToSDCard();
	RestoreDefaultSettings();
	HAL_NVIC_SystemReset();
}

static void CALENDAR_Refresh(WM_HWIN hWin)
{
	WM_HWIN hItem = WM_GetDialogItem(hWin, ID_CALENDAR);
	CALENDAR_DATE selDate, currentDate;
	CALENDAR_GetSel(hItem, &selDate);
	CALENDAR_GetDate(hItem, &currentDate);
	WM_DeleteWindow(hItem);
	hItem = CALENDAR_Create(hWin, 430, 8, currentDate.Year, currentDate.Month, currentDate.Day, 2, ID_CALENDAR, 0);
	CALENDAR_SetSel(hItem, &selDate);
}

static void _cbMainWindow(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		CopyGeneralSettings(&tempGeneralSettings, &bkGeneralSettings);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SET_GENRAL);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		MULTIPAGE_AddPage(hItem, CreateLanguageWindow(pMsg->hWin), GUI_LANG_GetText(10));
		MULTIPAGE_AddPage(hItem, CreateDisplayWindow(pMsg->hWin), GUI_LANG_GetText(52));
		MULTIPAGE_AddPage(hItem, CreateTimeAndDateWindow(pMsg->hWin), GUI_LANG_GetText(56));
		MULTIPAGE_AddPage(hItem, CreateServiceWindow(pMsg->hWin), GUI_LANG_GetText(80));
		if (PASSWORDS_GetCurrentLevel() >= ADMIN_LEVEL)
		{
			MULTIPAGE_SetTabWidth(hItem, 182, 0);
			MULTIPAGE_SetTabWidth(hItem, 182, 1);
			MULTIPAGE_SetTabWidth(hItem, 182, 2);
			MULTIPAGE_SetTabWidth(hItem, 182, 3);
			MULTIPAGE_EnablePage(hItem, 3);
		}
		else
		{
			MULTIPAGE_DisablePage(hItem, 3);
			MULTIPAGE_SetTabWidth(hItem, 243, 0);
			MULTIPAGE_SetTabWidth(hItem, 243, 1);
			MULTIPAGE_SetTabWidth(hItem, 243, 2);
			MULTIPAGE_SetTabWidth(hItem, 0, 3);
		}

		MULTIPAGE_SelectPage(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_MULTIPAGE_SET_GENRAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SET_GENRAL);
				MULTIPAGE_SelectPage(hItem, MULTIPAGE_GetSelection(hItem));
				break;
			}
			break;
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				if (tempGeneralSettings.DeviceLanguage != bkGeneralSettings.DeviceLanguage)
				{
					SetLanguage(tempGeneralSettings.DeviceLanguage);
					SKINS_UpdateCalendar();

					UserMessage.MsgId = WM_USER_REFRESH_LANG;
					UserMessage.hWin = hActualWindow;
					UserMessage.hWinSrc = pMsg->hWin;
					UserMessage.Data.v = 0;
					WM_SendMessage(hActualWindow, &UserMessage);

					USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(5));
				}

				if (bkGeneralSettings.DeviceID != tempGeneralSettings.DeviceID || bkGeneralSettings.DSTMode != tempGeneralSettings.DSTMode)
				{
					NewSettings = 1;
					RestartDevice = 1;
					NewArchive = 1;
				}
				else if (0 != strncmp(bkGeneralSettings.DeviceDescription, tempGeneralSettings.DeviceDescription, 81)
						|| bkGeneralSettings.AlarmBeeperMode != tempGeneralSettings.AlarmBeeperMode || bkGeneralSettings.BeeperMode != tempGeneralSettings.BeeperMode
						|| bkGeneralSettings.Brightness != tempGeneralSettings.Brightness || bkGeneralSettings.DeviceLanguage != tempGeneralSettings.DeviceLanguage
						|| bkGeneralSettings.ScreenSaverBrightness != tempGeneralSettings.ScreenSaverBrightness
						|| bkGeneralSettings.ScreenSaverTime != tempGeneralSettings.ScreenSaverTime)
				{
					NewSettings = 1;
				}
				CopyGeneralSettings(&bkGeneralSettings, &tempGeneralSettings);
				BACKLIGHT_Set(bkGeneralSettings.Brightness);
				break;
			}
			break;
		case ID_BUTTON_CANCEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				BACKLIGHT_Set(bkGeneralSettings.Brightness);
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_USER_LOGIN:
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SET_GENRAL);
			MULTIPAGE_SelectPage(hItem, 0);
			MULTIPAGE_DisablePage(hItem, 3);
			MULTIPAGE_SetTabWidth(hItem, 243, 0);
			MULTIPAGE_SetTabWidth(hItem, 243, 1);
			MULTIPAGE_SetTabWidth(hItem, 243, 2);
			MULTIPAGE_SetTabWidth(hItem, 0, 3);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SET_GENRAL);
			MULTIPAGE_EnablePage(hItem, 3);
			MULTIPAGE_SetTabWidth(hItem, 182, 0);
			MULTIPAGE_SetTabWidth(hItem, 182, 1);
			MULTIPAGE_SetTabWidth(hItem, 182, 2);
			MULTIPAGE_SetTabWidth(hItem, 182, 3);
		}
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbLanguageWindow(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[4];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LANG);
		TEXT_SetText(hItem, GUI_LANG_GetText(49));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_LANG);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, "EN (ENGLISH)");
		DROPDOWN_AddString(hItem, "DE (DEUTSCH)");
		DROPDOWN_AddString(hItem, "ES (ESPAÑOL)");
		DROPDOWN_AddString(hItem, "FR (FRANÇAIS)");
		DROPDOWN_AddString(hItem, "IT (ITALIANO)");
		DROPDOWN_AddString(hItem, "PL (POLSKI)");
		DROPDOWN_AddString(hItem, "PT (PORTUGUÊS)");
		DROPDOWN_SetSel(hItem, tempGeneralSettings.DeviceLanguage);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEVICE_DESCRIPTION);
		TEXT_SetText(hItem, GUI_LANG_GetText(50));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_DESCRIPTION);
		EDIT_SetText(hItem, tempGeneralSettings.DeviceDescription);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEVICE_ID);
		TEXT_SetText(hItem, GUI_LANG_GetText(51));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ID);
		mini_snprintf(GUITextBuffer, 4, "%02d", tempGeneralSettings.DeviceID);
		EDIT_SetText(hItem, GUITextBuffer);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_BEEPER);
		CHECKBOX_SetTextColor(hItem, 0x0000c000);
		CHECKBOX_SetState(hItem, tempGeneralSettings.BeeperMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM);
		CHECKBOX_SetTextColor(hItem, 0x0000c000);
		CHECKBOX_SetState(hItem, tempGeneralSettings.AlarmBeeperMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_DST);
		CHECKBOX_SetTextColor(hItem, 0x0000c000);
		CHECKBOX_SetState(hItem, tempGeneralSettings.DSTMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BEEPER);
		TEXT_SetText(hItem, GUI_LANG_GetText(130));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM);
		TEXT_SetText(hItem, GUI_LANG_GetText(131));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DST);
		TEXT_SetText(hItem, GUI_LANG_GetText(163));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;

		switch (Id)
		{
		case ID_EDIT_DEVICE_DESCRIPTION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 40);
				break;
			}
			break;
		case ID_DROPDOWN_LANG:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				tempGeneralSettings.DeviceLanguage = (uint8_t) DROPDOWN_GetSel(pMsg->hWinSrc);
				break;
			}
			break;
		case ID_EDIT_DEVICE_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 2);
				break;
			}
			break;
		case ID_CHECKBOX_BEEPER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_CHECKBOX_ALARM:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_CHECKBOX_DST:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(320, 53, 399, 39, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(320, 98, 110, 39, 4, 2, GUI_BLUE);
		break;
	case WM_DELETE:
		EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_DESCRIPTION), tempGeneralSettings.DeviceDescription, 81);
		CorrectNullString(tempGeneralSettings.DeviceDescription);

		EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ID), GUITextBuffer, 4);

		tempGeneralSettings.DeviceID = (short) atoi(GUITextBuffer);
		if (tempGeneralSettings.DeviceID < 0)
			tempGeneralSettings.DeviceID = 0;
		else if (tempGeneralSettings.DeviceID > 99)
			tempGeneralSettings.DeviceID = 99;

		tempGeneralSettings.BeeperMode = CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_BEEPER));
		tempGeneralSettings.AlarmBeeperMode = CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM));
		tempGeneralSettings.DSTMode = CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_DST));
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_LANG);
		DROPDOWN_SetTextSkin(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_DESCRIPTION);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ID);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbDisplayWindow(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BRIGHTNESS);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(53));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BRIGHTNESS_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SCREENSAVER_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(54));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_SCREENSAVER_MODE);
		RADIO_SetGroupId(hItem, 5);
		RADIO_SetText(hItem, " 0%", 0);
		RADIO_SetText(hItem, " 20%", 1);
		RADIO_SetFocusColor(hItem, GUI_WHITE);
		RADIO_SetValue(hItem, tempGeneralSettings.ScreenSaverBrightness);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SCREENSAVER_TIME);
		TEXT_SetText(hItem, GUI_LANG_GetText(55));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SCREENSAVER_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_BRIGHTNESS);
		SLIDER_SetRange(hItem, 21, 100);
		SLIDER_SetNumTicks(hItem, 10);
		SLIDER_SetValue(hItem, tempGeneralSettings.Brightness);
		SLIDER_SetWidth(hItem, 15);
		SLIDER_SetBarColor(hItem, GUI_BLUE);
		SLIDER_SetFocusColor(hItem, GUI_WHITE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BACKGROUND_COLOR);
		TEXT_SetText(hItem, GUI_LANG_GetText(261));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_BACKGROUND_COLOR);
		RADIO_SetText(hItem, GUI_LANG_GetText(263), 0);
		RADIO_SetText(hItem, GUI_LANG_GetText(262), 1);
		RADIO_SetFocusColor(hItem, GUI_TRANSPARENT);
		RADIO_SetValue(hItem, SKINS_GetStatus());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLIDER_SCREENSAVER_TIME);
		SLIDER_SetRange(hItem, 0, 60);
		SLIDER_SetNumTicks(hItem, 10);
		SLIDER_SetValue(hItem, tempGeneralSettings.ScreenSaverTime);
		SLIDER_SetWidth(hItem, 15);
		SLIDER_SetBarColor(hItem, GUI_BLUE);
		SLIDER_SetFocusColor(hItem, GUI_WHITE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BRIGHTNESS);
		EDIT_SetFocussable(hItem, 0);
		EDIT_SetInsertMode(hItem, 0);
		EDIT_SetDecMode(hItem, tempGeneralSettings.Brightness, 0, 100, 0, GUI_EDIT_NORMAL | GUI_EDIT_SUPPRESS_LEADING_ZEROES);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SCREENSAVER);
		EDIT_SetFocussable(hItem, 0);
		EDIT_SetInsertMode(hItem, 0);
		EDIT_SetDecMode(hItem, tempGeneralSettings.ScreenSaverTime, 0, 60, 0, GUI_EDIT_NORMAL | GUI_EDIT_SUPPRESS_LEADING_ZEROES);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_RADIO_SCREENSAVER_MODE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				tempGeneralSettings.ScreenSaverBrightness = RADIO_GetValue(pMsg->hWinSrc);
				break;
			}
			break;
		case ID_RADIO_BACKGROUND_COLOR:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (SKINS_GetStatus() != RADIO_GetValue(pMsg->hWinSrc))
				{
					if (1 == RADIO_GetValue(pMsg->hWinSrc))
						SKINS_SetStatus(0);
					else
						SKINS_SetStatus(1);

					UserMessage.MsgId = WM_USER_REFRESH_SKIN;
					UserMessage.Data.v = SKINS_GetStatus();
					WM_BroadcastMessage(&UserMessage);
				}
				break;
			}
			break;
		case ID_SLIDER_BRIGHTNESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				tempGeneralSettings.Brightness = SLIDER_GetValue(pMsg->hWinSrc);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BRIGHTNESS);
				EDIT_SetValue(hItem, tempGeneralSettings.Brightness);
				BACKLIGHT_Set(tempGeneralSettings.Brightness);
				break;
			}
			break;

		case ID_SLIDER_SCREENSAVER_TIME:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				tempGeneralSettings.ScreenSaverTime = SLIDER_GetValue(pMsg->hWinSrc);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SCREENSAVER);
				EDIT_SetValue(hItem, tempGeneralSettings.ScreenSaverTime);
				break;
			}
			break;
		}
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BRIGHTNESS);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_SCREENSAVER);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_SCREENSAVER_MODE);
		RADIO_SetTextColor(hItem, SKINS_GetTextColor());
		RADIO_SetFocusColor(hItem, SKINS_GetBkColor());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_BACKGROUND_COLOR);
		RADIO_SetTextColor(hItem, SKINS_GetTextColor());
		RADIO_SetFocusColor(hItem, SKINS_GetBkColor());

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbTimeAndDateWindow(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
	RTC_TimeTypeDef TempTime;
	RTC_DateTypeDef TempDate;
	char GUITextBuffer[10];

	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_TIME);
		BUTTON_SetText(hItem, GUI_LANG_GetText(57));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_DATE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(57));

		RTC_GetTimeAndDate(&TempTime, &TempDate);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_HOURS);
		LISTWHEEL_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_HCENTER);
		for (int i = 0; i < 24; ++i)
			LISTWHEEL_AddString(hItem, time[i]);
		LISTWHEEL_SetFont(hItem, &GUI_FontLato30);
		LISTWHEEL_SetSkinColors(hItem);
		LISTWHEEL_SetLineHeight(hItem, 35);
		LISTWHEEL_SetSnapPosition(hItem, 45);
		LISTWHEEL_SetSel(hItem, TempTime.Hours);
		LISTWHEEL_SetPos(hItem, TempTime.Hours);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_MINUTES);
		LISTWHEEL_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_HCENTER);
		for (int i = 0; i < 60; ++i)
			LISTWHEEL_AddString(hItem, time[i]);
		LISTWHEEL_SetFont(hItem, &GUI_FontLato30);
		LISTWHEEL_SetSkinColors(hItem);
		LISTWHEEL_SetLineHeight(hItem, 35);
		LISTWHEEL_SetSnapPosition(hItem, 45);
		LISTWHEEL_SetSel(hItem, TempTime.Minutes);
		LISTWHEEL_SetPos(hItem, TempTime.Minutes);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_SECONDS);
		LISTWHEEL_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_HCENTER);
		for (int i = 0; i < 60; ++i)
			LISTWHEEL_AddString(hItem, time[i]);
		LISTWHEEL_SetFont(hItem, &GUI_FontLato30);
		LISTWHEEL_SetSkinColors(hItem);
		LISTWHEEL_SetLineHeight(hItem, 35);
		LISTWHEEL_SetSnapPosition(hItem, 45);
		LISTWHEEL_SetSel(hItem, TempTime.Seconds);
		LISTWHEEL_SetPos(hItem, TempTime.Seconds);

		CALENDAR_Create(pMsg->hWin, 430, 8, (unsigned) TempDate.Year + 2000, TempDate.Month, TempDate.Date, 2, ID_CALENDAR, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_TIME_CORRECTION);
		BUTTON_SetText(hItem, GUI_LANG_GetText(57));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TIME_CORRECTION);
		TEXT_SetText(hItem, GUI_LANG_GetText(151));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TIME_CORRECTION_UNIT);
		TEXT_SetText(hItem, GUI_LANG_GetText(219));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TIME_CORRECTION);
		EDIT_SetInt(hItem, RTC_GetSecondsCorrection());
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTWHEEL_HOURS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_HOURS);
				LISTWHEEL_SetSel(hItem, LISTWHEEL_GetItemFromPos(hItem, TIME_WHEELS_SNAP_POSITION));
				break;
			case WM_NOTIFICATION_MOVED_OUT:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_HOURS);
				LISTWHEEL_SetSel(hItem, LISTWHEEL_GetItemFromPos(hItem, TIME_WHEELS_SNAP_POSITION));
				break;
			}
			break;
		case ID_LISTWHEEL_MINUTES:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_MINUTES);
				LISTWHEEL_SetSel(hItem, LISTWHEEL_GetItemFromPos(hItem, TIME_WHEELS_SNAP_POSITION));
				break;
			case WM_NOTIFICATION_MOVED_OUT:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_MINUTES);
				LISTWHEEL_SetSel(hItem, LISTWHEEL_GetItemFromPos(hItem, TIME_WHEELS_SNAP_POSITION));
				break;
			}
			break;
		case ID_LISTWHEEL_SECONDS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_SECONDS);
				LISTWHEEL_SetSel(hItem, LISTWHEEL_GetItemFromPos(hItem, TIME_WHEELS_SNAP_POSITION));
				break;
			case WM_NOTIFICATION_MOVED_OUT:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_SECONDS);
				LISTWHEEL_SetSel(hItem, LISTWHEEL_GetItemFromPos(hItem, TIME_WHEELS_SNAP_POSITION));
				break;
			}
			break;
		case ID_BUTTON_SET_TIME:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (USER_LEVEL < PASSWORDS_GetCurrentLevel())
				{
					TempTime.Hours = LISTWHEEL_GetSel(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_HOURS));
					TempTime.Minutes = LISTWHEEL_GetSel(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_MINUTES));
					TempTime.Seconds = LISTWHEEL_GetSel(WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_SECONDS));
					RTC_SetTime(&TempTime);
					ARCHIVE_SendEvent("SYS:TIME CHANGED");
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				}
				break;
			}
			break;
		case ID_BUTTON_SET_DATE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (USER_LEVEL < PASSWORDS_GetCurrentLevel())
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_CALENDAR);
					CALENDAR_GetSel(hItem, &data_set);
					TempDate.Date = data_set.Day;
					TempDate.Month = data_set.Month;
					TempDate.Year = data_set.Year - 2000;
					RTC_SetDate(&TempDate);
					ARCHIVE_SendEvent("SYS:DATE CHANGED");
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				}
				break;
			}
			break;
		case ID_EDIT_TIME_CORRECTION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (USER_LEVEL < PASSWORDS_GetCurrentLevel())
				{
					setNumKeyboard(pMsg->hWinSrc, EDIT, 5);
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				}
				break;
			}
			break;
		case ID_BUTTON_SET_TIME_CORRECTION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (USER_LEVEL < PASSWORDS_GetCurrentLevel())
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TIME_CORRECTION);
					EDIT_GetText(hItem, GUITextBuffer, 6);
					RTC_Calibration(atoi(GUITextBuffer));
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(117), GUI_YELLOW, GUI_BLACK);
				}
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(10, 175, 279, 80, 4, 2, GUI_BLUE);
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_HOURS);
		LISTWHEEL_SetSkinColors(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_MINUTES);
		LISTWHEEL_SetSkinColors(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_SECONDS);
		LISTWHEEL_SetSkinColors(hItem);

		CALENDAR_Refresh(pMsg->hWin);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TIME_CORRECTION);
		EDIT_SetSkin(hItem, EDIT_CI_ENABLED);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbServiceWindow(WM_MESSAGE *pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char textBuffer[200] =
	{ 0 };
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RESTORE_FACTORY);
		BUTTON_SetText(hItem, GUI_LANG_GetText(58));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HART_CONFIG);
		BUTTON_SetText(hItem, GUI_LANG_GetText(228));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_BOARD_FIRMWARE_UPDATE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(239));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_FIRMWARE_UPDATE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(270));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_TP_CALIBRATION);
		BUTTON_SetText(hItem, GUI_LANG_GetText(275));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_CALIBRATION);
		BUTTON_SetText(hItem, GUI_LANG_GetText(91));
		if (PASSWORDS_GetCurrentLevel() < SERVICE_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_SNMAC);
		BUTTON_SetText(hItem, GUI_LANG_GetText(198));
		if (PASSWORDS_GetCurrentLevel() < FACTORY_LEVEL)
			WM_HideWindow(hItem);



		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_RESTART:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateConfirmMessage(GUI_LANG_GetText(271), DEFAULT_COLOR, DEFAULT_COLOR, SERVICE_RestartDevice, NULL);
				break;
			}
			break;
		case ID_BUTTON_SET_CALIBRATION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateCalibarationMenu(hSetGeneral);
				break;
			}
			break;
		case ID_BUTTON_HART_CONFIG:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateHartConfigMenu(hSetGeneral);
				break;
			}
			break;
		case ID_BUTTON_BOARD_FIRMWARE_UPDATE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateFirmwareUpgradeMenu(hSetGeneral);
				break;
			}
			break;
		case ID_BUTTON_RESTORE_FACTORY:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				mini_snprintf(textBuffer, 200, "%s\n%s %s", GUI_LANG_GetText(272), GUI_LANG_GetText(170), GUI_LANG_GetText(176));
				CreateConfirmMessage(textBuffer, DEFAULT_COLOR, DEFAULT_COLOR, SERVICE_RestoreFactoryDefaults, NULL);
				break;
			}
			break;
		case ID_BUTTON_SET_FIRMWARE_UPDATE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateSetFirmwareUpdateWindow(hSetGeneral);
				break;
			}
			break;
		case ID_BUTTON_SET_TP_CALIBRATION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateCalibrateTouchPanel();
				break;
			}
			break;
		case ID_BUTTON_SET_SNMAC:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateserialnNumberWin(hSetGeneral);
				break;
			}
			break;
		}
		break;

	case WM_USER_LOGIN:
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_BOARD_FIRMWARE_UPDATE);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_FIRMWARE_UPDATE);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_TP_CALIBRATION);
			WM_HideWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_BOARD_FIRMWARE_UPDATE);
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_FIRMWARE_UPDATE);
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_TP_CALIBRATION);
			WM_ShowWindow(hItem);
		}

		if (PASSWORDS_GetCurrentLevel() < SERVICE_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_CALIBRATION);
			WM_HideWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_CALIBRATION);
			WM_ShowWindow(hItem);
		}

		if (PASSWORDS_GetCurrentLevel() < FACTORY_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_SNMAC);
			WM_HideWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SET_SNMAC);
			WM_ShowWindow(hItem);
		}

		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSetGeneral(WM_HWIN hParent)
{
	hSetGeneral = GUI_CreateDialogBox(_aGeneralMainWindow, GUI_COUNTOF(_aGeneralMainWindow), _cbMainWindow, hParent, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(10));
	return hSetGeneral;
}

static WM_HWIN CreateLanguageWindow(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aGeneralLanguageWindow, GUI_COUNTOF(_aGeneralLanguageWindow), _cbLanguageWindow, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateDisplayWindow(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aGeneralDisplayWindow, GUI_COUNTOF(_aGeneralDisplayWindow), _cbDisplayWindow, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateTimeAndDateWindow(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aGeneralTimeAndDateWindow, GUI_COUNTOF(_aGeneralTimeAndDateWindow), _cbTimeAndDateWindow, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateServiceWindow(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aServiceWindow, GUI_COUNTOF(_aServiceWindow), _cbServiceWindow, hParent, 0, 0);
	return hWin;
}

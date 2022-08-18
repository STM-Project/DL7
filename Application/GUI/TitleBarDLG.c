#include "TitleBarDLG.h"

#include "parameters.h"
#include "printScreen.h"
#include "graphics.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "WindowManagement.h"
#include "buzzer.h"
#include "archive.h"
#include "shift_reg.h"
#include "passwords.h"
#include "rtc.h"
#include "alarms.h"

#include "MenuBarDLG.h"
#include "SetConfirmDLG.h"
#include "fram.h"
#include "mini-printf.h"
#include "SetLoginDLG.h"

#define ID_TITLE_WINDOW     	(GUI_ID_USER + 0x02)
#define TITLE     						(GUI_ID_USER + 0x03)
#define ID_LOGO_IMAGE     		(GUI_ID_USER + 0x04)
#define ID_USER_IMAGE     		(GUI_ID_USER + 0x05)
#define ID_ARCHIVE_IMAGE    	(GUI_ID_USER + 0x06)
#define ID_ALARM_IMAGE     		(GUI_ID_USER + 0x07)
#define ID_TIME_TEXT     			(GUI_ID_USER + 0x08)
#define ID_DATE_TEXT     			(GUI_ID_USER + 0x09)
#define ID_PRINTSCREEN_BUTTON (GUI_ID_USER + 0x0A)

#define AUTO_CHANGE_OFF		0

// #define CPU_STAT

extern uint8_t FlagWindowChangeTimer;
extern GUI_CONST_STORAGE GUI_BITMAP bmERR;

static BYTE bufferForLogoSmallGraphic[20176] __attribute__ ((section(".sdram")));
UINT bytesreadFromLogoSmall;
static BYTE bufferForAlarmOnGraphic[11616] __attribute__ ((section(".sdram")));
UINT bytesreadFromAlarmOnGraphic;
static BYTE bufferForAlarmOffGraphic[11616] __attribute__ ((section(".sdram")));
UINT bytesreadFromAlarmOffGraphic;
BYTE bufferForRecordGraphic[11616] __attribute__ ((section(".sdram")));
UINT bytesreadFromRecordGraphic;
static BYTE bufferForNoUserGraphic[10016] __attribute__ ((section(".sdram")));
UINT bytesreadFromNoUserGraphic;
BYTE bufferForUserGraphic[10016] __attribute__ ((section(".sdram")));
UINT bytesreadFromUserGraphic;
static BYTE bufferForAdminGraphic[10016] __attribute__ ((section(".sdram")));
UINT bytesreadFromAdminGraphic;
static BYTE bufferForFactoryGraphic[10016] __attribute__ ((section(".sdram")));
UINT bytesreadFromFactoryGraphic;
static BYTE bufferForServiceGraphic[10016] __attribute__ ((section(".sdram")));
UINT bytesreadFromServiceGraphic;



WM_HWIN TitleBar;

#ifdef CPU_STAT
static uint8_t CPU_ON = 0;
#endif

static const GUI_WIDGET_CREATE_INFO _aTitleBarDialogCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_TITLE_WINDOW, 0, 0, 800, 60, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", TITLE, 200, 0, 400, 58, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TIME_TEXT, 700, 29, 95, 29, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_DATE_TEXT, 700, 0, 95, 29, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ IMAGE_CreateIndirect, "", ID_LOGO_IMAGE, 0, 0, 100, 58, 0, 0, 0 },
{ TEXT_CreateIndirect, "", 	ID_PRINTSCREEN_BUTTON, 5, 5, 90, 48, WM_CF_HASTRANS, 0 },
{ IMAGE_CreateIndirect, "", ID_USER_IMAGE, 100, 0, 50, 58, 0, 0, 0 },
{ IMAGE_CreateIndirect, "", ID_ARCHIVE_IMAGE, 650, 0, 50, 58, 0, 0, 0 },
{ IMAGE_CreateIndirect, "", ID_ALARM_IMAGE, 600, 0, 50, 58, 0, 0, 0 }, };

static void _cbTitleBarDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	static uint8_t PulseAlarm = 0;
	static uint8_t UserImageStatus = 0;
	static uint8_t BlinkAlarm = 0;
	static RTC_TimeTypeDef Time;
	static RTC_DateTypeDef Date;
	int Id;
	int NCode;

	char GUITextBuffer[10];

	switch (pMsg->MsgId)
	{

	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LOGO_IMAGE);
		if (bytesreadFromLogoSmall)
			IMAGE_SetDTA(hItem, &bufferForLogoSmallGraphic, bytesreadFromLogoSmall);
		else
			IMAGE_SetBitmap(hItem, &bmERR);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_ALARM_IMAGE);
		if (bytesreadFromAlarmOffGraphic)
			IMAGE_SetDTA(hItem, &bufferForAlarmOffGraphic, bytesreadFromAlarmOffGraphic);
		else
			IMAGE_SetBitmap(hItem, &bmERR);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_IMAGE);
		if (bytesreadFromRecordGraphic)
			IMAGE_SetDTA(hItem, &bufferForRecordGraphic, bytesreadFromRecordGraphic);
		else
			IMAGE_SetBitmap(hItem, &bmERR);

		if (ARCHIVE_GetState() == 1)
			WM_ShowWindow(hItem);
		else
			WM_HideWindow(hItem);

		UserImageStatus = PASSWORDS_GetCurrentLevel();

		UserMessage.MsgId = WM_USER_LOGIN;
		UserMessage.hWin = 0;
		UserMessage.hWinSrc = pMsg->hWin;
		UserMessage.Data.v = UserImageStatus;
		WM_BroadcastMessage(&UserMessage);
		break;

	case WM_TIMER:
		if (PulseAlarm)
			PulseAlarm = 0;
		else
			PulseAlarm = 1;

		RTC_GetTimeAndDate(&Time, &Date);
#ifdef CPU_STAT
		if (CPU_ON==0)
		mini_snprintf(GUITextBuffer, 6, "%d", WM_GetNumWindows());
		else
		mini_snprintf(GUITextBuffer, 6, "%02d:%02d", Time.Hours, Time.Minutes);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TIME_TEXT);
		TEXT_SetText(hItem, GUITextBuffer);

		if (CPU_ON==0)
		mini_snprintf(GUITextBuffer, 5, "%d%%", osGetCPUUsage());
		else
		mini_snprintf(GUITextBuffer, 10, "%02d-%02d-%02d", Date.Date, Date.Month, Date.Year);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DATE_TEXT);
		TEXT_SetText(hItem, GUITextBuffer);
#else
		mini_snprintf(GUITextBuffer, 6, "%02d:%02d", Time.Hours, Time.Minutes);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TIME_TEXT);
		TEXT_SetText(hItem, GUITextBuffer);

		mini_snprintf(GUITextBuffer, 10, "%02d-%02d-%02d", Date.Date, Date.Month, Date.Year);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DATE_TEXT);
		TEXT_SetText(hItem, GUITextBuffer);
#endif

		hItem = WM_GetDialogItem(pMsg->hWin, ID_USER_IMAGE);

		if (UserImageStatus != PASSWORDS_GetCurrentLevel())
		{
			UserMessage.MsgId = WM_USER_LOGIN;
			UserMessage.hWin = 0;
			UserMessage.hWinSrc = pMsg->hWin;
			UserMessage.Data.v = PASSWORDS_GetCurrentLevel();
			WM_BroadcastMessage(&UserMessage);
		}

		BlinkAlarm = ALARMS_GetState();

		if (BlinkAlarm != 0xFF)
		{
			UserMessage.MsgId = WM_USER_ALARM;
			UserMessage.hWin = 0;
			UserMessage.hWinSrc = pMsg->hWin;
			UserMessage.Data.v = BlinkAlarm;
			WM_BroadcastMessage(&UserMessage);
		}

		WM_RestartTimer(pMsg->Data.v, 1000);
		break;

	case WM_USER_ALARM:
		switch (pMsg->Data.v)
		{
		case 0:
			hItem = WM_GetDialogItem(pMsg->hWin, ID_ALARM_IMAGE);
			IMAGE_SetDTA(hItem, &bufferForAlarmOffGraphic, bytesreadFromAlarmOffGraphic);
			Buzzer_Off();
			break;
		case 1:
			hItem = WM_GetDialogItem(pMsg->hWin, ID_ALARM_IMAGE);
			IMAGE_SetDTA(hItem, &bufferForAlarmOnGraphic, bytesreadFromAlarmOnGraphic);
			if (GeneralSettings.AlarmBeeperMode)
				Buzzer_Off();
			break;
		case 2:
			hItem = WM_GetDialogItem(pMsg->hWin, ID_ALARM_IMAGE);
			if (PulseAlarm)
			{
				IMAGE_SetDTA(hItem, &bufferForAlarmOnGraphic, bytesreadFromAlarmOnGraphic);
				if (GeneralSettings.AlarmBeeperMode)
					Buzzer_On();
			}
			else
			{
				IMAGE_SetDTA(hItem, &bufferForAlarmOffGraphic, bytesreadFromAlarmOffGraphic);
				if (GeneralSettings.AlarmBeeperMode)
					Buzzer_Off();
			}

			break;
		default:
			Buzzer_Off();
		}
		break;

	case WM_USER_LOGIN:
		NCode = pMsg->Data.v;
		hItem = WM_GetDialogItem(pMsg->hWin, ID_USER_IMAGE);
		switch (NCode)
		{
		case NO_USER:
			if (GetKeyboardHandle())
				deleteKeyboard();

			if (bytesreadFromUserGraphic)
				IMAGE_SetDTA(hItem, &bufferForNoUserGraphic, bytesreadFromNoUserGraphic);
			else
				IMAGE_SetBitmap(hItem, &bmERR);

			break;

		case USER_LEVEL:
			if (PASSWORDS_IsUserPasswordDisable() && GetKeyboardHandle())
				deleteKeyboard();

			if (bytesreadFromUserGraphic)
				IMAGE_SetDTA(hItem, &bufferForUserGraphic, bytesreadFromUserGraphic);
			else
				IMAGE_SetBitmap(hItem, &bmERR);
			WM_ShowWindow(hItem);
			break;

		case ADMIN_LEVEL:
			if (bytesreadFromAdminGraphic)
				IMAGE_SetDTA(hItem, &bufferForAdminGraphic, bytesreadFromAdminGraphic);
			else
				IMAGE_SetBitmap(hItem, &bmERR);
			WM_ShowWindow(hItem);
			break;

		case SERVICE_LEVEL:
			if (bytesreadFromServiceGraphic)
				IMAGE_SetDTA(hItem, &bufferForServiceGraphic, bytesreadFromServiceGraphic);
			else
				IMAGE_SetBitmap(hItem, &bmERR);
			WM_ShowWindow(hItem);
			break;

		case FACTORY_LEVEL:
			if (bytesreadFromFactoryGraphic)
				IMAGE_SetDTA(hItem, &bufferForFactoryGraphic, bytesreadFromFactoryGraphic);
			else
				IMAGE_SetBitmap(hItem, &bmERR);
			WM_ShowWindow(hItem);
			break;
		}
		UserImageStatus = PASSWORDS_GetCurrentLevel();
		break;
	case WM_USER_ARCHIVE:
		NCode = pMsg->Data.v;
		hItem = WM_GetDialogItem(pMsg->hWin, ID_ARCHIVE_IMAGE);
		switch (NCode)
		{
		case 0:
			WM_HideWindow(hItem);
			break;
		case 1:
			if (bytesreadFromRecordGraphic)
				IMAGE_SetDTA(hItem, &bufferForRecordGraphic, bytesreadFromRecordGraphic);
			else
				IMAGE_SetBitmap(hItem, &bmERR);
			WM_ShowWindow(hItem);
			break;
		}
		break;

	case WM_USER_WIN_NAME:
		hItem = WM_GetDialogItem(pMsg->hWin, TITLE);
		TEXT_SetText(hItem, pMsg->Data.p);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_PRINTSCREEN_BUTTON:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
				{
					BUZZER_Beep();
					BLUE_LED_ON;
					PrintScreen();
					CreateMessage(GUI_LANG_GetText(172), DEFAULT_COLOR, DEFAULT_COLOR);
					GUI_Delay(5);
					BLUE_LED_OFF;
				}
				break;
			}
			break;
		case ID_USER_IMAGE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				CreateSetLogin(hActualWindow);
				break;
			}
			break;
		case ID_ALARM_IMAGE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				FlagWindowChangeTimer = AUTO_CHANGE_OFF;
				FRAM_Write(AUTO_CHANGE_ADDR, FlagWindowChangeTimer);

				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				if (WindowsState == SETTINGS && NewSettings)
					CreateSetConfirm(WM_HBKWIN, (uint8_t) ALARMS);
				else
					hActualWindow = ChangeWindow(hActualWindow, ALARMS);
				break;
			}
			break;
#ifdef CPU_STAT
			case ID_TIME_TEXT:
			switch (NCode)
			{
				case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (CPU_ON)
				{
					CPU_ON = 0;
					setDarkSkin();
					WM_DeleteWindow(TitleBar);
					CreateTitleBar();
					WM_DeleteWindow(MenuBar);
					CreateMenuBar();
				}
				else
				{
					CPU_ON = 1;
					setLightSkin();
					WM_DeleteWindow(TitleBar);
					CreateTitleBar();
					WM_DeleteWindow(MenuBar);
					CreateMenuBar();
				}
				break;
			}
			break;
#endif
		}
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());

		hItem = WM_GetDialogItem(pMsg->hWin, TITLE);
		TEXT_SetTextColor(hItem, SKINS_GetTextColor());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DATE_TEXT);
		TEXT_SetTextColor(hItem, SKINS_GetTextColor());

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TIME_TEXT);
		TEXT_SetTextColor(hItem, SKINS_GetTextColor());
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateTitleBar(void)
{
	TitleBar = GUI_CreateDialogBox(_aTitleBarDialogCreate, GUI_COUNTOF(_aTitleBarDialogCreate), _cbTitleBarDialog, WM_HBKWIN, 0, 0);
	WM_CreateTimer(TitleBar, 0, 50, 0);
	return TitleBar;
}

void loadTitleBarWindowGraphicsToRAM()
{
	bytesreadFromLogoSmall = LoadGraphicFileToRAM("0:image/slogo.dta", bufferForLogoSmallGraphic);

	bytesreadFromNoUserGraphic = LoadGraphicFileToRAM("0:image/nouser.dta", bufferForNoUserGraphic);
	bytesreadFromUserGraphic = LoadGraphicFileToRAM("0:image/user.dta", bufferForUserGraphic);
	bytesreadFromAdminGraphic = LoadGraphicFileToRAM("0:image/admin.dta", bufferForAdminGraphic);
	bytesreadFromServiceGraphic = LoadGraphicFileToRAM("0:image/service.dta", bufferForServiceGraphic);
	bytesreadFromFactoryGraphic = LoadGraphicFileToRAM("0:image/factory.dta", bufferForFactoryGraphic);
	bytesreadFromRecordGraphic = LoadGraphicFileToRAM("0:image/rec.dta", bufferForRecordGraphic);
	bytesreadFromAlarmOnGraphic = LoadGraphicFileToRAM("0:image/alarmon.dta", bufferForAlarmOnGraphic);
	bytesreadFromAlarmOffGraphic = LoadGraphicFileToRAM("0:image/alarmoff.dta", bufferForAlarmOffGraphic);
}

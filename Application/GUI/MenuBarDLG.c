#include "MenuBarDLG.h"
#include "graphics.h"

#include "WindowManagement.h"
#include "buzzer.h"

#include "SetConfirmDLG.h"
#include "fram.h"
#include "parameters.h"
#include "skins.h"
#include "UserMessage.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

#define ID_WINDOW_MENUBAR     	(GUI_ID_USER + 0x10)
#define ID_ICONVIEW_MENUBAR     (GUI_ID_USER + 0x11)

#define AUTO_CHANGE_OFF		0
#define AUTO_CHANGE_INIT	1
#define AUTO_CHANGE_DESC	2
#define AUTO_CHANGE_ASC		3

extern GUI_CONST_STORAGE GUI_BITMAP bmERR;
extern uint8_t NewSettings;

WM_HWIN MenuBar;


SDRAM BYTE bufferForUpDTAGraphic[13472];
SDRAM UINT bytesreadFromUpDTA;
SDRAM BYTE bufferForInfoDTAGraphic[13472];
SDRAM UINT bytesreadFromInfoDTA;
SDRAM BYTE bufferForTableDTAGraphic[13472];
SDRAM UINT bytesreadFromTableDTA;
SDRAM BYTE bufferForGraphDTAGraphic[13472];
SDRAM UINT bytesreadFromGraphDTA;
SDRAM BYTE bufferForArchiveDTAGraphic[13472];
SDRAM UINT bytesreadFromArchiveDTA;
SDRAM BYTE bufferForSettingsDTAGraphic[13472];
SDRAM UINT bytesreadFromSettingsDTA;
SDRAM BYTE bufferForDownDTAGraphic[13472];
SDRAM UINT bytesreadFromDownDTA;

WM_HMEM WindowChangeTimer = 0;
uint8_t FlagWindowChangeTimer;
int selection;

void loadMenuBarWindowGrapihicsToRAM()
{
	bytesreadFromUpDTA = LoadGraphicFileToRAM("0:image/up.dta" , bufferForUpDTAGraphic);
	bytesreadFromInfoDTA = LoadGraphicFileToRAM("0:image/info.dta" , bufferForInfoDTAGraphic);
	bytesreadFromTableDTA = LoadGraphicFileToRAM("0:image/table.dta" ,bufferForTableDTAGraphic );
	bytesreadFromGraphDTA = LoadGraphicFileToRAM("0:image/graph.dta" , bufferForGraphDTAGraphic);
	bytesreadFromArchiveDTA = LoadGraphicFileToRAM("0:image/arch.dta" , bufferForArchiveDTAGraphic);
	bytesreadFromSettingsDTA = LoadGraphicFileToRAM("0:image/set.dta" , bufferForSettingsDTAGraphic);
	bytesreadFromDownDTA = LoadGraphicFileToRAM("0:image/down.dta" ,bufferForDownDTAGraphic);
}

static const GUI_WIDGET_CREATE_INFO _aMenuBarDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "Menu_bar", ID_WINDOW_MENUBAR, 740, 60, 60, 420, 0, 0, 0 },
		};

static void _cbMenuBarDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		ICONVIEW_CreateEx(0, 0, 60, 420,  pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_ICONVIEW_MENUBAR, 58, 58);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_MENUBAR);
		ICONVIEW_EnableStreamAuto();
		ICONVIEW_SetSkinColors(hItem);

		if (bytesreadFromUpDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForUpDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromInfoDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForInfoDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromTableDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForTableDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromGraphDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForGraphDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromArchiveDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForArchiveDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromSettingsDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForSettingsDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		if (bytesreadFromDownDTA)
			ICONVIEW_AddStreamedBitmapItem(hItem, &bufferForDownDTAGraphic, "");
		else
			ICONVIEW_AddBitmapItem(hItem, &bmERR, "");

		ICONVIEW_SetFrame(hItem, GUI_COORD_X, 0);
		ICONVIEW_SetFrame(hItem, GUI_COORD_Y, 0);
		ICONVIEW_SetSpace(hItem, GUI_COORD_Y, 2);
		ICONVIEW_SetSpace(hItem, GUI_COORD_X, 0);

		hActualWindow = ChangeWindow(hActualWindow, WindowsState);

		FlagWindowChangeTimer = FRAM_Read(AUTO_CHANGE_ADDR);
		if(FlagWindowChangeTimer>4 || FlagWindowChangeTimer<0)
			FlagWindowChangeTimer = AUTO_CHANGE_OFF;

		if (FlagWindowChangeTimer == AUTO_CHANGE_ASC)
			WindowChangeTimer = WM_CreateTimer(pMsg->hWin, 102, 3000, 0);
		else if (FlagWindowChangeTimer == AUTO_CHANGE_DESC)
			WindowChangeTimer = WM_CreateTimer(pMsg->hWin, 101, 3000, 0);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;

		switch (Id)
		{
		case ID_ICONVIEW_MENUBAR:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (FlagWindowChangeTimer == AUTO_CHANGE_INIT)
				{
					FlagWindowChangeTimer = AUTO_CHANGE_OFF;
					FRAM_Write(AUTO_CHANGE_ADDR, FlagWindowChangeTimer);

					if (WindowChangeTimer)
					{
						WM_DeleteTimer(WindowChangeTimer);
						WindowChangeTimer = 0;
					}
				}
				break;
			case WM_NOTIFICATION_CLICKED:

				FlagWindowChangeTimer = AUTO_CHANGE_INIT;
				FRAM_Write(AUTO_CHANGE_ADDR, FlagWindowChangeTimer);
				if (WindowChangeTimer)
				{
					WM_DeleteTimer(WindowChangeTimer);
					WindowChangeTimer = 0;
				}
				selection = ICONVIEW_GetSel(pMsg->hWinSrc);

				WM_SetCapture(pMsg->hWinSrc, 1);


				if (NewSettings)
				{
					if (WindowsState == SETTINGS)
					{
						if (selection != 5)
							CreateSetConfirm(WM_HBKWIN, selection);
						break;
					}
				}
				switch (selection)
				{
				case 0:
					if (WindowsState != SINGLE_VALUE)
					{
						WindowInstance = 99;
						while (CheckChannelType(WindowInstance) == 0)
						{
							WindowInstance--;
							if (WindowInstance < 0)
								break;
						}
						if (WindowInstance >= 0)
						{
							WindowChangeTimer = WM_CreateTimer(pMsg->hWin, 101, 2000, 0);
							BUZZER_Beep();
							hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
						}
					}
					else if (WindowsState == SINGLE_VALUE)
					{
						WindowInstance--;
						if (WindowInstance < 0)
							WindowInstance = 99;

						while (CheckChannelType(WindowInstance) == 0)
						{
							WindowInstance--;
							if (WindowInstance < 0)
								WindowInstance = 99;
						}

						WindowChangeTimer = WM_CreateTimer(pMsg->hWin, 101, 2000, 0);
						BUZZER_Beep();
						hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
					}
					break;
				case 1:/* Selected Info Window */
					BUZZER_Beep();
					hActualWindow = ChangeWindow(hActualWindow, INFO);
					break;
				case 2: /* Selected User Values Window */
					BUZZER_Beep();
					hActualWindow = ChangeWindow(hActualWindow, USERVALUES);
					break;
				case 3:/* Selected Trends Window */
					BUZZER_Beep();
					hActualWindow = ChangeWindow(hActualWindow, USERTRENDS);
					break;
				case 4: /* Selected Archive Window */
					BUZZER_Beep();
					hActualWindow = ChangeWindow(hActualWindow, ARCHIVE);
					break;
				case 5:/* Selected Settings Window */
					BUZZER_Beep();
					hActualWindow = ChangeWindow(hActualWindow, SETTINGS);
					break;
				case 6:

					if (WindowsState != SINGLE_VALUE)
					{
						WindowInstance = 0;
						while (CheckChannelType(WindowInstance) == 0)
						{
							WindowInstance++;

							if (WindowInstance > 99)
								break;

						}
						if (WindowInstance <= 99)
						{
							WindowChangeTimer = WM_CreateTimer(pMsg->hWin, 102, 2000, 0);
							BUZZER_Beep();
							hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
						}

					}
					else if (WindowsState == SINGLE_VALUE)
					{

						WindowInstance++;
						if (WindowInstance > 99)
							WindowInstance = 0;

						while (CheckChannelType(WindowInstance) == 0)
						{
							WindowInstance++;
							if (WindowInstance > 99)
								WindowInstance = 0;
						}

						WindowChangeTimer = WM_CreateTimer(pMsg->hWin, 102, 2000, 0);
						BUZZER_Beep();
						hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
					}
					break;
				}
				break;
			}
			break;
		}
		break;

	case WM_TIMER:
		Id = WM_GetTimerId(pMsg->Data.v);
		switch (Id)
		{
		case 101:
			if (FlagWindowChangeTimer == AUTO_CHANGE_INIT)
			{
				WM_DeleteTimer(pMsg->hWinSrc);
				WindowChangeTimer = WM_CreateTimer(pMsg->hWin, Id, 3000, 0);

				FlagWindowChangeTimer = AUTO_CHANGE_DESC;
				FRAM_Write(AUTO_CHANGE_ADDR, FlagWindowChangeTimer);

				BUZZER_Beep();
			}
			else if (FlagWindowChangeTimer == AUTO_CHANGE_DESC)
			{
				WindowInstance--;
				if (WindowInstance < 0)
					WindowInstance = 99;
				while (CheckChannelType(WindowInstance) == 0)
				{
					WindowInstance--;
					if (WindowInstance < 0)
						WindowInstance = 99;
				}

				hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
				WM_RestartTimer(pMsg->Data.v, 3000);
			}
			else
				WM_DeleteTimer(pMsg->Data.v);
			break;

		case 102:
			if (FlagWindowChangeTimer == AUTO_CHANGE_INIT)
			{
				WM_DeleteTimer(pMsg->hWinSrc);
				WindowChangeTimer = WM_CreateTimer(pMsg->hWin, Id, 3000, 0);

				FlagWindowChangeTimer = AUTO_CHANGE_ASC;
				FRAM_Write(AUTO_CHANGE_ADDR, FlagWindowChangeTimer);

				BUZZER_Beep();
			}
			else if (FlagWindowChangeTimer == AUTO_CHANGE_ASC)
			{
				WindowInstance++;
				if (WindowInstance > 99)
					WindowInstance = 0;
				while (CheckChannelType(WindowInstance) == 0)
				{
					WindowInstance++;
					if (WindowInstance > 99)
						WindowInstance = 0;
				}
				hActualWindow = ChangeWindow(hActualWindow, SINGLE_VALUE);
				WM_RestartTimer(pMsg->Data.v, 3000);
			}
			else
			{
				WM_DeleteTimer(pMsg->Data.v);
				WindowChangeTimer = 0;
			}
			break;
		}
		break;
	case WM_USER_REFRESH_SKIN:
		WINDOW_SetBkColor(pMsg->hWin, SKINS_GetBkColor());
		hItem = WM_GetDialogItem(pMsg->hWin, ID_ICONVIEW_MENUBAR);
		ICONVIEW_SetSkinColors(hItem);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateMenuBar(void)
{
	MenuBar = GUI_CreateDialogBox(_aMenuBarDialogCreate, GUI_COUNTOF(_aMenuBarDialogCreate), _cbMenuBarDialog, WM_HBKWIN,
			0, 0);
	return MenuBar;
}

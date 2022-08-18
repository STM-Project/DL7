#include "SetLoginDLG.h"
#include <string.h>
#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_LOGIN 								(GUI_ID_USER + 0x200)
#define ID_DROPDOWN_USER_LOGIN 					(GUI_ID_USER + 0x201)
#define ID_BUTTON_LOGIN 								(GUI_ID_USER + 0x202)
#define ID_MULTIEDIT_PASSWORD 					(GUI_ID_USER + 0x203)
#define ID_MULTIEDIT_NEW_PASSWORD 			(GUI_ID_USER + 0x204)
#define ID_DROPDOWN_USER_NEW_PASSWORD 	(GUI_ID_USER + 0x205)
#define ID_MULTIEDIT_CONFIRM_PASSWORD 	(GUI_ID_USER + 0x206)
#define ID_BUTTON_CANCEL 								(GUI_ID_USER + 0x207)
#define ID_BUTTON_CHANGE_PASSWORD 			(GUI_ID_USER + 0x208)
#define ID_TEXT_USER_LOGIN 							(GUI_ID_USER + 0x209)
#define ID_TEXT_PASSWORD 								(GUI_ID_USER + 0x20A)
#define ID_TEXT_USER_NEW_PASSWORD 			(GUI_ID_USER + 0x20B)
#define ID_TEXT_NEW_PASSWORD 						(GUI_ID_USER + 0x20C)
#define ID_TEXT_CONFIRM_PASSWORD 				(GUI_ID_USER + 0x20D)
#define ID_TEXT_CHANGE_PASSWORD 				(GUI_ID_USER + 0x20E)
#define ID_BUTTON_LOGOUT 								(GUI_ID_USER + 0x20F)

static WM_HWIN hWinLogin = 0;

int SelectedUser __attribute__ ((section(".sdram")));

char NewPassword1[20] =
		{ 0 };
char NewPassword2[20] =
		{ 0 };

static const GUI_WIDGET_CREATE_INFO _aSetLoginDialogCreate[] =
{
		{ WINDOW_CreateIndirect, "", ID_WINDOW_LOGIN, 0, 0, 740, 420, 0, 0x0, 0 },
		{ TEXT_CreateIndirect, "", ID_TEXT_USER_LOGIN, 10, 5, 285, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
		{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_USER_LOGIN, 300, 5, 200, 130, 0, 0x0, 0 },
		{ TEXT_CreateIndirect, "", ID_TEXT_PASSWORD, 10, 50, 285, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
		{ MULTIEDIT_CreateIndirect, "", ID_MULTIEDIT_PASSWORD, 305, 53, 190, 30, 0, 0x64, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_LOGIN, 300, 95, 200, 40, 0, 0x0, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_LOGOUT, 300, 141, 200, 40, 0, 0x0, 0 },

		{ TEXT_CreateIndirect, "", ID_TEXT_CHANGE_PASSWORD, 250, 200, 300, 30,  GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
		{ TEXT_CreateIndirect, "", ID_TEXT_USER_NEW_PASSWORD, 10, 235, 285, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
		{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_USER_NEW_PASSWORD, 300, 235, 200, 100, 0, 0x0, 0 },
		{ TEXT_CreateIndirect, "", ID_TEXT_NEW_PASSWORD, 10, 280, 285, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
		{ MULTIEDIT_CreateIndirect, "", ID_MULTIEDIT_NEW_PASSWORD, 305, 283, 185, 30, 0, 0x64, 0 },
		{ TEXT_CreateIndirect, "", ID_TEXT_CONFIRM_PASSWORD, 10, 325, 285, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
		{ MULTIEDIT_CreateIndirect, "", ID_MULTIEDIT_CONFIRM_PASSWORD, 305, 328, 185, 30, 0, 0x64, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_CHANGE_PASSWORD, 300, 370, 200, 40, 0, 0x0, 0 },
		{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 610, 370, 120, 40, 0, 0x0, 0 },
};

static void _cbSetLoginDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_LOGIN);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(78));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(79));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(80));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(81));
		DROPDOWN_SetTextSkin(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_PASSWORD);
		MULTIEDIT_SetPasswordMode(hItem, 1);
		MULTIEDIT_SetText(hItem, "");
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		MULTIEDIT_SetSkin(hItem, MULTIEDIT_CI_EDIT);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_NEW_PASSWORD);
		MULTIEDIT_SetPasswordMode(hItem, 1);
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		MULTIEDIT_SetText(hItem, "AABB");
		MULTIEDIT_SetSkin(hItem, MULTIEDIT_CI_EDIT);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_NEW_PASSWORD);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(78));
		DROPDOWN_SetTextSkin(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_CONFIRM_PASSWORD);
		MULTIEDIT_SetPasswordMode(hItem, 1);
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		MULTIEDIT_SetText(hItem, "BBAA");
		MULTIEDIT_SetSkin(hItem, MULTIEDIT_CI_EDIT);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_USER_LOGIN);
		TEXT_SetText(hItem, GUI_LANG_GetText(16));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PASSWORD);
		TEXT_SetText(hItem, GUI_LANG_GetText(17));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_USER_NEW_PASSWORD);
		TEXT_SetText(hItem, GUI_LANG_GetText(16));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_PASSWORD);
		TEXT_SetText(hItem, GUI_LANG_GetText(20));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONFIRM_PASSWORD);
		TEXT_SetText(hItem, GUI_LANG_GetText(21));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANGE_PASSWORD);
		TEXT_SetText(hItem, GUI_LANG_GetText(19));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LOGIN);
		BUTTON_SetText(hItem, GUI_LANG_GetText(18));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_PASSWORD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(22));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 8);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LOGOUT);
		BUTTON_SetText(hItem, GUI_LANG_GetText(164));

		UserMessage.MsgId = WM_USER_LOGIN;
		UserMessage.hWin = pMsg->hWin;
		UserMessage.hWinSrc = pMsg->hWin;
		UserMessage.Data.v = PASSWORDS_GetCurrentLevel();
		WM_SendMessage(pMsg->hWin, &UserMessage);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_USER_LOGIN:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_LOGIN:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_PASSWORD);
				MULTIEDIT_GetText(hItem, NewPassword1, 20);
				SelectedUser = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_LOGIN)) + 1;

				if (PASSWORDS_Login(SelectedUser, NewPassword1))
				{
					UserMessage.MsgId = WM_USER_LOGIN;
					UserMessage.hWin = 0;
					UserMessage.hWinSrc = pMsg->hWin;
					UserMessage.Data.v = PASSWORDS_GetCurrentLevel();
					WM_BroadcastMessage(&UserMessage);
				}
				else
				{
					CreateMessage(GUI_LANG_GetText(115), GUI_YELLOW, GUI_BLACK);
				}
				break;

			}
			break;
		case ID_MULTIEDIT_PASSWORD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				MULTIEDIT_SetText(pMsg->hWinSrc, "");
				setKeyboard(pMsg->hWinSrc, MULTIEDIT, PASSON, 16);
				break;
			}
			break;
		case ID_MULTIEDIT_NEW_PASSWORD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				MULTIEDIT_SetText(pMsg->hWinSrc, "");
				setKeyboard(pMsg->hWinSrc, MULTIEDIT, PASSON, 16);
				break;
			}
			break;
		case ID_DROPDOWN_USER_NEW_PASSWORD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_MULTIEDIT_CONFIRM_PASSWORD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				MULTIEDIT_SetText(pMsg->hWinSrc, "");
				setKeyboard(pMsg->hWinSrc, MULTIEDIT, PASSON, 16);
				break;
			case WM_NOTIFICATION_VALUE_CHANGED:
				break;
			}
			break;
		case ID_BUTTON_CHANGE_PASSWORD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_NEW_PASSWORD);
				MULTIEDIT_GetText(hItem, NewPassword1, 20);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_CONFIRM_PASSWORD);
				MULTIEDIT_GetText(hItem, NewPassword2, 20);

				if ((strcmp(NewPassword2, NewPassword1) == 0))
				{
					SelectedUser = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_NEW_PASSWORD));
					switch (SelectedUser)
					{
					case 0:
						if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
						{
							PASSWORDS_Change(0, NewPassword1);
							CreateMessage(GUI_LANG_GetText(118), DEFAULT_COLOR, DEFAULT_COLOR);
						}

						break;
					case 1:
						if (PASSWORDS_GetCurrentLevel() >= ADMIN_LEVEL)
						{
							PASSWORDS_Change(1, NewPassword1);
							CreateMessage(GUI_LANG_GetText(118), DEFAULT_COLOR, DEFAULT_COLOR);
						}
						break;
					case 2:
						if (PASSWORDS_GetCurrentLevel() >= SERVICE_LEVEL)
						{
							PASSWORDS_Change(2, NewPassword1);
							CreateMessage(GUI_LANG_GetText(118), DEFAULT_COLOR, DEFAULT_COLOR);
						}
						break;
					}
				}
				else
					CreateMessage(GUI_LANG_GetText(116), GUI_YELLOW, GUI_BLACK);
				break;
			}
			break;
		case ID_BUTTON_CANCEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_LOGOUT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				PASSWORDS_Logout();
				UserMessage.MsgId = WM_USER_LOGIN;
				UserMessage.hWin = 0;
				UserMessage.hWinSrc = pMsg->hWin;
				UserMessage.Data.v = PASSWORDS_GetCurrentLevel();
				WM_BroadcastMessage(&UserMessage);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(2, 0, 735, 417, 4, 2, GUI_GRAY);
		DRAW_RoundedFrame(300, 48, 199, 39, 4, 2, GUI_BLUE);
		if (PASSWORDS_GetCurrentLevel() >= USER_LEVEL)
		{
			DRAW_RoundedFrame(300, 278, 199, 39, 4, 2, GUI_BLUE);
			DRAW_RoundedFrame(300, 323, 199, 39, 4, 2, GUI_BLUE);
		}
		break;

	case WM_USER_LOGIN:
		NCode = pMsg->Data.v;
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_NEW_PASSWORD);
		DROPDOWN_SetSel(hItem, 0);
		DROPDOWN_DeleteItem(hItem, 2);
		DROPDOWN_DeleteItem(hItem, 1);
		if (NCode >= ADMIN_LEVEL)
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(79));
		if (NCode >= SERVICE_LEVEL)
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(80));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_PASSWORD);
		MULTIEDIT_SetText(hItem, "");

		if (NCode == NO_USER)
		{
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_NEW_PASSWORD));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_USER_NEW_PASSWORD));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_PASSWORD));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONFIRM_PASSWORD));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANGE_PASSWORD));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_PASSWORD));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LOGOUT));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_NEW_PASSWORD);
			MULTIEDIT_SetText(hItem, "AABB");
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_CONFIRM_PASSWORD);
			MULTIEDIT_SetText(hItem, "BBAA");
			WM_HideWindow(hItem);
		}
		else if (NCode >= USER_LEVEL)
		{
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER_NEW_PASSWORD));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_USER_NEW_PASSWORD));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_NEW_PASSWORD));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONFIRM_PASSWORD));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANGE_PASSWORD));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CHANGE_PASSWORD));
			WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_LOGOUT));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_NEW_PASSWORD);
			MULTIEDIT_SetText(hItem, "AABB");
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_CONFIRM_PASSWORD);
			MULTIEDIT_SetText(hItem, "BBAA");
			WM_ShowWindow(hItem);
		}
		WM_InvalidateWindow(pMsg->hWin);

		break;
	case WM_DELETE:
		hWinLogin = 0;
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSetLogin(WM_HWIN hParent)
{
	if(hWinLogin)
	{
		WM_DeleteWindow(hWinLogin);
		hWinLogin = 0;
	}
	hWinLogin = GUI_CreateDialogBox(_aSetLoginDialogCreate, GUI_COUNTOF(_aSetLoginDialogCreate), _cbSetLoginDialog, hParent, 0, 0);
	return hWinLogin;
}

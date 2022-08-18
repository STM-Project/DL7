#include "SetUSBDLG.h"
#include "draw.h"
#include "skins.h"
#include "PopUpMessageDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "mini-printf.h"
#include "dtos.h"

#include "LISTVIEW_files.h"

#include "USBtask.h"
#include "archive.h"
#include "fatfs.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_SET_USB     						(GUI_ID_USER + 300)
#define ID_TEXT_ARCHIVE										(GUI_ID_USER + 301)
#define ID_TEXT_SETTINGS     							(GUI_ID_USER + 302)
#define ID_BUTTON_EXIT     								(GUI_ID_USER + 303)
#define ID_TEXT_FILE_SIZE_SD							(GUI_ID_USER + 304)
#define ID_LISTVIEW_SD										(GUI_ID_USER + 305)
#define ID_TEXT_FILE_SIZE_USB							(GUI_ID_USER + 306)
#define ID_LISTVIEW_USB  	 								(GUI_ID_USER + 307)
#define ID_BUTTON_COPY_PARAMETERS_TO_USB	(GUI_ID_USER + 308)
#define ID_BUTTON_COPY_PARAMETERS_TO_SD		(GUI_ID_USER + 309)
#define ID_BUTTON_COPY_ACTUAL_ARCH				(GUI_ID_USER + 310)
#define ID_BUTTON_COPY_SELECTED_FILE			(GUI_ID_USER + 311)
#define ID_BUTTON_DELETE_SELECTED_FILE		(GUI_ID_USER + 312)

static uint8_t USBFlag = 0;
static TaskHandle_t vtaskPopulateListHandle;
static WM_HWIN hUSBWin=0;

static const GUI_WIDGET_CREATE_INFO _aSetUSB[] =
{
	{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_USB, 0, 0, 740, 420, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_EXIT, 618, 360, 120, 58, 0, 0x0, 0 },
	{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_SD, 6, 4, 352, 380, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "", ID_TEXT_FILE_SIZE_SD, 6, 384, 352, 30, 0, 0x64, 0 },

	{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_USB, 6, 213, 352, 171, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "", ID_TEXT_FILE_SIZE_USB, 6, 384, 352, 30, 0, 0x64, 0 },

	{ TEXT_CreateIndirect, "", ID_TEXT_ARCHIVE, 370, 5, 360, 30, TEXT_CF_VCENTER | TEXT_CF_HCENTER, 0x64, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_COPY_ACTUAL_ARCH, 370, 40, 360, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_COPY_SELECTED_FILE, 370, 90, 360, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_DELETE_SELECTED_FILE, 370, 166, 360, 40, 0, 0x0, 0 },

	{ TEXT_CreateIndirect, "", ID_TEXT_SETTINGS, 370, 225, 360, 30, TEXT_CF_VCENTER | TEXT_CF_HCENTER, 0x64, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_COPY_PARAMETERS_TO_USB, 370, 260, 360, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_COPY_PARAMETERS_TO_SD, 370, 310, 360, 40, 0, 0x0, 0 },
};

void vtaskPopulateList(void *pvParameters)
{
	WM_MakeModal(hUSBWin);
	WM_HWIN hItem = WM_GetDialogItem(hUSBWin, ID_LISTVIEW_SD);
	LISTVIEW_PopulateWithFiles(hItem,"0:/archive");
	LISTVIEW_PopulateWithFiles(hItem,"0:/prtscr");
	LISTVIEW_SetSel(hItem, 0);
	hItem = WM_GetDialogItem(hUSBWin, ID_BUTTON_EXIT);
	WM_EnableWindow(hItem);
	WM_MakeModal(0);
	vTaskDelete(NULL);
}

static void CreatePopulateListTask(void)
{
	xTaskCreate(vtaskPopulateList, (char* )"PopulateList", 1024, NULL, ( unsigned portBASE_TYPE )0, &vtaskPopulateListHandle);
}

void CopyParametersFromUSB(void)
{
	sendUSBEvent(COPY_PAR_TO_SD);
}

static void _cbSetUSB(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	char textBuffer[200]={0};
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXIT);
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETTINGS);
		TEXT_SetText(hItem, GUI_LANG_GetText(74));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCHIVE);
		TEXT_SetText(hItem, GUI_LANG_GetText(4));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
		TEXT_SetText(hItem, "");
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_USB);
		TEXT_SetText(hItem, "");
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SD);
		LISTVIEW_SetAutoScrollH(hItem, 0);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_BLUE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetHeaderHeight(hItem, 30);
		LISTVIEW_SetRowHeight(hItem, 25);
		LISTVIEW_AddColumn(hItem, 1, "", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 160, GUI_LANG_GetText(4), GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 155, GUI_LANG_GetText(274), GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_SetCompareFunc(hItem, 1, LISTVIEW_CompareText);
		LISTVIEW_SetCompareFunc(hItem, 2, LISTVIEW_CompareText);
		LISTVIEW_EnableSort(hItem);
		LISTVIEW_SetSort(hItem, 2, 0);
		LISTVIEW_SetRBorder(hItem, 5);
		LISTVIEW_SetSel(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
		LISTVIEW_SetAutoScrollH(hItem, 1);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_BLUE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetHeaderHeight(hItem, 30);
		LISTVIEW_SetRowHeight(hItem, 25);
		LISTVIEW_SetRBorder(hItem, 5);
		LISTVIEW_AddColumn(hItem, 1, "", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 160, "USB", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 160, GUI_LANG_GetText(274), GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_SetCompareFunc(hItem, 1, LISTVIEW_CompareText);
		LISTVIEW_SetCompareFunc(hItem, 2, LISTVIEW_CompareText);
		LISTVIEW_EnableSort(hItem);
		LISTVIEW_SetSort(hItem, 2, 0);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_PARAMETERS_TO_USB);
		BUTTON_SetText(hItem, GUI_LANG_GetText(88));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_SELECTED_FILE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(122));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DELETE_SELECTED_FILE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(123));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_ACTUAL_ARCH);
		BUTTON_SetText(hItem, GUI_LANG_GetText(161));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_PARAMETERS_TO_SD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(89));
		if (PASSWORDS_GetCurrentLevel() < ADMIN_LEVEL)
			WM_HideWindow(hItem);

		CreatePopulateListTask();

		sendUSBEvent(START);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_EXIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_LISTVIEW_SD:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				LISTVIEW_GetFileFullPath(pMsg->hWinSrc, SrcFilePath);
				float2stri(textBuffer, FATFS_GetFileSize(SrcFilePath), 2);
				strcat(textBuffer, " kB\0");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
				TEXT_SetText(hItem, textBuffer);

				break;
			}
			break;
		case ID_LISTVIEW_USB:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				LISTVIEW_GetFileFullPath(pMsg->hWinSrc, SrcFilePath);
				float2stri(textBuffer, FATFS_GetFileSize(SrcFilePath), 2);
				strcat(textBuffer, " kB\0");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_USB);
				TEXT_SetText(hItem, textBuffer);
			}
			break;
		case ID_BUTTON_COPY_PARAMETERS_TO_USB:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
				TEXT_SetText(hItem, "");

				if (GetUSBState() == USB_OK)
				{
					sendUSBEvent(COPY_PAR_TO_USB);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
					TEXT_SetText(hItem, GUI_LANG_GetText(125));
				}
				break;
			}
			break;

		case ID_BUTTON_COPY_PARAMETERS_TO_SD:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
				TEXT_SetText(hItem, "");

				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
				if(0 <= LISTVIEW_GetSel(hItem))
				{
					if (GetUSBState() == USB_OK)
					{
						LISTVIEW_GetFileFullPath(hItem,SrcFilePath);
						mini_snprintf(textBuffer, 200, "%s\n%s %s", GUI_LANG_GetText(273), GUI_LANG_GetText(170), GUI_LANG_GetText(176));
						CreateConfirmMessage(textBuffer, DEFAULT_COLOR, DEFAULT_COLOR, CopyParametersFromUSB, NULL);
					}
					else
					{
						hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
						TEXT_SetText(hItem, GUI_LANG_GetText(125));
					}
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
					TEXT_SetText(hItem, GUI_LANG_GetText(124));
				}
				break;
			}
			break;

		case ID_BUTTON_COPY_SELECTED_FILE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
				TEXT_SetText(hItem, "");

				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SD);
				if(0 <= LISTVIEW_GetSel(hItem))
				{
					LISTVIEW_SetFileNamesToCopy(hItem);
					if (GetUSBState() == USB_OK)
						sendUSBEvent(COPY_FILE);
					else
					{
						hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
						TEXT_SetText(hItem, GUI_LANG_GetText(125));
					}
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
					TEXT_SetText(hItem, GUI_LANG_GetText(124));
				}
				break;
			}
			break;
		case ID_BUTTON_DELETE_SELECTED_FILE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:

				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SD);
				if(0 <= LISTVIEW_GetSel(hItem))
				{
					LISTVIEW_GetFileFullPath(hItem,DeleteFilePath);
					if (0==ARCHIVE_IsFileCurrentArchive(DeleteFilePath))
					{
						sendUSBEvent(DELETE_FILE);
						int sel = LISTVIEW_GetSel(hItem);
						LISTVIEW_DeleteRowSorted(hItem, sel);
						LISTVIEW_SetSel(hItem,sel);
					}
					else
					{
						CreateMessage(GUI_LANG_GetText(171), GUI_YELLOW, GUI_BLACK);
					}
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
					TEXT_SetText(hItem, GUI_LANG_GetText(124));
				}
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_COPY_ACTUAL_ARCH:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
				TEXT_SetText(hItem, "");

				if (GetUSBState() == USB_OK)
				{
					sendUSBEvent(COPY_ACTUAL_ARCHIVES);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
					TEXT_SetText(hItem, GUI_LANG_GetText(125));
				}
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		if (USBFlag)
		{
			DRAW_RoundedFrame(2, 0, 359, 206, 4, 2, GUI_BLUE);
			DRAW_RoundedFrame(2, 209, 359, 207, 4, 2, GUI_BLUE);
		}
		else
			DRAW_RoundedFrame(2, 0, 359, 416, 4, 2, GUI_BLUE);
		break;
	case WM_TIMER:
		if (USBFlag == 0 && GetUSBState() == USB_OK)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SD);
			WM_SetYSize(hItem, 169);
			LISTVIEW_SetAutoScrollV(hItem, 0);
			LISTVIEW_SetAutoScrollV(hItem, 1);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
			WM_MoveChildTo(hItem, 6, 173);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
			WM_ShowWindow(hItem);
			LISTVIEW_PopulateWithParametersFiles(hItem, "1:");
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_USB);
			WM_ShowWindow(hItem);

			USBFlag = 1;
			WM_InvalidateWindow(pMsg->hWin);
		}
		else if (USBFlag == 1 && GetUSBState() == REGISTER_USB)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
			LISTVIEW_DeleteAllRows(hItem);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_USB);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_SD);
			WM_MoveChildTo(hItem, 6, 384);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_SD);
			WM_SetYSize(hItem, 380);
			LISTVIEW_SetAutoScrollV(hItem, 0);
			LISTVIEW_SetAutoScrollV(hItem, 1);

			USBFlag = 0;
			WM_InvalidateWindow(pMsg->hWin);
		}

		WM_RestartTimer(pMsg->Data.v, 500);
		break;

	case WM_DELETE:
		sendUSBEvent(STOP);
		break;
	case WM_USER_LOGIN:

		if (PASSWORDS_GetCurrentLevel() > USER_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_PARAMETERS_TO_USB);
			WM_ShowWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_PARAMETERS_TO_SD);
			WM_ShowWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DELETE_SELECTED_FILE);
			WM_ShowWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETTINGS);
			WM_ShowWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_PARAMETERS_TO_USB);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_PARAMETERS_TO_SD);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DELETE_SELECTED_FILE);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SETTINGS);
			WM_HideWindow(hItem);
		}

		break;
	case WM_USER_REFRESH_USB_LISTVIEW:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
		LISTVIEW_DeleteAllRows(hItem);
		LISTVIEW_PopulateWithParametersFiles(hItem, "1:");
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSetUSB(WM_HWIN hParent)
{
	hUSBWin = GUI_CreateDialogBox(_aSetUSB, GUI_COUNTOF(_aSetUSB), _cbSetUSB, hParent, 0, 0);
	USERMESSAGE_ChangeWinTitle("USB");
	USBFlag = 0;
	WM_CreateTimer(hUSBWin, 1, 1, 0);

	return hUSBWin;
}

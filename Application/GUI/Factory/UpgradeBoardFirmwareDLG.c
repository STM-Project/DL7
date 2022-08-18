/*
 * CardsBoot.c
 *
 *  Created on: 25.08.2020
 *      Author: RafalMar
 */

#include "UpgradeBoardFirmwareDLG.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"

#include "task.h"
#include "ExpansionCards.h"
#include "ExpansionCards_typedef.h"
#include "ExpansionCards_descriptions.h"
#include "stm32h7xx_hal.h"

#include "mini-printf.h"
#include "dtos.h"
#include "draw.h"
#include "buzzer.h"
#include "skins.h"
#include "UserMessage.h"
#include "passwords.h"

#include "TaskUpgradeBoardFirmware.h"

#include "fatfs.h"
#include "USBtask.h"
#include "LISTVIEW_files.h"

#define ID_WINDOW_FIRMWARE_UPDATA		(GUI_ID_USER + 1)
#define ID_LISTVIEW_USB   					(GUI_ID_USER + 2)
#define ID_TEXT_FILE_SIZE_USB 			(GUI_ID_USER + 3)
#define ID_TEXT_SLOT     						(GUI_ID_USER + 4)
#define ID_TEXT_CARD_DESCRIPTION    (GUI_ID_USER + 5)
#define ID_TEXT_UPGRADE_MESSAGE     (GUI_ID_USER + 6)
#define ID_BUTTON_GET_BOARD_ID     	(GUI_ID_USER + 7)
#define ID_BUTTON_UPLOAD     				(GUI_ID_USER + 8)
#define ID_BUTTON_EXIT     					(GUI_ID_USER + 9)

extern GUI_CONST_STORAGE GUI_BITMAP bmNO;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

WM_HWIN CardBootWindow;

static EXPANSION_CARD localExpansionCard;

static uint8_t localBoardNumber = 0;
static uint8_t USBFlag = 0;
static int fileSelected = 0;
extern uint32_t isTaskCreated;

FILINFO t_filinfo;
DIR t_dir;

static const GUI_WIDGET_CREATE_INFO _aDialogCardBoot[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_FIRMWARE_UPDATA, 0, 0, 740, 420, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SLOT, 5, 5, 80, 30, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CARD_DESCRIPTION, 90, 5, 310, 30, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_USB, 10, 45, 720, 185, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_FILE_SIZE_USB, 10, 235, 720, 30, 0, 0x64, 0 },

{ BUTTON_CreateIndirect, "", ID_BUTTON_GET_BOARD_ID, 5, 280, 360, 40, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_UPLOAD, 375, 280, 360, 40, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_UPGRADE_MESSAGE, 5, 325, 730, 30, TEXT_CF_VCENTER | TEXT_CF_HCENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_EXIT, 618, 360, 120, 58, 0, 0x0, 0 }, };

static void _cbDialogCardBoot(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[50];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SLOT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		mini_snprintf(GUITextBuffer, 10, "Slot %c", 'A' + localBoardNumber);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_DESCRIPTION);
		TEXT_SetText(hItem, GetExpansionCardDescription(localExpansionCard.type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_GET_BOARD_ID);
		BUTTON_SetText(hItem, GUI_LANG_GetText(240));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UPLOAD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(241));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXIT);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
		LISTVIEW_SetAutoScrollH(hItem, 1);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_BLUE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetHeaderHeight(hItem, 30);
		LISTVIEW_SetRowHeight(hItem, 25);
		LISTVIEW_SetRBorder(hItem, 5);
		LISTVIEW_AddColumn(hItem, 1, "", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 520, "USB", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 160, GUI_LANG_GetText(274), GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_SetCompareFunc(hItem, 1, LISTVIEW_CompareText);
		LISTVIEW_SetCompareFunc(hItem, 2, LISTVIEW_CompareText);
		LISTVIEW_EnableSort(hItem);
		LISTVIEW_SetSort(hItem, 2, 0);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_USB);
		TEXT_SetText(hItem, "");
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);

		sendUSBEvent(START);
		USBFlag=0;

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_GET_BOARD_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (0 == isTaskCreated)
				{
					localExpansionCard.firmwareRevision=0;
					localExpansionCard.type=0;
					IOBoards_InitalizeBoard(localBoardNumber,&localExpansionCard);
					if(0==localExpansionCard.type)
						mini_snprintf(GUITextBuffer, 45, " ---------- ");
					else
					{
						float2stri(&GUITextBuffer[45], (float)localExpansionCard.firmwareRevision / 100, 2);
						mini_snprintf(GUITextBuffer, 45, "%s   Firmware %s",GetExpansionCardDescription(localExpansionCard.type),&GUITextBuffer[45]);
					}
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UPGRADE_MESSAGE);
					TEXT_SetText(hItem, GUITextBuffer);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_DESCRIPTION);
					TEXT_SetText(hItem, GetExpansionCardDescription(localExpansionCard.type));
				}
				break;
			}
			break;
		case ID_BUTTON_UPLOAD:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
				if(0 <= LISTVIEW_GetSel(hItem) && (0 == isTaskCreated))
				{
					LISTVIEW_GetFileFullPath(hItem,SrcFilePath);
					sendUSBEvent(START_TASK_FIRMWARE_UPGRADE);
				}
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
				float2stri(GUITextBuffer, FATFS_GetFileSize(SrcFilePath), 2);
				strcat(GUITextBuffer, " kB\0");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILE_SIZE_USB);
				TEXT_SetText(hItem, GUITextBuffer);
			}
			break;
		case ID_BUTTON_EXIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (0 == isTaskCreated)
				{
					sendUSBEvent(STOP);
					fileSelected = 0;
					WM_DeleteWindow(pMsg->hWin);
				}
				break;
			}
			break;
		}
		break;

	case WM_TIMER:
		if (USBFlag == 0 && GetUSBState() == USB_OK)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
			WM_ShowWindow(hItem);
			LISTVIEW_PopulateWithBinaryFiles(hItem, "1:");
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

			USBFlag = 0;
			WM_InvalidateWindow(pMsg->hWin);
		}
		WM_RestartTimer(pMsg->Data.v, 500);
		break;

	case WM_POST_PAINT:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
		if(WM_IsVisible(hItem))
			DRAW_RoundedFrame(5, 40, 730, 230, 4, 2, GUI_BLUE);
		break;

	case WM_USER_BOARD_FIRMWARE_UPGRADE:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UPGRADE_MESSAGE);
		NCode = pMsg->Data.v;
		switch (NCode)
		{
		case READ_FILE_OK:
			TEXT_SetText(hItem, GUI_LANG_GetText(242));
			break;
		case READ_FILE_ERROR:
			TEXT_SetText(hItem, GUI_LANG_GetText(243));
			break;
		case FIRMWARE_UPGRADE_INIT:
			TEXT_SetText(hItem, GUI_LANG_GetText(244));
			break;
		case FIRMWARE_UPGRADE_INIT_ERROR:
			TEXT_SetText(hItem, GUI_LANG_GetText(245));
			break;
		case START_ERASE_FLASH:
			TEXT_SetText(hItem, GUI_LANG_GetText(246));
			break;
		case START_ERASE_FLASH_FAIL:
			TEXT_SetText(hItem, GUI_LANG_GetText(247));
			break;
		case ERASE_FLASH_FAIL:
			TEXT_SetText(hItem, GUI_LANG_GetText(248));
			break;
		case ERASE_FLASH_SUCCESS:
			TEXT_SetText(hItem, GUI_LANG_GetText(249));
			break;
		case NONE_RESPONSE:
			TEXT_SetText(hItem, GUI_LANG_GetText(226));
			break;
		case CRC_FAIL:
			TEXT_SetText(hItem, GUI_LANG_GetText(251));
			break;
		case WRITE_FLASH_FAIL:
			TEXT_SetText(hItem, GUI_LANG_GetText(252));
			break;
		case WRITE_DATA_COMPLETE:
			vTaskDelay(1000);
			IOBoards_InitalizeBoard(localBoardNumber, &localExpansionCard);
			if(0==localExpansionCard.type)
			{
				TEXT_SetText(hItem, GUI_LANG_GetText(252));
			}
			else
			{
				TEXT_SetText(hItem, GUI_LANG_GetText(253));
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_DESCRIPTION);
				TEXT_SetText(hItem, GetExpansionCardDescription(localExpansionCard.type));
			}
			break;
		}
		break;
	case WM_USER_BOARD_WRITE_DATA_INPROGRESS:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UPGRADE_MESSAGE);
		mini_snprintf(GUITextBuffer, 50, "%s %d", GUI_LANG_GetText(250),pMsg->Data.v);
		TEXT_SetText(hItem, GUITextBuffer);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

uint8_t GetBoardNumber(void)
{
	return localBoardNumber;
}

WM_HWIN CreateCardBootWin(WM_HWIN hParent, uint8_t boardNumber)
{
	WM_HWIN hWin;
	localBoardNumber = boardNumber;
	GetExpansionBoard(localBoardNumber, &localExpansionCard);

	hWin = GUI_CreateDialogBox(_aDialogCardBoot, GUI_COUNTOF(_aDialogCardBoot), _cbDialogCardBoot, hParent, 0, 0);
	WM_CreateTimer(hWin, 0, 1, 0);
	CardBootWindow = hWin;
	return hWin;
}

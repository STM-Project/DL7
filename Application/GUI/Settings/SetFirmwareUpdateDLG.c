/*
 * SetFirmwareUpdateDLG.c
 *
 *  Created on: 30 lis 2021
 *      Author: TomaszSokolowski
 */

#include "SetFirmwareUpdateDLG.h"
#include "skins.h"
#include "draw.h"
#include "UserMessage.h"
#include "passwords.h"
#include "buzzer.h"

#include "FreeRTOS.h"
#include "task.h"

#include "PopUpMessageDLG.h"
#include "USBtask.h"
#include "LISTVIEW_files.h"
#include "fatfs.h"
#include "shift_reg.h"
#include "archive.h"
#include "SetGeneralDLG.h"

#define ID_WINDOW_FIRMWARE_UPDATE			(GUI_ID_USER + 601)
#define ID_LISTVIEW_USB								(GUI_ID_USER + 602)
#define ID_BUTTON_UPDATE_FILE_SYSTEM	(GUI_ID_USER + 603)
#define ID_BUTTON_RESTART							(GUI_ID_USER + 604)
#define ID_BUTTON_EXIT								(GUI_ID_USER + 605)

extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

static WM_HWIN hSetFirmwareUpdateWindow=0;
static uint8_t USBFlag = 0;
static char NewFileSystemFolder[64] __attribute__ ((section(".sdram")));

TaskHandle_t vtaskNewFileSystemHandle;

static const GUI_WIDGET_CREATE_INFO _aSetFirmwareUpdateWindow[] =
{
	{ WINDOW_CreateIndirect, "", ID_WINDOW_FIRMWARE_UPDATE, 0, 0, 740, 420, 0, 0x0, 0 },
	{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_USB, 6, 4, 352, 380, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_UPDATE_FILE_SYSTEM, 370, 10, 360, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_RESTART, 370, 60, 360, 40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "", ID_BUTTON_EXIT, 618, 360, 120, 58, 0, 0x0, 0 },
};

static int CleanFolder(char *folderPath)
{
	char filePath[128]={0};
	FILINFO filInfo;
	DIR dir;
	if (FR_OK == f_opendir(&dir, folderPath))
	{
		if (FR_OK == f_readdir(&dir, &filInfo))
		{
			BLUE_LED_ON;
			while (0 != filInfo.fname[0])
			{
				BLUE_LED_TOGGLE;
				if (AM_DIR != filInfo.fattrib)
				{
					strcpy(filePath,folderPath);
					strcat(filePath,"/");
					strcat(filePath,filInfo.fname);
					if(FR_OK != f_unlink(filePath))
					{
						f_closedir(&dir);
						return 0;
					}
				}
				if (FR_OK != f_readdir(&dir, &filInfo))
					break;
			}
			BLUE_LED_OFF;
		}
		f_closedir(&dir);
		return 1;
	}
	else
	{
		return 0;
	}
}

static int CleansFolderOnSD (void)
{
	if (0 == CleanFolder("0:lang"))
		return 0;
	if (0 == CleanFolder("0:image"))
		return 0;
	if (0 == CleanFolder("0:website"))
		return 0;
	return 1;
}

static int ChcekFolder (char* FolderPath)
{
	FILINFO filInfo;
	FRESULT res;
	char subFolderPath[128]={0};

	strcpy(subFolderPath,FolderPath);
	strcat(subFolderPath,"/lang\0");
	res = f_stat(subFolderPath, &filInfo);
	if(FR_NO_FILE == res)
		return 0;

	strcpy(subFolderPath,FolderPath);
	strcat(subFolderPath,"/image\0");
	res = f_stat(subFolderPath, &filInfo);
	if(FR_NO_FILE == res)
		return 0;

	strcpy(subFolderPath,FolderPath);
	strcat(subFolderPath,"/website\0");
	res = f_stat(subFolderPath, &filInfo);
	if(FR_NO_FILE == res)
		return 0;

	return 1;
}

static int CopyFolder(char *DstFolder, char* SrcFolder)
{
	FILINFO t_filinfo;
	DIR t_dir;
	int ret = 1;
	char dstFilePath[128]={0};
	char srcFilePath[128]={0};

	if (FR_OK == f_opendir(&t_dir, SrcFolder))
	{
		if (FR_OK == f_readdir(&t_dir, &t_filinfo))
		{
			while (0 != t_filinfo.fname[0])
			{
				if (AM_DIR != t_filinfo.fattrib)
				{
					strcpy(dstFilePath,DstFolder);
					strcat(dstFilePath,"/");
					strcat(dstFilePath,t_filinfo.fname);
					strcat(dstFilePath,"\0");

					strcpy(srcFilePath,SrcFolder);
					strcat(srcFilePath,"/");
					strcat(srcFilePath,t_filinfo.fname);
					strcat(srcFilePath,"\0");

					if (FR_OK != FATFS_CopyFile(srcFilePath, dstFilePath))
					{
						ret = 0;
						break;
					}
				}
				if (FR_OK != f_readdir(&t_dir, &t_filinfo))
					break;
			}
		}
		else
		{
			ret = 0;
		}
		f_closedir(&t_dir);
	}
	else
	{
		ret = 0;
	}
	return ret;
}

static void CopyDescriptionFile(char* descFilepath)
{
	FILINFO filInfo;
	FRESULT res;
	res = f_stat(descFilepath, &filInfo);
	if(FR_NO_FILE == res)
	{
		return;
	}
	else
	{
		FATFS_CopyFile(descFilepath, "0:config/desc.txt");
	}
}

static void vtaskNewFileSystem(void *argument)
{
	char *name = (char*) argument;
	char folderName[64] = { 0 };
	strcpy(folderName, name);
	char tempfolderName[64] = { 0 };

	if(0 == ChcekFolder(folderName))
	{
		CreateMessage(GUI_LANG_GetText(269), GUI_YELLOW, GUI_BLACK);
		vTaskDelete(NULL);
	}

	if(0== CleansFolderOnSD())
	{
		CreateMessage(GUI_LANG_GetText(167), GUI_YELLOW, GUI_BLACK);
		vTaskDelete(NULL);
	}

	strcpy(tempfolderName, folderName);
	strcat(tempfolderName, "/image");
	if (0 == CopyFolder("0:image", tempfolderName))
	{
		CreateMessage(GUI_LANG_GetText(268), GUI_YELLOW, GUI_BLACK);
		vTaskDelete(NULL);
	}

	strcpy(tempfolderName, folderName);
	strcat(tempfolderName, "/lang");
	if (0 == CopyFolder("0:lang", tempfolderName))
	{
		CreateMessage(GUI_LANG_GetText(268), GUI_YELLOW, GUI_BLACK);
		vTaskDelete(NULL);
	}

	strcpy(tempfolderName, folderName);
	strcat(tempfolderName, "/website");
	if (0 == CopyFolder("0:website", tempfolderName))
	{
		CreateMessage(GUI_LANG_GetText(268), GUI_YELLOW, GUI_BLACK);
		vTaskDelete(NULL);
	}

	strcpy(tempfolderName, folderName);
	strcat(tempfolderName, "/config/desc.txt");
	CopyDescriptionFile(tempfolderName);

	CreateMessage(GUI_LANG_GetText(267), GUI_YELLOW, GUI_BLACK);
	vTaskDelete(NULL);
}

void CreatevTaskNewFileSystem(char *charFile)
{
	xTaskCreate(vtaskNewFileSystem, "vNewFileSystem", 2048, (void*) charFile, (unsigned portBASE_TYPE ) 1, &vtaskNewFileSystemHandle);
}

static void SERVICE_UpdateFileSystem(void)
{
	WM_HWIN hItem = WM_GetDialogItem(hSetFirmwareUpdateWindow, ID_LISTVIEW_USB);
	if(0 <= LISTVIEW_GetSel(hItem))
	{
		LISTVIEW_GetFileFullPath(hItem,NewFileSystemFolder);
		CreatevTaskNewFileSystem(NewFileSystemFolder);
	}
	else
	{
		CreateMessage(GUI_LANG_GetText(269), GUI_YELLOW, GUI_BLACK);
	}
}

static void _cbSetFirmwareUpdateWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UPDATE_FILE_SYSTEM);
		WM_DisableWindow(hItem);
		BUTTON_SetText(hItem, GUI_LANG_GetText(270));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_RESTART);
		BUTTON_SetText(hItem, "Restart");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXIT);
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_BLUE);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetHeaderHeight(hItem, 30);
		LISTVIEW_SetRowHeight(hItem, 30);
		LISTVIEW_SetRBorder(hItem, 5);
		LISTVIEW_AddColumn(hItem, 1, "", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 350, "USB", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_SetCompareFunc(hItem, 1, LISTVIEW_CompareText);
		LISTVIEW_EnableSort(hItem);
		LISTVIEW_SetSort(hItem, 1, 1);
		WM_HideWindow(hItem);

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
				WM_DeleteWindow(pMsg->hWin);
				BUZZER_Beep();
				break;
			}
			break;
		case ID_BUTTON_UPDATE_FILE_SYSTEM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				SERVICE_UpdateFileSystem();
				break;
			}
			break;;
		case ID_BUTTON_RESTART:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				CreateConfirmMessage(GUI_LANG_GetText(271), DEFAULT_COLOR, DEFAULT_COLOR, SERVICE_RestartDevice, NULL);
				break;
			}
			break;
		}
		break;
	case WM_TIMER:
		if (0 == USBFlag && USB_OK == GetUSBState())
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
			LISTVIEW_PopulateWithFolders(hItem, "1:/");
			WM_ShowWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UPDATE_FILE_SYSTEM);
			WM_EnableWindow(hItem);

			USBFlag = 1;
			WM_InvalidateWindow(pMsg->hWin);
		}
		else if (1 == USBFlag && GetUSBState() == REGISTER_USB)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
			LISTVIEW_DeleteAllRows(hItem);
			WM_HideWindow(hItem);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UPDATE_FILE_SYSTEM);
			WM_DisableWindow(hItem);

			USBFlag = 0;
			WM_InvalidateWindow(pMsg->hWin);
		}
		WM_RestartTimer(pMsg->Data.v, 500);
		break;
	case WM_POST_PAINT:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_USB);
		if(WM_IsVisible(hItem))
			DRAW_RoundedFrame(2, 0, 359, 416, 4, 2, GUI_BLUE);
		break;
	case WM_DELETE:
		sendUSBEvent(STOP);
		hSetFirmwareUpdateWindow = 0;
		break;
	case WM_USER_LOGIN:
		if(ADMIN_LEVEL > PASSWORDS_GetCurrentLevel())
			WM__DeleteSecure(pMsg->hWin);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateSetFirmwareUpdateWindow(WM_HWIN hParent)
{
	hSetFirmwareUpdateWindow = GUI_CreateDialogBox(_aSetFirmwareUpdateWindow, GUI_COUNTOF(_aSetFirmwareUpdateWindow), _cbSetFirmwareUpdateWindow, hParent, 0, 0);
	WM_CreateTimer(hSetFirmwareUpdateWindow, 0, 10, 0);
	WM_SetStayOnTop(hSetFirmwareUpdateWindow, 1);
	return hSetFirmwareUpdateWindow;
}

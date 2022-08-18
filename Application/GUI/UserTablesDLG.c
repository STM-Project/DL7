#include <UserLabels.h>
#include "UserTablesDLG.h"
#include "draw.h"
#include "skins.h"
#include "UserMessage.h"

#include "WindowManagement.h"
#include "buzzer.h"
#include "fram.h"
#include "SingleChannelDLG.h"
#include "parameters.h"

#include "channels_typedef.h"
#include "EDIT_double.h"

#define ID_WINDOW_USER_TABLES    	(GUI_ID_USER + 0x101)
#define ID_MULTIPAGE_USER_TABLES  (GUI_ID_USER + 0x102)
#define ID_WINDOW_USER_TABLE 			(GUI_ID_USER + 0x110)
#define ID_USER_TABLE_VALUE				(GUI_ID_USER + 0x111)
#define ID_USER_TABLE_DESC				(GUI_ID_USER + 0x121)
#define ID_USER_TABLE_UNIT 				(GUI_ID_USER + 0x131)

static WM_HWIN CreateUserTableDLG(void);
void openSingleChannelWindow(void);
void TABLE_SetText(WM_HWIN hEdit, USER_LABEL *table);

static USER_LABEL tempTableLabel;
WM_HWIN TableWindow = 0;
static int currentUserTableNumber = 7;

static const GUI_WIDGET_CREATE_INFO _aTableDialogCreate[] =
{
		{ WINDOW_CreateIndirect, "Table Window", ID_WINDOW_USER_TABLE, 0, 0, 732, 376, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aUserTablesDialogCreate[] =
{
		{ WINDOW_CreateIndirect, "Tabela wyników", ID_WINDOW_USER_TABLES, 0, 60, 740, 420, 0, 0x0, 0 },
		{ MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_USER_TABLES, 4, 4, 732, 412, 0, 0x0, 0 },
};

static void _cbUserTablesDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int i, j, k;

	switch (pMsg->MsgId)
	{
	case WM_TIMER:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_TABLES);
		UserMessage.MsgId = WM_USER_REFRESH;
		UserMessage.hWin = TableWindow;
		UserMessage.hWinSrc = pMsg->hWin;
		UserMessage.Data.v = MULTIPAGE_GetSelection(hItem);
		WM_SendMessage(TableWindow, &UserMessage);

		WM_RestartTimer(pMsg->Data.v, 1000);
		break;

	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_TABLES);
		TableWindow = CreateUserTableDLG();
		MULTIPAGE_AddEmptyPage(hItem, TableWindow, UserTablesSettings.TablesTitles[0]);
		MULTIPAGE_AddEmptyPage(hItem, TableWindow, UserTablesSettings.TablesTitles[1]);
		MULTIPAGE_AddEmptyPage(hItem, TableWindow, UserTablesSettings.TablesTitles[2]);
		MULTIPAGE_AddEmptyPage(hItem, TableWindow, UserTablesSettings.TablesTitles[3]);
		MULTIPAGE_AddEmptyPage(hItem, TableWindow, UserTablesSettings.TablesTitles[4]);
		MULTIPAGE_AddEmptyPage(hItem, TableWindow, UserTablesSettings.TablesTitles[5]);

		for (i = 0; i < 6; i++)
		{
			k = 0;
			for (j = 0; j < 15; j++)
				k += UserTablesSettings.TablesData[i * 15 + j];
			if (k == -15)
			{
				MULTIPAGE_SetTabWidth(hItem, 0, i);
				MULTIPAGE_DisablePage(hItem, i);
			}
		}
		if (MULTIPAGE_IsPageEnabled(hItem, currentUserTableNumber))
			MULTIPAGE_SelectPage(hItem, currentUserTableNumber);
		else
		{
			for (i = 0; i < 6; i++)
			{
				if (MULTIPAGE_IsPageEnabled(hItem, i))
				{
					MULTIPAGE_SelectPage(hItem, i);
					currentUserTableNumber = i;
					break;
				}
			}
			if (i == 5)
			{
				MULTIPAGE_SelectPage(hItem, 0);
				currentUserTableNumber = 0;
			}
			FRAM_Write(USER_TABLE_SCREEN_ADDR, (uint8_t) currentUserTableNumber);
		}

		UserMessage.MsgId = WM_USER_REFRESH;
		UserMessage.hWin = TableWindow;
		UserMessage.hWinSrc = pMsg->hWin;
		UserMessage.Data.v = MULTIPAGE_GetSelection(hItem);
		WM_SendMessage(TableWindow, &UserMessage);

		WM_CreateTimer(pMsg->hWin, 0, 1000, 0);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_MULTIPAGE_USER_TABLES:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_USER_TABLES);
				UserMessage.MsgId = WM_USER_REFRESH;
				UserMessage.hWin = TableWindow;
				UserMessage.hWinSrc = pMsg->hWin;
				UserMessage.Data.v = MULTIPAGE_GetSelection(hItem);
				WM_SendMessage(TableWindow, &UserMessage);
				FRAM_Write(USER_TABLE_SCREEN_ADDR, (uint8_t) UserMessage.Data.v);
				break;

			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
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

static void _cbTableDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin, hDesc, hUnit, hValue;
	int NCode;
	int Id;
	int i;
	short wWynik;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;

		switch (NCode)
		{
		case WM_NOTIFICATION_RELEASED:
			if ((Id >= ID_USER_TABLE_VALUE) && (Id <= (ID_USER_TABLE_VALUE + 15)))
			{
				i = UserTablesSettings.TablesData[Id - ID_USER_TABLE_VALUE + 15 * currentUserTableNumber];
				if (i > -1)
				{
					GetUserLabel(&tempTableLabel, i);
					if (tempTableLabel.channelType)
						openSingleChannelWindow();
				}
			}
			else if ((Id >= ID_USER_TABLE_DESC) && (Id <= (ID_USER_TABLE_DESC + 15)))
			{
				i = UserTablesSettings.TablesData[Id - ID_USER_TABLE_DESC + 15 * currentUserTableNumber];
				if (i > -1)
				{
					GetUserLabel(&tempTableLabel, i);
					if (tempTableLabel.channelType)
						openSingleChannelWindow();
				}
			}
			else if ((Id >= ID_USER_TABLE_UNIT) && (Id <= (ID_USER_TABLE_UNIT + 15)))
			{
				i = UserTablesSettings.TablesData[Id - ID_USER_TABLE_UNIT + 15 * currentUserTableNumber];
				if (i > -1)
				{
					GetUserLabel(&tempTableLabel, i);
					if (i > -1 && tempTableLabel.channelType)
						openSingleChannelWindow();
				}
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_Line(1, 0, 1, 376, 3, GUI_GRAY);
		DRAW_Line(244, 0, 244, 376, 3, GUI_GRAY);
		DRAW_Line(487, 0, 487, 376, 3, GUI_GRAY);
		DRAW_Line(730, 0, 730, 376, 3, GUI_GRAY);

		DRAW_Line(0, 1, 732, 1, 3, GUI_GRAY);
		DRAW_Line(0, 75, 732, 75, 3, GUI_GRAY);
		DRAW_Line(0, 149, 732, 149, 3, GUI_GRAY);
		DRAW_Line(0, 223, 732, 223, 3, GUI_GRAY);
		DRAW_Line(0, 297, 732, 297, 3, GUI_GRAY);
		DRAW_Line(0, 374, 732, 374, 3, GUI_GRAY);
		break;
	case WM_USER_REFRESH:
		wWynik = 15 * pMsg->Data.v;

		for (i = 0; i < 15; i++)
		{
			hDesc = WM_GetDialogItem(hWin, ID_USER_TABLE_DESC + i);
			hUnit = WM_GetDialogItem(hWin, ID_USER_TABLE_UNIT + i);
			hValue = WM_GetDialogItem(hWin, ID_USER_TABLE_VALUE + i);

			if (UserTablesSettings.TablesData[wWynik+i] > -1)
			{
				if (currentUserTableNumber != pMsg->Data.v)
				{
					GetUserLabel(&tempTableLabel, UserTablesSettings.TablesData[wWynik+i]);
					WM_ShowWindow(hValue);
					WM_ShowWindow(hDesc);
					WM_ShowWindow(hUnit);

					if (tempTableLabel.channelType)
					{
						TABLE_SetText(hValue, &tempTableLabel);

						TEXT_SetText(hDesc, tempTableLabel.description);
						TEXT_SetTextColor(hDesc, GUI_BLUE);
						TEXT_SetText(hUnit, tempTableLabel.unit);
						TEXT_SetTextColor(hUnit, GUI_BLUE);

						if (tempTableLabel.failureMode == 1 && tempTableLabel.labelType == 0)
						{
							if (tempTableLabel.failureState == 1)
							{
								TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);
								TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
							}
							else
							{
								TEXT_SetBkColor(hDesc, GUI_YELLOW);
								TEXT_SetBkColor(hUnit, GUI_YELLOW);
							}
						}
						else
						{
							EDIT_SetSkin(hValue, EDIT_CI_ENABLED);
							TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);
							TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
						}
					}
					else
					{
						EDIT_SetTextMode(hValue);
						EDIT_SetSkin(hValue, EDIT_CI_ENABLED);
						EDIT_SetText(hValue, "-------");
						EDIT_SetTextColor(hValue, EDIT_CI_ENABLED, GUI_GRAY);

						TEXT_SetText(hDesc, tempTableLabel.description);
						TEXT_SetTextColor(hDesc, GUI_GRAY);
						TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);

						TEXT_SetText(hUnit, tempTableLabel.unit);
						TEXT_SetTextColor(hUnit, GUI_GRAY);
						TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
					}
				}
				else
				{
					GetUserLabelValue(&tempTableLabel, UserTablesSettings.TablesData[wWynik+i]);

					if (tempTableLabel.channelType)
					{
						TABLE_SetText(hValue, &tempTableLabel);
						if (tempTableLabel.failureMode == 1)
						{
							if (tempTableLabel.failureState == 1)
							{
								TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);
								TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
							}
							else
							{
								TEXT_SetBkColor(hDesc, GUI_YELLOW);
								TEXT_SetBkColor(hUnit, GUI_YELLOW);
							}
						}
						else
						{
							TEXT_SetBkColor(hDesc, GUI_TRANSPARENT);
							TEXT_SetBkColor(hUnit, GUI_TRANSPARENT);
						}
					}
				}
			}
			else
			{
				if (currentUserTableNumber != pMsg->Data.v)
				{
					WM_HideWindow(hValue);
					WM_HideWindow(hDesc);
					WM_HideWindow(hUnit);
				}
			}
		}
		if (currentUserTableNumber != pMsg->Data.v)
			currentUserTableNumber = pMsg->Data.v;
		break;

	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/***********************************************************************/
WM_HWIN CreateUserTables(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aUserTablesDialogCreate, GUI_COUNTOF(_aUserTablesDialogCreate), _cbUserTablesDialog, WM_HBKWIN, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(2));
	return hWin;
}

static WM_HWIN CreateUserTableDLG(void)
{
	WM_HWIN hWin, hItem;
	int tablesDataIndex, dataCellIndex;
	hWin = GUI_CreateDialogBox(_aTableDialogCreate, GUI_COUNTOF(_aTableDialogCreate), _cbTableDialog, WM_HBKWIN, 0, 0);

	currentUserTableNumber = FRAM_Read(USER_TABLE_SCREEN_ADDR);
	if (currentUserTableNumber < 0 || currentUserTableNumber > 6)
		currentUserTableNumber = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			tablesDataIndex = j + 5 * i + 15 * currentUserTableNumber;
			dataCellIndex = j + 5 * i;
			TEXT_CreateEx(3 + 243 * i, 3 + 74 * j, 240, 36, hWin, WM_CF_SHOW,
			GUI_TA_LEFT | GUI_TA_VCENTER, ID_USER_TABLE_DESC + (dataCellIndex), "");
			hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_DESC + (dataCellIndex));
			TEXT_SetBkColor(hItem, GUI_TRANSPARENT);
			TEXT_SetTextColor(hItem, GUI_BLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);

			TEXT_CreateEx(148 + 243 * i, 39 + 74 * j, 95, 35, hWin, WM_CF_SHOW,
			GUI_TA_LEFT | GUI_TA_VCENTER, ID_USER_TABLE_UNIT + (dataCellIndex), "");
			hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_UNIT + (dataCellIndex));
			TEXT_SetBkColor(hItem, GUI_TRANSPARENT);
			TEXT_SetTextColor(hItem, GUI_BLUE);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);

			EDIT_CreateAsChild(3 + 243 * i, 39 + 74 * j, 145, 35, hWin, ID_USER_TABLE_VALUE + (j + 5 * i),
			WM_CF_SHOW | EDIT_CI_DISABLED | GUI_TA_RIGHT | GUI_TA_VCENTER, 11);
			hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_VALUE + dataCellIndex);
			EDIT_SetFocussable(hItem, 0);
			EDIT_SetSkin(hItem, EDIT_CI_DISABLED);
			EDIT_SetSkin(hItem, EDIT_CI_ENABLED);
			EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);

			if (UserTablesSettings.TablesData[tablesDataIndex] > -1)
			{

				GetUserLabel(&tempTableLabel, UserTablesSettings.TablesData[tablesDataIndex]);

				if (tempTableLabel.channelType)
				{
					hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_DESC + (dataCellIndex));
					TEXT_SetText(hItem, tempTableLabel.description);
					hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_UNIT + (dataCellIndex));
					TEXT_SetText(hItem, tempTableLabel.unit);
					hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_VALUE + dataCellIndex);
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, tempTableLabel.Color);
					EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, tempTableLabel.Color);
					TABLE_SetText(hItem, &tempTableLabel);
				}
				else
				{
					hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_DESC + (dataCellIndex));
					TEXT_SetText(hItem, tempTableLabel.description);
					TEXT_SetTextColor(hItem, GUI_GRAY);

					hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_UNIT + (dataCellIndex));
					TEXT_SetText(hItem, tempTableLabel.unit);
					TEXT_SetTextColor(hItem, GUI_GRAY);

					hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_VALUE + dataCellIndex);
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_GRAY);
					EDIT_SetText(hItem, "-------");
				}
			}
			else
			{
				hItem = WM_GetDialogItem(hWin, ID_USER_TABLE_VALUE + dataCellIndex);
				EDIT_SetTextMode(hItem);
				EDIT_SetText(hItem, "");
				EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_TRANSPARENT);
				EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_TRANSPARENT);
			}
		}
	}
	return hWin;
}

void openSingleChannelWindow(void)
{
	BUZZER_Beep();
	WM_DeleteWindow(hActualWindow);
	hActualWindow = CreateSingleChannelWindow(tempTableLabel.channelNumber);
	WindowsState = SINGLE_VALUE;
	FRAM_Write(WINDOWS_STATE_ADDR, (uint8_t) WindowsState);
	FRAM_Write(WINDOW_INSTANCE_ADDR, (uint8_t) WindowInstance);
}

void TABLE_SetText(WM_HWIN hEdit, USER_LABEL *table)
{
	if (table->failureMode == 0)
	{
		EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
		switch (table->failureState)
		{
		case 1:
			EDIT_SetShortFloat(hEdit, table->value, table->resolution);
			EDIT_SetTextColor(hEdit, EDIT_CI_ENABLED, table->Color);
			break;
		case 0:
			EDIT_SetText(hEdit, "-----");
			break;
		case 10:
			EDIT_SetText(hEdit, "--W--");
			break;
		case 2:
			EDIT_SetTextMode(hEdit);
			EDIT_SetText(hEdit, "--||--");
			break;
		case 3:
			EDIT_SetText(hEdit, "--E--");
			break;
		case 4:
			EDIT_SetText(hEdit, "--R--");
			break;
		default:
			EDIT_SetText(hEdit, "-ERR-");
			break;
		}
	}
	else if (table->failureMode == 1)
	{
		EDIT_SetShortFloat(hEdit, table->value, table->resolution);
		if (table->failureState == 1)
		{
			EDIT_SetSkin(hEdit, EDIT_CI_ENABLED);
			EDIT_SetTextColor(hEdit, EDIT_CI_ENABLED, table->Color);
		}
		else
		{
			EDIT_SetTextColor(hEdit, EDIT_CI_ENABLED, GUI_BLACK);
			EDIT_SetBkColor(hEdit, EDIT_CI_ENABLED, GUI_YELLOW);
		}
	}
}

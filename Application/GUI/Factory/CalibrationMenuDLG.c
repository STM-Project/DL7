#include <ExpansionCards.h>
#include <UpgradeBoardFirmwareDLG.h>
#include "CalibrationMenuDLG.h"
#include "CardCalibrationDLG.h"
#include "skins.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"

#include "ExpansionCards_descriptions.h"
#include "ExpansionCardsTask.h"
#include "HartConfigurationDLG.h"

EXPANSION_CARD localExpansionCard;

#define ID_WINDOW_CALIBRATION_MENU  (GUI_ID_USER + 0x300)
#define ID_BUTTON_OK     						(GUI_ID_USER + 0x301)
#define ID_TEXT_CARD_TYPE_DESC     	(GUI_ID_USER + 0x302)
#define ID_DROPDOWN_SELECT_CARD   	(GUI_ID_USER + 0x303)
#define ID_DROPDOWN_INPUT_TYPE   		(GUI_ID_USER + 0x304)
#define ID_BUTTON_CALIBRATE_CARD    (GUI_ID_USER + 0x305)

static const GUI_WIDGET_CREATE_INFO _aCalibrationMenuCreate[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_CALIBRATION_MENU, 0, 0, 740, 420, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 2, 360, 736, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CALIBRATE_CARD, 10, 100, 300, 40, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CARD_TYPE_DESC, 140, 10, 180, 35, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SELECT_CARD, 10, 10, 120, 210, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_INPUT_TYPE, 10, 55, 240, 210, 0, 0x0, 0 }, };

static void SetCalibrationButtonsVisibility(WM_HWIN hWin, uint8_t cardType)
{
	WM_HWIN hItem;
	if(SPI_CARD_EMPTY == cardType || SPI_CARD_IN6D == cardType || SPI_CARD_OUT6RL == cardType
			|| SPI_CARD_2RS485 == cardType || SPI_CARD_2RS485I24 == cardType
			|| SPI_CARD_1HRT == cardType || SPI_CARD_PSBATT == cardType || SPI_CARD_OUT3 == cardType)
	{
		hItem = WM_GetDialogItem(hWin, ID_BUTTON_CALIBRATE_CARD);
		WM_HideWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_INPUT_TYPE);
		WM_HideWindow(hItem);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_BUTTON_CALIBRATE_CARD);
		WM_ShowWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_INPUT_TYPE);
		WM_ShowWindow(hItem);
		FillInputModeList(hItem, cardType);
		DROPDOWN_SetSel(hItem, 1);
	}
}

static void _cbCalibariotnMenuDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	uint8_t boardNumber = 0;
	uint8_t inputsMode = 0;

	uint8_t cardType = SPI_CARD_EMPTY;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_CARD);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "A");
		DROPDOWN_AddString(hItem, "B");
		DROPDOWN_AddString(hItem, "C");
		DROPDOWN_AddString(hItem, "D");
		DROPDOWN_AddString(hItem, "E");
		DROPDOWN_AddString(hItem, "F");
		DROPDOWN_AddString(hItem, "G");
		DROPDOWN_SetSel(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE_DESC);
		TEXT_SetTextColor(hItem, GUI_BLACK);
		TEXT_SetText(hItem, GetExpansionCardDescription(ExpansionCards[0].type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_INPUT_TYPE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetText(hItem, GUI_LANG_GetText(178));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALIBRATE_CARD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(177));

		cardType = ExpansionCards[0].type;

		SetCalibrationButtonsVisibility(pMsg->hWin, cardType);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_CALIBRATE_CARD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				boardNumber = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_CARD));
				inputsMode = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_INPUT_TYPE));
				GetExpansionBoard(boardNumber, &localExpansionCard);
				if (SPI_CARD_1HRT != localExpansionCard.type && SPI_CARD_PSBATT != localExpansionCard.type  && SPI_CARD_EMPTY != localExpansionCard.type)
				{
					SetBoardToCalibartionMode(boardNumber, inputsMode);
					CreateCalibrationWin(pMsg->hWin, boardNumber, inputsMode);
				}
				break;
			}
			break;
		case ID_DROPDOWN_SELECT_CARD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE_DESC);
				TEXT_SetText(hItem, GetExpansionCardDescription(ExpansionCards[DROPDOWN_GetSel(pMsg->hWinSrc)].type));

				cardType = ExpansionCards[DROPDOWN_GetSel(pMsg->hWinSrc)].type;
				SetCalibrationButtonsVisibility(pMsg->hWin, cardType);
				break;
			}
			break;
		case ID_DROPDOWN_INPUT_TYPE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				BUZZER_Beep();
				break;
			}
			break;
		}
		break;
	case WM_DELETE:
		DeactivateCalibrationMode();
		break;
	case WM_USER_LOGIN:
		if(SERVICE_LEVEL > PASSWORDS_GetCurrentLevel())
		{
			WM__DeleteSecure(pMsg->hWin);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbHartConfigMenuDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	uint8_t boardNumber = 0;
	uint8_t inputsMode = 0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_CARD);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "A");
		DROPDOWN_AddString(hItem, "B");
		DROPDOWN_AddString(hItem, "C");
		DROPDOWN_AddString(hItem, "D");
		DROPDOWN_AddString(hItem, "E");
		DROPDOWN_AddString(hItem, "F");
		DROPDOWN_AddString(hItem, "G");
		DROPDOWN_SetSel(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE_DESC);
		TEXT_SetTextColor(hItem, GUI_BLACK);
		TEXT_SetText(hItem, GetExpansionCardDescription(ExpansionCards[0].type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_INPUT_TYPE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetText(hItem, GUI_LANG_GetText(230));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALIBRATE_CARD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(235));
		WM_MoveChildTo(hItem, 10, 55);
		if(SPI_CARD_1HRT != ExpansionCards[0].type)
			WM_HideWindow(hItem);
		else
			WM_ShowWindow(hItem);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_CALIBRATE_CARD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();

				boardNumber = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_CARD));
				inputsMode = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_INPUT_TYPE));
				GetExpansionBoard(boardNumber, &localExpansionCard);
				if (localExpansionCard.type == SPI_CARD_1HRT)
				{
					HART_Init2SendCommandGetLongAddr(boardNumber, inputsMode);
					CreateHARTConfigurationWin(pMsg->hWin, boardNumber, inputsMode);
				}
				break;
			}
			break;
		case ID_DROPDOWN_SELECT_CARD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE_DESC);
				TEXT_SetText(hItem, GetExpansionCardDescription(ExpansionCards[DROPDOWN_GetSel(pMsg->hWinSrc)].type));

				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALIBRATE_CARD);
				if(SPI_CARD_1HRT != ExpansionCards[DROPDOWN_GetSel(pMsg->hWinSrc)].type)
					WM_HideWindow(hItem);
				else
					WM_ShowWindow(hItem);
				break;
			}
			break;
		}
		break;
	case WM_DELETE:
		DeactivateCalibrationMode();
		break;
	case WM_USER_LOGIN:
		if(ADMIN_LEVEL > PASSWORDS_GetCurrentLevel())
		{
			WM__DeleteSecure(pMsg->hWin);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbFirmwareUpgradeMenuDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	uint8_t boardNumber = 0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_CARD);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "A");
		DROPDOWN_AddString(hItem, "B");
		DROPDOWN_AddString(hItem, "C");
		DROPDOWN_AddString(hItem, "D");
		DROPDOWN_AddString(hItem, "E");
		DROPDOWN_AddString(hItem, "F");
		DROPDOWN_AddString(hItem, "G");
		DROPDOWN_SetSel(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE_DESC);
		TEXT_SetTextColor(hItem, GUI_BLACK);
		TEXT_SetText(hItem, GetExpansionCardDescription(ExpansionCards[0].type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_INPUT_TYPE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetText(hItem, GUI_LANG_GetText(254));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALIBRATE_CARD);
		BUTTON_SetText(hItem, GUI_LANG_GetText(255));
		WM_MoveChildTo(hItem, 10, 55);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_CALIBRATE_CARD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();

				boardNumber = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SELECT_CARD));
				GetExpansionBoard(boardNumber, &localExpansionCard);
				CreateCardBootWin(pMsg->hWin, boardNumber);

				break;
			}
			break;
		case ID_DROPDOWN_SELECT_CARD:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE_DESC);
				TEXT_SetText(hItem, GetExpansionCardDescription(ExpansionCards[DROPDOWN_GetSel(pMsg->hWinSrc)].type));
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALIBRATE_CARD);
				break;
			}
			break;
		}
		break;
	case WM_DELETE:
		DeactivateCalibrationMode();
		break;
	case WM_USER_LOGIN:
		if(ADMIN_LEVEL > PASSWORDS_GetCurrentLevel())
		{
			WM__DeleteSecure(pMsg->hWin);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateCalibarationMenu(WM_HWIN hParent)
{
	WM_HWIN hWin;
	ActivateCalibrationMode();
	hWin = GUI_CreateDialogBox(_aCalibrationMenuCreate, GUI_COUNTOF(_aCalibrationMenuCreate), _cbCalibariotnMenuDialog, hParent, 0, 0);
	WM_SetStayOnTop(hWin, 1);
	return hWin;
}

WM_HWIN CreateHartConfigMenu(WM_HWIN hParent)
{
	WM_HWIN hWin;
	ActivateCalibrationMode();
	hWin = GUI_CreateDialogBox(_aCalibrationMenuCreate, GUI_COUNTOF(_aCalibrationMenuCreate), _cbHartConfigMenuDialog, hParent, 0, 0);
	WM_SetStayOnTop(hWin, 1);
	return hWin;
}

WM_HWIN CreateFirmwareUpgradeMenu(WM_HWIN hParent)
{
	WM_HWIN hWin;
	ActivateCalibrationMode();
	hWin = GUI_CreateDialogBox(_aCalibrationMenuCreate, GUI_COUNTOF(_aCalibrationMenuCreate), _cbFirmwareUpgradeMenuDialog, hParent, 0, 0);
	WM_SetStayOnTop(hWin, 1);
	return hWin;
}

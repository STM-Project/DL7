#include <ExpansionCards.h>
#include <ExpansionCards_typedef.h>
#include "CardCalibrationDLG.h"
#include <stdlib.h>
#include <string.h>
#include "draw.h"
#include "dtos.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"

#include "ExpansionCards_descriptions.h"
#include "EDIT_double.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_CARD_CALIBRATION       	(GUI_ID_USER + 0x401)
#define ID_BUTTON_EXIT       							(GUI_ID_USER + 0x402)
#define ID_MULTIPAGE_SELECT_INPUT    			(GUI_ID_USER + 0x403)
#define ID_TEXT_BOARD_SLOT         				(GUI_ID_USER + 0x404)
#define ID_TEXT_BOARD_DESC         				(GUI_ID_USER + 0x405)
#define ID_BUTTON_COPY_CAL_COEFFS       	(GUI_ID_USER + 0x406)
#define ID_BUTTON_PASTE_CAL_COEFFS       	(GUI_ID_USER + 0x407)

#define ID_WINDOW_INPUT_CALIBRATION      	(GUI_ID_USER + 0x408)
#define ID_TEXT_LOW_DESC        					(GUI_ID_USER + 0x409)
#define ID_TEXT_HIGH_DESC        					(GUI_ID_USER + 0x40A)
#define ID_TEXT_REF_VALUES        				(GUI_ID_USER + 0x40B)
#define ID_TEXT_CAL_VALUES        				(GUI_ID_USER + 0x40C)
#define ID_EDIT_LOW_REF_VALUE        			(GUI_ID_USER + 0x40D)
#define ID_EDIT_HIGH_REF_VALUE       		 	(GUI_ID_USER + 0x40E)
#define ID_EDIT_LOW_CAL_VALUE        			(GUI_ID_USER + 0x40F)
#define ID_EDIT_HIGH_CAL_VALUE        		(GUI_ID_USER + 0x410)
#define ID_BUTTON_READ_LOW_VALUES      		(GUI_ID_USER + 0x411)
#define ID_BUTTON_READ_HIGH_VALUES      	(GUI_ID_USER + 0x412)
#define ID_BUTTON_CALCULATE_CALIBRATION 	(GUI_ID_USER + 0x413)
#define ID_TEXT_LOW_REF_UNIT        			(GUI_ID_USER + 0x414)
#define ID_TEXT_HIGH_REF_UNIT        			(GUI_ID_USER + 0x415)
#define ID_TEXT_LOW_CAL_UNIT        			(GUI_ID_USER + 0x416)
#define ID_TEXT_HIGH_CAL_UNIT        			(GUI_ID_USER + 0x417)

#define ID_EDIT_LOW_RAW_VALUE        			(GUI_ID_USER + 0x418)
#define ID_EDIT_HIGH_RAW_VALUE        		(GUI_ID_USER + 0x419)
#define ID_TEXT_LOW_RAW_UNIT        			(GUI_ID_USER + 0x41A)
#define ID_TEXT_HIGH_RAW_UNIT        			(GUI_ID_USER + 0x41B)

#define ID_TEXT_RAW_VALUES        				(GUI_ID_USER + 0x41C)

#define ID_TEXT_COEF_A        						(GUI_ID_USER + 0x41D)
#define ID_TEXT_COEF_B        						(GUI_ID_USER + 0x41E)

#define ID_EDIT_COEF_A        						(GUI_ID_USER + 0x41F)
#define ID_EDIT_COEF_B        						(GUI_ID_USER + 0x420)

#define ID_WINDOW_CALIBRATION_SUMMARY     (GUI_ID_USER + 0x421)
#define ID_BUTTON_WRITE_CALIBRATION      	(GUI_ID_USER + 0x422)
#define ID_TEXT_LAST_CALIBRATION      		(GUI_ID_USER + 0x423)

static EXPANSION_CARD localExpansionCard;

static CALIBRATION_StructTypeDef localCalibrationData;
static CALIBRATION tempCalibrationCoefs =
{ 0.0, 0.0 };

static WM_HWIN BoardCalibrationWindow = 0;
static uint8_t Calibration = 0;
static uint8_t CurrentInputNo = 0;
static uint8_t localBoardNumber = 0;
static uint8_t localInputsMode = 0;

static WM_HWIN CreateBoardCalibrationWin(void);
static WM_HWIN CreateCalibrationSummaryWin(void);

void SetFirstReferenceValue(EDIT_Handle hObj, uint8_t CardType, uint8_t InputMode)
{
	switch (CardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
		EDIT_SetText(hObj, "4.0000");
		break;
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		EDIT_SetText(hObj, "200.0000");
		break;
	case SPI_CARD_IN6V:
		EDIT_SetText(hObj, "1.0000");
		break;
	case SPI_CARD_IN6TC:
		EDIT_SetText(hObj, "10.0000");
		break;
	case SPI_CARD_IN4SG:
		EDIT_SetText(hObj, "0.50");
		break;
	case SPI_CARD_IN3:
		switch (InputMode)
		{
		case 1:
		case 2:
			EDIT_SetText(hObj, "4.0000");
			break;
		case 3:
		case 4:
			EDIT_SetText(hObj, "1.0000");
			break;
		case 5:
			EDIT_SetText(hObj, "10.0000");
			break;
		case 6:
		case 7:
		case 8:
			EDIT_SetText(hObj, "200.0000");
			break;
		default:
			EDIT_SetText(hObj, "0.0000");
		}
		break;
	default:
		EDIT_SetText(hObj, "0.0000");
	}
}

void SetSecondReferenceValue(EDIT_Handle hObj, uint8_t CardType, uint8_t InputMode)
{
	switch (CardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
		EDIT_SetText(hObj, "20.0000");
		break;
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		EDIT_SetText(hObj, "3000.0000");
		break;
	case SPI_CARD_IN6V:
		EDIT_SetText(hObj, "5.0000");
		break;
	case SPI_CARD_IN6TC:
		EDIT_SetText(hObj, "70.0000");
		break;
	case SPI_CARD_IN4SG:
		EDIT_SetText(hObj, "1.00");
		break;
	case SPI_CARD_IN3:
		switch (InputMode)
		{
		case 1:
		case 2:
			EDIT_SetText(hObj, "20.0000");
			break;
		case 3:
		case 4:
			EDIT_SetText(hObj, "5.0000");
			break;
		case 5:
			EDIT_SetText(hObj, "70.0000");
			break;
		case 6:
		case 7:
		case 8:
			EDIT_SetText(hObj, "3000.0000");
			break;
		default:
			EDIT_SetText(hObj, "0.0000");
		}
		break;
	default:
		EDIT_SetText(hObj, "0.0000");
	}
}

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
{ WINDOW_CreateIndirect, "",ID_WINDOW_CARD_CALIBRATION, 0, 0, 740, 420, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "",ID_MULTIPAGE_SELECT_INPUT, 10, 50, 720, 300, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_BOARD_SLOT, 10, 10, 120, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_BOARD_DESC, 140, 10, 310, 35, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "",ID_BUTTON_EXIT, 618, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "",ID_BUTTON_COPY_CAL_COEFFS, 496, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "",ID_BUTTON_PASTE_CAL_COEFFS, 374, 360, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aDialogBoardCalibration[] =
{
{ WINDOW_CreateIndirect, "",ID_WINDOW_INPUT_CALIBRATION, 0, 0, 720, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_LOW_DESC, 280, 10, 210, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_HIGH_DESC, 500, 10, 210, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_REF_VALUES, 5, 50, 260, 30, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_LOW_REF_VALUE, 285, 50, 150, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_LOW_REF_UNIT, 435, 50, 50, 30, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_HIGH_REF_VALUE, 505, 50, 150, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_HIGH_REF_UNIT, 655, 50, 50, 30, 0, 0x64, 0 },

{ TEXT_CreateIndirect, "",ID_TEXT_CAL_VALUES, 5, 91, 260, 30, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_LOW_CAL_VALUE, 285, 91, 150, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_LOW_CAL_UNIT, 435, 91, 50, 30, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_HIGH_CAL_VALUE, 505, 91, 150, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_HIGH_CAL_UNIT, 655, 90, 51, 30, 0, 0x64, 0 },

{ TEXT_CreateIndirect, "",ID_TEXT_RAW_VALUES, 5, 132, 260, 30, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_LOW_RAW_VALUE, 285, 132, 150, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_LOW_RAW_UNIT, 435, 132, 50, 30, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_HIGH_RAW_VALUE, 505, 132, 150, 30, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_HIGH_RAW_UNIT, 655, 132, 50, 30, 0, 0x64, 0 },

{ TEXT_CreateIndirect, "",ID_TEXT_COEF_A, 5, 170, 40, 38, 0, 0x64, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_COEF_B, 5, 210, 40, 38, 0, 0x64, 0 },

{ EDIT_CreateIndirect, "",ID_EDIT_COEF_A, 50, 170, 260, 38, 0, 0x64, 0 },
{ EDIT_CreateIndirect, "",ID_EDIT_COEF_B, 50, 210, 260, 38, 0, 0x64, 0 },



{ BUTTON_CreateIndirect, "",ID_BUTTON_READ_LOW_VALUES, 280, 170, 210, 38, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "",ID_BUTTON_READ_HIGH_VALUES, 500, 170, 210, 38, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "",ID_BUTTON_CALCULATE_CALIBRATION, 280, 210, 430, 38, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aDialogSummary[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_CALIBRATION_SUMMARY, 0, 0, 720, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "",ID_TEXT_LAST_CALIBRATION, 10, 10, 700, 30, 0, 0x64, 0 },
{ BUTTON_CreateIndirect, "",ID_BUTTON_WRITE_CALIBRATION, 0, 50, 720, 58, 0, 0x0, 0 }, };

void _cbDialogCAL(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[10];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOARD_SLOT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		mini_snprintf(GUITextBuffer, 10, "%s %c", GUI_LANG_GetText(30), 'A' + localBoardNumber);
		TEXT_SetText(hItem, GUITextBuffer);

		GetExpansionBoard(localBoardNumber, &localExpansionCard);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOARD_DESC);
		TEXT_SetText(hItem, GetExpansionCardDescription(localExpansionCard.type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SELECT_INPUT);
		if (localExpansionCard.type == SPI_CARD_IN6I24 || localExpansionCard.type == SPI_CARD_IN6I || localExpansionCard.type == SPI_CARD_IN6RTD
				|| localExpansionCard.type == SPI_CARD_IN6TC || localExpansionCard.type == SPI_CARD_IN6V
				|| localExpansionCard.type == SPI_CARD_IN3RTD || localExpansionCard.type == SPI_CARD_IN3 || localExpansionCard.type == SPI_CARD_IN4SG)
		{
			WM_ShowWindow(hItem);

			BoardCalibrationWindow = CreateBoardCalibrationWin();

			MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, "I/O 1");
			MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, "I/O 2");
			MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, "I/O 3");

			if (localExpansionCard.type == SPI_CARD_IN3RTD || localExpansionCard.type == SPI_CARD_IN3)
			{
				MULTIPAGE_AddEmptyPage(hItem, CreateCalibrationSummaryWin(), GUI_LANG_GetText(93));
				MULTIPAGE_SetTabWidth(hItem, 160, 0);
				MULTIPAGE_SetTabWidth(hItem, 160, 1);
				MULTIPAGE_SetTabWidth(hItem, 160, 2);
				MULTIPAGE_SetTabWidth(hItem, 230, 3);
			}
			else if (localExpansionCard.type == SPI_CARD_IN4SG)
			{
				MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, " I/O 4 ");
				MULTIPAGE_AddEmptyPage(hItem, CreateCalibrationSummaryWin(), GUI_LANG_GetText(93));
				MULTIPAGE_SetTabWidth(hItem, 120, 0);
				MULTIPAGE_SetTabWidth(hItem, 120, 1);
				MULTIPAGE_SetTabWidth(hItem, 120, 2);
				MULTIPAGE_SetTabWidth(hItem, 120, 3);
				MULTIPAGE_SetTabWidth(hItem, 230, 4);
			}
			else
			{
				MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, "I/O 4");
				MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, "I/O 5");
				MULTIPAGE_AddEmptyPage(hItem, BoardCalibrationWindow, "I/O 6");
				MULTIPAGE_AddEmptyPage(hItem, CreateCalibrationSummaryWin(), GUI_LANG_GetText(93));
				MULTIPAGE_SetTabWidth(hItem, 80, 0);
				MULTIPAGE_SetTabWidth(hItem, 80, 1);
				MULTIPAGE_SetTabWidth(hItem, 80, 2);
				MULTIPAGE_SetTabWidth(hItem, 80, 3);
				MULTIPAGE_SetTabWidth(hItem, 80, 4);
				MULTIPAGE_SetTabWidth(hItem, 80, 5);
				MULTIPAGE_SetTabWidth(hItem, 230, 6);
			}
			CurrentInputNo = 0;
			MULTIPAGE_SelectPage(hItem, CurrentInputNo);

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_CAL_COEFFS);
			WM_ShowWindow(hItem);
			BUTTON_SetText(hItem, GUI_LANG_GetText(99));

			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PASTE_CAL_COEFFS);
			WM_HideWindow(hItem);
			BUTTON_SetText(hItem, GUI_LANG_GetText(100));

			hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SELECT_INPUT);
			UserMessage.MsgId = WM_USER_REFRESH;
			UserMessage.hWin = BoardCalibrationWindow;
			UserMessage.hWinSrc = pMsg->hWin;
			UserMessage.Data.v = MULTIPAGE_GetSelection(hItem);
			WM_SendMessage(BoardCalibrationWindow, &UserMessage);
		}
		else
		{
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY_CAL_COEFFS);
			WM_HideWindow(hItem);
			BUTTON_SetText(hItem, GUI_LANG_GetText(99));
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PASTE_CAL_COEFFS);
			WM_HideWindow(hItem);
			BUTTON_SetText(hItem, GUI_LANG_GetText(100));
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_EXIT);
		BUTTON_SetText(hItem, "");
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);
		if (PASSWORDS_GetCurrentLevel() < 20)
			WM_HideWindow(hItem);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_EXIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_MULTIPAGE_SELECT_INPUT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SELECT_INPUT);
				CurrentInputNo = MULTIPAGE_GetSelection(hItem);

				UserMessage.MsgId = WM_USER_REFRESH;
				UserMessage.hWin = BoardCalibrationWindow;
				UserMessage.hWinSrc = pMsg->hWin;
				UserMessage.Data.v = CurrentInputNo;
				WM_SendMessage(BoardCalibrationWindow, &UserMessage);

				break;
			}
			break;
		case ID_BUTTON_COPY_CAL_COEFFS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SELECT_INPUT);
				if (MULTIPAGE_GetSelection(hItem) < 6)
				{
					BUZZER_Beep();
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PASTE_CAL_COEFFS);
					WM_ShowWindow(hItem);
					tempCalibrationCoefs.A = localCalibrationData.Coefficient[CurrentInputNo].A;
					tempCalibrationCoefs.B = localCalibrationData.Coefficient[CurrentInputNo].B;
				}
				break;
			}
			break;

		case ID_BUTTON_PASTE_CAL_COEFFS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SELECT_INPUT);
				if (MULTIPAGE_GetSelection(hItem) < 6)
				{
					BUZZER_Beep();
					localCalibrationData.Coefficient[CurrentInputNo].A = tempCalibrationCoefs.A;
					localCalibrationData.Coefficient[CurrentInputNo].B = tempCalibrationCoefs.B;

					UserMessage.MsgId = WM_USER_REFRESH;
					UserMessage.hWin = BoardCalibrationWindow;
					UserMessage.hWinSrc = pMsg->hWin;
					UserMessage.Data.v = CurrentInputNo;
					WM_SendMessage(BoardCalibrationWindow, &UserMessage);
				}
			}
			break;
		}
		break;
	case WM_DELETE:
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbDialogBoardCalibration(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[30];

	static float CalibrationPoints[2];
	static float MeasuredRawValues[2];
	static float MeasuredCalValues[2];

	switch (pMsg->MsgId)
	{
	case WM_USER_REFRESH:
		Calibration = 0;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_CAL_VALUE);
		EDIT_SetText(hItem, "");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_CAL_VALUE);
		EDIT_SetText(hItem, "");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_RAW_VALUE);
		EDIT_SetText(hItem, "");
		EDIT_SetTextColor(hItem, 1, GUI_RED);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_RAW_VALUE);
		EDIT_SetText(hItem, "");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALCULATE_CALIBRATION);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COEF_A);
		EDIT_SetDouble(hItem, localCalibrationData.Coefficient[pMsg->Data.v].A, 8);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COEF_B);
		EDIT_SetDouble(hItem, localCalibrationData.Coefficient[pMsg->Data.v].B, 8);


		break;

	case WM_POST_PAINT:
		DRAW_RoundedFrame(280, 46, 209, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(280, 87, 209, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(280, 128,209, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(500, 46, 209, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(500, 87, 209, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(500, 128,209, 38, 4, 2, GUI_BLUE);

		break;
	case WM_DELETE:
		break;
	case WM_INIT_DIALOG:
		GetExpansionBoard(localBoardNumber, &localExpansionCard);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_DESC);
		TEXT_SetText(hItem, "PK 1");
		TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_DESC);
		TEXT_SetText(hItem, "PK 2");
		TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REF_VALUES);
		TEXT_SetText(hItem, GUI_LANG_GetText(96));
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CAL_VALUES);
		TEXT_SetText(hItem, GUI_LANG_GetText(97));
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RAW_VALUES);
		TEXT_SetText(hItem, GUI_LANG_GetText(98));
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_REF_UNIT);
		SetCardUnit(hItem, localExpansionCard.type, localInputsMode);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_REF_UNIT);
		SetCardUnit(hItem, localExpansionCard.type, localInputsMode);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_CAL_UNIT);
		SetCardUnit(hItem, localExpansionCard.type, localInputsMode);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_CAL_UNIT);
		SetCardUnit(hItem, localExpansionCard.type, localInputsMode);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_RAW_UNIT);
		SetCardUnit(hItem, localExpansionCard.type, localInputsMode);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_RAW_UNIT);
		SetCardUnit(hItem, localExpansionCard.type, localInputsMode);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_REF_VALUE);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
		SetFirstReferenceValue(hItem, localExpansionCard.type, localInputsMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_REF_VALUE);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
		SetSecondReferenceValue(hItem, localExpansionCard.type, localInputsMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_CAL_VALUE);
		EDIT_SetText(hItem, "");
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_CAL_VALUE);
		EDIT_SetText(hItem, "");
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_RAW_VALUE);
		EDIT_SetText(hItem, "");
		EDIT_SetTextColor(hItem, 1, GUI_RED);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_RAW_VALUE);
		EDIT_SetText(hItem, "");
		EDIT_SetTextColor(hItem, 1, GUI_RED);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_READ_LOW_VALUES);
		BUTTON_SetText(hItem, GUI_LANG_GetText(158));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_READ_HIGH_VALUES);
		BUTTON_SetText(hItem, GUI_LANG_GetText(158));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALCULATE_CALIBRATION);
		BUTTON_SetText(hItem, GUI_LANG_GetText(92));
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COEF_A);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COEF_B);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COEF_A);
		EDIT_SetTextAlign (hItem,GUI_TA_LEFT | GUI_TA_VCENTER);
		EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);
		EDIT_SetDouble(hItem, localCalibrationData.Coefficient[0].A, 8);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COEF_B);
		EDIT_SetTextAlign (hItem,GUI_TA_LEFT | GUI_TA_VCENTER);
		EDIT_SetTextColor(hItem,EDIT_CI_ENABLED, GUI_BLUE);
		EDIT_SetDouble(hItem, localCalibrationData.Coefficient[0].B, 8);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_LOW_REF_VALUE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_REF_VALUE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_LOW_CAL_VALUE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_CAL_VALUE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_BUTTON_READ_LOW_VALUES:

			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				Calibration_ReadValues(&MeasuredCalValues[0], &MeasuredRawValues[0], CurrentInputNo, localBoardNumber);

				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_REF_VALUE), GUITextBuffer, 10);
				CalibrationPoints[0] = atof(GUITextBuffer);

				if(localExpansionCard.type == SPI_CARD_IN4SG)
				{
					dbl2stri(GUITextBuffer,MeasuredCalValues[0], 2);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_CAL_VALUE);
					EDIT_SetTextColor(hItem, 1, 0x0000c000);
					EDIT_SetText(hItem, GUITextBuffer);

					dbl2stri(GUITextBuffer,MeasuredRawValues[0], 2);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_RAW_VALUE);
					EDIT_SetText(hItem, GUITextBuffer);
				}
				else
				{
					dbl2stri(GUITextBuffer,MeasuredCalValues[0],4);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_CAL_VALUE);
					EDIT_SetTextColor(hItem, 1, 0x0000c000);
					EDIT_SetText(hItem, GUITextBuffer);

					dbl2stri(GUITextBuffer,MeasuredRawValues[0],4);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_RAW_VALUE);
					EDIT_SetText(hItem, GUITextBuffer);
				}

				Calibration = 0b1 | Calibration;
				if (Calibration == 3)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALCULATE_CALIBRATION);
					WM_ShowWindow(hItem);
				}
				break;
			}
			break;
		case ID_BUTTON_READ_HIGH_VALUES:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				Calibration_ReadValues(&MeasuredCalValues[1], &MeasuredRawValues[1], CurrentInputNo, localBoardNumber);

				EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_REF_VALUE), GUITextBuffer, 10);
				CalibrationPoints[1] = atof(GUITextBuffer);

				if(localExpansionCard.type == SPI_CARD_IN4SG)
				{
					dbl2stri(GUITextBuffer,MeasuredCalValues[1], 2);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_CAL_VALUE);
					EDIT_SetTextColor(hItem, 1, 0x0000c000);
					EDIT_SetText(hItem, GUITextBuffer);

					dbl2stri(GUITextBuffer,MeasuredRawValues[1], 2);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_RAW_VALUE);
					EDIT_SetText(hItem, GUITextBuffer);
				}
				else
				{
					dbl2stri(GUITextBuffer,MeasuredCalValues[1],4);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_CAL_VALUE);
					EDIT_SetTextColor(hItem, 1, 0x0000c000);
					EDIT_SetText(hItem, GUITextBuffer);

					dbl2stri(GUITextBuffer,MeasuredRawValues[1],4);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_RAW_VALUE);
					EDIT_SetText(hItem, GUITextBuffer);
				}

				Calibration = 0b10 | Calibration;
				if (Calibration == 3)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALCULATE_CALIBRATION);
					WM_ShowWindow(hItem);
				}
				break;
			}
			break;
		case ID_BUTTON_CALCULATE_CALIBRATION:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();

				localCalibrationData.Coefficient[CurrentInputNo].A = (CalibrationPoints[1] - CalibrationPoints[0])
						/ (MeasuredRawValues[1] - MeasuredRawValues[0]);

				localCalibrationData.Coefficient[CurrentInputNo].B = CalibrationPoints[0]
						- localCalibrationData.Coefficient[CurrentInputNo].A * MeasuredRawValues[0];


				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COEF_A);
				EDIT_SetDouble(hItem, localCalibrationData.Coefficient[CurrentInputNo].A, 8);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COEF_B);
				EDIT_SetDouble(hItem, localCalibrationData.Coefficient[CurrentInputNo].B, 8);

					Calibration = 0;
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_CAL_VALUE);
				EDIT_SetText(hItem, "");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_CAL_VALUE);
				EDIT_SetText(hItem, "");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_RAW_VALUE);
				EDIT_SetText(hItem, "");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_RAW_VALUE);
				EDIT_SetText(hItem, "");
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CALCULATE_CALIBRATION);
				WM_HideWindow(hItem);
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

static void _cbDialogSummary(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[100];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LAST_CALIBRATION);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		mini_snprintf(GUITextBuffer, 100, "%s: %02d/%02d/%02d %02d:%02d:%02d\n", GUI_LANG_GetText(160), localCalibrationData.CalYear,
				localCalibrationData.CalMonth, localCalibrationData.CalDay, localCalibrationData.CalHour, localCalibrationData.CalMin,
				localCalibrationData.CalSec);
		TEXT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_WRITE_CALIBRATION);
		BUTTON_SetText(hItem, GUI_LANG_GetText(94));

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_WRITE_CALIBRATION:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				if (PASSWORDS_GetCurrentLevel() >= SERVICE_LEVEL)
					Calibration_Write(&localCalibrationData, localBoardNumber, GetActiveCalibrationPointsMode(localBoardNumber, localInputsMode));
				break;
			}
			break;
		}
		break;
	case WM_DELETE:
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateCalibrationWin(WM_HWIN hParent, uint8_t boardNumber, uint8_t InputsMode)
{
	WM_HWIN hWin;
	localBoardNumber = boardNumber;
	localInputsMode = InputsMode;
	IOBoards_ReadCalibrationFromBoard(&localCalibrationData, localBoardNumber, GetActiveCalibrationPointsMode(boardNumber, InputsMode));

	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialogCAL, hParent, 0, 0);

	return hWin;
}

static WM_HWIN CreateBoardCalibrationWin(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogBoardCalibration, GUI_COUNTOF(_aDialogBoardCalibration), _cbDialogBoardCalibration,
	WM_HBKWIN, 0, 0);
	return hWin;
}

static WM_HWIN CreateCalibrationSummaryWin(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogSummary, GUI_COUNTOF(_aDialogSummary), _cbDialogSummary, WM_HBKWIN, 0, 0);
	return hWin;
}

/*************************** End of file ****************************/

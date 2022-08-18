/*
 * TouchPanelCalibrationDLG.c
 *
 *  Created on: 10.07.2020
 *      Author: TomaszSok
 */


#include "SetTouchPanelCalibrationDLG.h"
#include "draw.h"
#include "touchpanel.h"
#include "buzzer.h"
#include "passwords.h"
#include "UserMessage.h"
#include "rng.h"

#define ID_WINDOW_0 (GUI_ID_USER + 100)
#define ID_TEXT_0 (GUI_ID_USER + 101)

static const GUI_WIDGET_CREATE_INFO _aDialogTPCalCreate[] =
{
		{ WINDOW_CreateIndirect, "", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
		{ TEXT_CreateIndirect, "", ID_TEXT_0, 0, 0, 800, 480, 0, 0x0, 0 },
};

int calibrationState = 0;
uint16_t calibrationPointX[5]={0};
uint16_t calibrationPointY[5]={0};

uint16_t testPointX = 0;
uint16_t testPointY = 0;

WM_HMEM hErrorTimer = 0;

static void TP_CalculateLinearCoefs(float ref0,float ref1,float meas0,float meas1,float* A,float* B)
{
	*A = (ref0-ref1)/(meas0 - meas1);
	*B = ref0- *A * meas0;
}

static void TP_CalculateNewCoefs(void)
{
	float AX[5]={0};
	float BX[5]={0};
	float AY[5]={0};
	float BY[5]={0};

	TP_CalculateLinearCoefs(40.0,760.0,calibrationPointX[0],calibrationPointX[1],&AX[0],&BX[0]);
	TP_CalculateLinearCoefs(40.0,760.0,calibrationPointX[0],calibrationPointX[3],&AX[1],&BX[1]);
	TP_CalculateLinearCoefs(40.0,760.0,calibrationPointX[2],calibrationPointX[1],&AX[2],&BX[2]);
	TP_CalculateLinearCoefs(40.0,760.0,calibrationPointX[2],calibrationPointX[3],&AX[3],&BX[3]);
	AX[4] = (AX[0]+AX[1]+AX[2]+AX[3])/4.0;
	BX[4] = (BX[0]+BX[1]+BX[2]+BX[3])/4.0;

	TP_CalculateLinearCoefs(25.0,455.0,calibrationPointY[0],calibrationPointY[2],&AY[0],&BY[0]);
	TP_CalculateLinearCoefs(25.0,455.0,calibrationPointY[0],calibrationPointY[3],&AY[1],&BY[1]);
	TP_CalculateLinearCoefs(25.0,455.0,calibrationPointY[1],calibrationPointY[2],&AY[2],&BY[2]);
	TP_CalculateLinearCoefs(25.0,455.0,calibrationPointY[1],calibrationPointY[3],&AY[3],&BY[3]);
	AY[4] = (AY[0]+AY[1]+AY[2]+AY[3])/4.0;
	BY[4] = (BY[0]+BY[1]+BY[2]+BY[3])/4.0;

	TP_CALIBARION_UpdateCoefs(AX[4],BX[4],AY[4],BY[4]);
}

static void _cbDialogTPCal(WM_MESSAGE * pMsg)
{
	int NCode;
	int Id;
	GUI_PID_STATE GUI_TP_state;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		for (int i = 0; i < 5; ++i)
		{
			calibrationPointX[i]=0;
			calibrationPointY[i]=0;
		}
		WINDOW_SetBkColor(pMsg->hWin,GUI_BLACK);
		calibrationState = 0;
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;

		switch (Id)
		{
		case ID_TEXT_0:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				TP_CALIBARION_GetRawValues(&calibrationPointX[calibrationState],&calibrationPointY[calibrationState]);

				++calibrationState;
				if(4 == calibrationState)
				{
					TP_CalculateNewCoefs();
					WM_InvalidateWindow(pMsg->hWin);
				}
				else if(5 <= calibrationState)
				{
					GUI_TOUCH_GetState(&GUI_TP_state);
					if((testPointX <= GUI_TP_state.x && (testPointX+20) >= GUI_TP_state.x )
							&& (testPointY <= GUI_TP_state.y && (testPointY+20) >= GUI_TP_state.y ))
					{
						TP_CALIBRATION_WriteCoefToFRAM();
						WM_DeleteWindow(pMsg->hWin);
					}
					else
					{
						TP_CALIBRATION_ReadCoefFromFRAM();
						calibrationState = 0;
						WM_InvalidateWindow(pMsg->hWin);
					}
				}
				else
					WM_InvalidateWindow(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		switch (calibrationState)
		{
		case 0:
			DRAW_Rect(30, 15, 20, 20, 1, GUI_ORANGE, GUI_ORANGE);
			break;
		case 1:
			DRAW_Rect(750, 15, 20, 20, 1, GUI_ORANGE, GUI_ORANGE);
			break;
		case 2:
			DRAW_Rect(30, 445, 20, 20, 1, GUI_ORANGE, GUI_ORANGE);
			break;
		case 3:
			DRAW_Rect(750, 445, 20, 20, 1, GUI_ORANGE, GUI_ORANGE);
			break;
		case 4:
			testPointX = MX_RNG_GenerateNumber(750) + 20;
			testPointY = MX_RNG_GenerateNumber(430) + 20;
			DRAW_Rect(testPointX, testPointY, 20, 20, 1, GUI_WHITE, GUI_WHITE);
			break;
		}
		break;
	case WM_USER_LOGIN:
		if (ADMIN_LEVEL > pMsg->Data.v)
			WM__DeleteSecure(pMsg->hWin);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateCalibrateTouchPanel(void)
{
	TP_CALIBRATION_SetDefaultsCoef();
	hErrorTimer = 0;
	return GUI_CreateDialogBox(_aDialogTPCalCreate, GUI_COUNTOF(_aDialogTPCalCreate), _cbDialogTPCal, WM_HBKWIN, 0, 0);
}

/*
 * SetInputsDLG.c
 *
 *  Created on: 17 lut 2015
 *      Author: Tomaszs
 */
#include "parameters.h"
#include "channels.h"
#include "ExpansionCards.h"
#include "SetChannelDLG.h"
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include "SetUserCharDLG.h"
#include "UserChar.h"
#include "dtos.h"
#include "draw.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "passwords.h"
#include "ExpansionCards_descriptions.h"
#include "mini-printf.h"
#include "EDIT_double.h"
#include "ComputeChannel.h"
#include "tcpipclient_netconn.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

USER_CHAR tempUserCharacteristics[10] __attribute__ ((section(".sdram")));

#define ID_WINDOW_SET_CHANNEL 			(GUI_ID_USER + 0x200)
#define ID_MULTIPAGE_SET_CHANNEL 		(GUI_ID_USER + 0x201)
#define ID_BUTTON_OK 								(GUI_ID_USER + 0x202)
#define ID_BUTTON_CANCEL 						(GUI_ID_USER + 0x203)
#define ID_DROPDOWN_CHANNEL_SEL			(GUI_ID_USER + 0x204)
#define ID_TEXT_CHANNEL_DESC    		(GUI_ID_USER + 0x205)
#define ID_EDIT_CHANNEL_DESC    		(GUI_ID_USER + 0x206)
#define ID_BUTTON_COPY 							(GUI_ID_USER + 0x207)
#define ID_BUTTON_PASTE 						(GUI_ID_USER + 0x208)
#define ID_WINDOW_CHANNEL_SOURCE		(GUI_ID_USER + 0x209)
#define ID_TEXT_CHANNEL_TYPE_DESC 	(GUI_ID_USER + 0x20A)
#define ID_CHANNEL_TYPE_DROPDOWN		(GUI_ID_USER + 0x20B)
#define ID_WINDOW_SOURCE 						(GUI_ID_USER + 0x20C)
#define ID_TEXT_SOURCE_DESC 				(GUI_ID_USER + 0x20D)
#define ID_TEXT_CHARACTERISTIC 			(GUI_ID_USER + 0x20E)
#define ID_TEXT_LOW_INPUT_UNIT 			(GUI_ID_USER + 0x20F)
#define ID_SLOT_NUMBER_DROPDOWN   	(GUI_ID_USER + 0x210)
#define ID_INPUT_NUMBER_DROPDOWN   	(GUI_ID_USER + 0x211)
#define ID_CHARACTERISTIC_DROPDOWN  (GUI_ID_USER + 0x212)
#define ID_EDIT_LOW_SCALE       		(GUI_ID_USER + 0x213)
#define ID_EDIT_HIGH_SCALE       		(GUI_ID_USER + 0x214)
#define ID_TEXT_HIGH_INPUT_UNIT     (GUI_ID_USER + 0x215)
#define ID_TEXT_INPUT_BOARD_DESC    (GUI_ID_USER + 0x216)
#define ID_EDIT_LOW_SIGNAL       		(GUI_ID_USER + 0x217)
#define ID_EDIT_HIGH_SIGNAL       	(GUI_ID_USER + 0x218)
#define ID_EDIT_FORMULA       			(GUI_ID_USER + 0x219)

#define ID_WINDOW_CHANNEL_GENERAL 	(GUI_ID_USER + 0x21A)
#define ID_TEXT_UNIT 								(GUI_ID_USER + 0x21B)
#define ID_TEXT_ARCH_STATE 					(GUI_ID_USER + 0x21C)
#define ID_TEXT_TIMEBASE 						(GUI_ID_USER + 0x21D)
#define ID_DROPDOWN_TIMEBASE 				(GUI_ID_USER + 0x24D)
#define ID_TEXT_GRAPH_MIN 					(GUI_ID_USER + 0x21E)
#define ID_TEXT_RES 								(GUI_ID_USER + 0x21F)
#define ID_CHECKBOX_ARCH_STATE 			(GUI_ID_USER + 0x220)
#define ID_EDIT_UNIT 								(GUI_ID_USER + 0x221)
#define ID_EDIT_GRAPH_MAX 					(GUI_ID_USER + 0x222)
#define ID_EDIT_GRAPH_MIN 					(GUI_ID_USER + 0x223)
#define ID_DROPDOWN_RES    					(GUI_ID_USER + 0x224)
#define ID_TEXT_GRAPH_MAX 					(GUI_ID_USER + 0x225)
#define ID_DROPDOWN_FAILURE_MODE		(GUI_ID_USER + 0x226)
#define ID_TEXT_FAILURE_MODE 				(GUI_ID_USER + 0x227)
#define ID_DROPDOWN_FILTER    			(GUI_ID_USER + 0x228)
#define ID_EDIT_FAILURE_VALUE 			(GUI_ID_USER + 0x229)
#define ID_TEXT_FILTER 							(GUI_ID_USER + 0x22A)

#define ID_WINDOW_SET_TOTALIZER 						(GUI_ID_USER + 0x22B)
#define ID_TEXT_TOTALIZER_MULTIPLER 				(GUI_ID_USER + 0x22C)
#define ID_TEXT_TOTALIZER_TYPE 							(GUI_ID_USER + 0x22E)
#define ID_TEXT_TOTALIZER_UNIT 							(GUI_ID_USER + 0x22F)
#define ID_TEXT_TOTALIZER_RES 							(GUI_ID_USER + 0x230)
#define ID_TEXT_TOTALIZER_ARCH_STATE 				(GUI_ID_USER + 0x231)
#define ID_DROPDOWN_TOTALIZER_TYPE  				(GUI_ID_USER + 0x232)
#define ID_DROPDOWN_TOTALIZER_MULTIPLER			(GUI_ID_USER + 0x233)
#define ID_EDIT_TOTALIZER_UNIT 							(GUI_ID_USER + 0x234)
#define ID_DROPDOWN_TOTALIZER_RES  					(GUI_ID_USER + 0x235)
#define ID_CHECKBOX_TOTALIZER_ARCH_STATE		(GUI_ID_USER + 0x237)
#define ID_TEXT_TOTALIZER_EMAIL_REPORT 			(GUI_ID_USER + 0x24F)
#define ID_CHECKBOX_TOTALIZER_EMAIL_REPORT 	(GUI_ID_USER + 0x250)

#define ID_WINDOW_SET_ALARM 							(GUI_ID_USER + 0x238)
#define ID_TEXT_ALARM_TYPE 								(GUI_ID_USER + 0x239)
#define ID_TEXT_ALARM_MODE 								(GUI_ID_USER + 0x23A)
#define ID_TEXT_ALARM_LEVEL 							(GUI_ID_USER + 0x23B)
#define ID_TEXT_ALARM_HISTER 							(GUI_ID_USER + 0x23C)
#define ID_TEXT_ALARM_COLOR 							(GUI_ID_USER + 0x23D)
#define ID_TEXT_ALARM_OUTPUT_DESC 				(GUI_ID_USER + 0x23E)
#define ID_TEXT_ALARM_LOG 								(GUI_ID_USER + 0x23F)
#define ID_TEXT_ALARM_FREQ 								(GUI_ID_USER + 0x240)
#define ID_DROPDOWN_ALARM_TYPE 						(GUI_ID_USER + 0x241)
#define ID_DROPDOWN_ALARM_MODE 						(GUI_ID_USER + 0x242)
#define ID_DROPDOWN_ALARM_COLOR 					(GUI_ID_USER + 0x243)
#define ID_DROPDOWN_ALARM_CHANNEL 				(GUI_ID_USER + 0x244)
#define ID_DROPDOWN_ALARM_BOARD 					(GUI_ID_USER + 0x245)
#define ID_EDIT_ALARM_LEVEL 							(GUI_ID_USER + 0x246)
#define ID_EDIT_ALARM_HISTER 							(GUI_ID_USER + 0x247)
#define ID_CHECKBOX_ALARM_LOG 						(GUI_ID_USER + 0x248)
#define ID_CHECKBOX_ALARM_FREQ 						(GUI_ID_USER + 0x249)
#define ID_TEXT_ALARM_OUTPUT_BOARD_DESC		(GUI_ID_USER + 0x24A)
#define ID_TEXT_ALARM_EMAIL								(GUI_ID_USER + 0x251)
#define ID_CHECKBOX_ALARM_EMAIL						(GUI_ID_USER + 0x252)

#define ID_DROPDOWN_USER									(GUI_ID_USER + 0x24B)
#define ID_BUTTON_VIEW										(GUI_ID_USER + 0x24C)
#define ID_TEXT_DOTS											(GUI_ID_USER + 0x24E)

#define OTHER 	0
#define RTD			1
#define TC			2
#define STATE		3
#define COUNTER	4

#define MEASUREMENT 0
#define USER_CHAR 1

static WM_HWIN CreateChannelGeneral(WM_HWIN hParent);
static WM_HWIN CreateChannelInputs(WM_HWIN hParent);
static WM_HWIN CreateChannelInputsMeasurment(WM_HWIN hParent);
static WM_HWIN CreateChannelInputsComputed(WM_HWIN hParent);
static WM_HWIN CreateChannelModbus(WM_HWIN hParent);
static WM_HWIN CreateChannelInputsDemo(WM_HWIN hParent);
static WM_HWIN CreateChannelTotalizers(WM_HWIN hParent);
static WM_HWIN CreateChannelAlarms(WM_HWIN hParent);

void FillIOBoardDropdownList(DROPDOWN_Handle hObj, uint8_t boardType);
void FillCharacteristicDropdownList(DROPDOWN_Handle hObj, uint8_t channelType, uint8_t CharacteristicType);
void SelectCharacteristicList(DROPDOWN_Handle hItem, uint8_t Board, uint8_t Input, uint8_t CharacteristicType);
static int CheckChannelSettings(CHANNEL *Dst, CHANNEL *Src);

static void DrawHighScaleDecorations(void);
static void DrawLowScaleDecorations(void);

static void HideLowScaleElements(WM_HWIN hWin);
static void HideHighScaleElements(WM_HWIN hWin);
static void ShowLowScaleElements(WM_HWIN hWin);
static void ShowHighScaleElements(WM_HWIN hWin);

static void ShowUserCharElements(WM_HWIN hWin);
static void HideUserCharElements(WM_HWIN hWin);

static void setCharacteristicsWidgets(WM_HWIN hWin, uint8_t SelectedBoard, uint8_t SelectedInput);

static void MultipageMessage(WM_HWIN hWinSrc, int msgId);

static void HideDots (WM_HWIN hWin);
static void ShowDots (WM_HWIN hWin);
static void CheckFormulaResolution (WM_HWIN hFormula);

static WM_HWIN ChannelType;
static WM_HWIN Alarm1Win, Alarm2Win;
static WM_HWIN Tot1Win, Tot2Win;
static int Selection;
static CHANNEL tempChannels[100] __attribute__ ((section(".sdram")));
static CHANNEL copyChannelBuffer __attribute__ ((section(".sdram")));
static char FormulaTextBuffer[FORMULA_RPN_SIZE];

static const GUI_WIDGET_CREATE_INFO _aSetChannelWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_CHANNEL, 0, 0, 740, 420, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_SET_CHANNEL, 5, 58, 730, 295, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_CHANNEL_SEL, 10, 10, 195, 280, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CHANNEL_DESC, 215, 10, 145, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_CHANNEL_DESC, 375, 10, 345, 35, GUI_TA_HCENTER | EDIT_CF_VCENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 496, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 618, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_COPY, 374, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_PASTE, 252, 360, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetChannelInputsWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_CHANNEL_SOURCE, 0, 0, 730, 260, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CHANNEL_TYPE_DESC, 10, 10, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_CHANNEL_TYPE_DROPDOWN, 250, 10, 200, 100, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetChannelInputsMeasurmentWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SOURCE, 0, 0, 730, 205, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_SOURCE_DESC, 10, 2, 220, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_SLOT_NUMBER_DROPDOWN, 250, 2, 80, 90, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_INPUT_NUMBER_DROPDOWN, 345, 2, 105, 90, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_INPUT_BOARD_DESC, 470, 2, 220, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_CHARACTERISTIC, 10, 50, 220, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_CHARACTERISTIC_DROPDOWN, 250, 50, 200, 120, 0, 0x0, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SIGNAL, 120, 95, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LOW_INPUT_UNIT, 245, 95, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SCALE, 375, 95, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SIGNAL, 120, 140, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_HIGH_INPUT_UNIT, 245, 140, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SCALE, 375, 140, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_USER, 220, 95, 260, 75, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_VIEW, 220, 140, 260, 45, 0, 0x0, 0 },};

static const GUI_WIDGET_CREATE_INFO _aSetChannelInputsComputedWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SOURCE, 0, 0, 730, 205, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SOURCE_DESC, 10, 2, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_FORMULA, 253, 2, 194, 35, EDIT_CF_LEFT | EDIT_CF_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_CHARACTERISTIC, 10, 50, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_CHARACTERISTIC_DROPDOWN, 250, 50, 200, 120, 0, 0x0, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SIGNAL, 120, 95, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LOW_INPUT_UNIT, 245, 95, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SCALE, 375, 95, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SIGNAL, 120, 140, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_HIGH_INPUT_UNIT, 245, 140, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SCALE, 375, 140, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_USER, 220, 95, 260, 75, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_VIEW, 220, 140, 260, 45, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "(...)", ID_TEXT_DOTS, 412, 2, 35, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aSetChannelModbusWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SOURCE, 0, 0, 730, 205, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_SOURCE_DESC, 10, 2, 220, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_INPUT_NUMBER_DROPDOWN, 250, 2, 400, 120, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_CHARACTERISTIC, 10, 50, 220, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_CHARACTERISTIC_DROPDOWN, 250, 50, 200, 120, 0, 0x0, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SIGNAL, 120, 95, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LOW_INPUT_UNIT, 245, 95, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SCALE, 375, 95, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SIGNAL, 120, 140, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_HIGH_INPUT_UNIT, 245, 140, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SCALE, 375, 140, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_USER, 220, 95, 260, 75, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_VIEW, 220, 140, 260, 45, 0, 0x0, 0 },};

static const GUI_WIDGET_CREATE_INFO _aSetChannelInputsDemoWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SOURCE, 0, 0, 730, 205, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SOURCE_DESC, 10, 2, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_FORMULA, 250, 2, 450, 35, EDIT_CF_LEFT | EDIT_CF_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_CHARACTERISTIC, 10, 50, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_CHARACTERISTIC_DROPDOWN, 250, 50, 200, 120, 0, 0x0, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SIGNAL, 120, 95, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LOW_INPUT_UNIT, 245, 95, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LOW_SCALE, 375, 95, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SIGNAL, 120, 140, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_HIGH_INPUT_UNIT, 245, 140, 80, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_HIGH_SCALE, 375, 140, 205, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_USER, 220, 95, 260, 75, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_VIEW, 220, 140, 260, 45, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetChannelGeneralWindow[] =
{

{ WINDOW_CreateIndirect, "", ID_WINDOW_CHANNEL_GENERAL, 0, 0, 730, 260, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_UNIT, 10, 10, 180, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_UNIT, 215, 10, 160, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_RES, 400, 10, 160, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RES, 570, 10, 150, 160, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TIMEBASE, 10, 55, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_TIMEBASE, 210, 55, 170, 120, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ARCH_STATE, 400, 55, 160, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_ARCH_STATE, 570, 55, 35, 35, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_FILTER, 10, 100, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_FILTER, 210, 100, 170, 120, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_FAILURE_MODE, 10, 145, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_FAILURE_MODE, 210, 145, 170, 60, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_FAILURE_VALUE, 435, 145, 165, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_GRAPH_MIN, 10, 190, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_GRAPH_MIN, 215, 190, 140, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_GRAPH_MAX, 370, 190, 190, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_GRAPH_MAX, 575, 190, 140, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetChannelTotlizersWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_TOTALIZER, 0, 0, 730, 260, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TOTALIZER_TYPE, 10, 10, 160, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_TOTALIZER_TYPE, 180, 10, 200, 200, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TOTALIZER_UNIT, 10, 55, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_TOTALIZER_UNIT, 185, 55, 190, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TOTALIZER_MULTIPLER, 10, 100, 160, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_TOTALIZER_MULTIPLER, 180, 100, 200, 120, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TOTALIZER_RES, 10, 145, 240, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_TOTALIZER_RES, 180, 145, 200, 120, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TOTALIZER_ARCH_STATE, 10, 190, 160, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_TOTALIZER_ARCH_STATE, 180, 190, 35, 35, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_TOTALIZER_EMAIL_REPORT, 390, 190, 250, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_TOTALIZER_EMAIL_REPORT, 650, 190, 35, 35, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aSetChannelAlarmsWindow[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_SET_ALARM, 0, 0, 730, 260, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_TYPE, 10, 10, 120, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ALARM_TYPE, 130, 10, 200, 90, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_MODE, 390, 10, 120, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ALARM_MODE, 520, 10, 200, 90, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_LEVEL, 10, 55, 120, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_ALARM_LEVEL, 135, 55, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_HISTER, 390, 55, 120, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_ALARM_HISTER, 525, 55, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_COLOR, 10, 100, 120, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ALARM_COLOR, 130, 100, 200, 120, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_OUTPUT_DESC, 390, 100, 120, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ALARM_CHANNEL, 520, 100, 95, 90, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ALARM_BOARD, 625, 100, 95, 90, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_OUTPUT_BOARD_DESC, 560, 135, 160, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_LOG, 10, 190, 280, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_ALARM_LOG, 295, 190, 35, 35, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_EMAIL, 390, 190, 250, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_ALARM_EMAIL, 650, 190, 35, 35, 0, 0x0, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ALARM_FREQ, 10, 145, 420, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ CHECKBOX_CreateIndirect, "", ID_CHECKBOX_ALARM_FREQ, 435, 145, 35, 35, 0, 0x0, 0 }, };

static void _cbSetChannelWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int i;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		CopyUserCharSettings(tempUserCharacteristics, bkUserCharacteristics);
		for (i = 0; i < 100; i++)
			tempChannels[i] = bkChannels[i];

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_CHANNEL_SEL);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for (i = 1; i <= 100; i++)
		{
			mini_snprintf(GUITextBuffer, 30, "%s %d", GUI_LANG_GetText(6), i);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		Selection = 0;
		DROPDOWN_SetSel(hItem, Selection);

		Alarm1Win = 0;
		Alarm2Win = 0;
		Tot1Win = 0;
		Tot2Win = 0;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_SET_CHANNEL);
		MULTIPAGE_EnableScrollbar(hItem, 0);
		MULTIPAGE_AddPage(hItem, CreateChannelInputs(pMsg->hWin), GUI_LANG_GetText(8));
		MULTIPAGE_AddPage(hItem, CreateChannelGeneral(pMsg->hWin), GUI_LANG_GetText(10));
		CreateChannelAlarms(pMsg->hWin);
		CreateChannelAlarms(pMsg->hWin);
		mini_snprintf(GUITextBuffer, 15, "%s 1", GUI_LANG_GetText(104));
		MULTIPAGE_AddPage(hItem, Alarm1Win, GUITextBuffer);
		mini_snprintf(GUITextBuffer, 15, "%s 2", GUI_LANG_GetText(104));
		MULTIPAGE_AddPage(hItem, Alarm2Win, GUITextBuffer);

		CreateChannelTotalizers(pMsg->hWin);
		CreateChannelTotalizers(pMsg->hWin);
		MULTIPAGE_AddPage(hItem, Tot1Win, "∑1");
		MULTIPAGE_AddPage(hItem, Tot2Win, "∑2");

		MULTIPAGE_SetTabWidth(hItem, 120, 0);
		MULTIPAGE_SetTabWidth(hItem, 125, 1);
		MULTIPAGE_SetTabWidth(hItem, 130, 2);
		MULTIPAGE_SetTabWidth(hItem, 130, 3);
		MULTIPAGE_SetTabWidth(hItem, 110, 4);
		MULTIPAGE_SetTabWidth(hItem, 110, 5);
		MULTIPAGE_SelectPage(hItem, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(23));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_CHANNEL_DESC);
		EDIT_SetMaxLen(hItem, 25);
		EDIT_SetText(hItem, tempChannels[Selection].description);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY);
		BUTTON_SetText(hItem, GUI_LANG_GetText(99));
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PASTE);
		BUTTON_SetText(hItem, GUI_LANG_GetText(100));
		WM_HideWindow(hItem);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_CHANNEL_DESC:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 25);
				break;
			}
			break;
		case ID_DROPDOWN_CHANNEL_SEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_LOST_FOCUS:
				DROPDOWN_Collapse(pMsg->hWinSrc);
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_CHANNEL_DESC);
				EDIT_GetText(hItem, tempChannels[Selection].description, 51);

				MultipageMessage(pMsg->hWin, WM_USER_SAVE_TEMP);

				Selection = DROPDOWN_GetSel(pMsg->hWinSrc);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_CHANNEL_DESC);
				EDIT_SetText(hItem, tempChannels[Selection].description);

				MultipageMessage(pMsg->hWin, WM_USER_REFRESH);

				break;
			}
			break;
		case ID_MULTIPAGE_SET_CHANNEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;

		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_CHANNEL_DESC);
				EDIT_GetText(hItem, tempChannels[Selection].description, 51);

				MultipageMessage(pMsg->hWin, WM_USER_SAVE_TEMP);

				for (int j = 0; j < 100; j++)
				{
					if (CheckChannelSettings(&bkChannels[j], &tempChannels[j]))
					{
						NewSettings = 1;
						break;
					}
				}

				for (int j = 0; j < 100; j++)
					bkChannels[j] = tempChannels[j];

				if (NewUserChar)
				{
					RestartDevice = 1;
					NewArchive = 1;
				}

				CopyUserCharSettings(bkUserCharacteristics, tempUserCharacteristics);
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;

		case ID_BUTTON_CANCEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_HWIN handler;
				handler = WM_GetNextSibling(pMsg->hWin);
				if (handler)
					WM_DeleteWindow(handler);
				else
					WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_COPY:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_CHANNEL_DESC);
				EDIT_GetText(hItem, tempChannels[Selection].description, 51);

				MultipageMessage(pMsg->hWin, WM_USER_SAVE_TEMP);

				copyChannelBuffer = tempChannels[Selection];
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PASTE);
				WM_ShowWindow(hItem);
				break;
			}
			break;

		case ID_BUTTON_PASTE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				tempChannels[Selection] = copyChannelBuffer;
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_CHANNEL_DESC);
				EDIT_SetText(hItem, tempChannels[Selection].description);

				MultipageMessage(pMsg->hWin, WM_USER_REFRESH);
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(370, 7, 360, 40, 4, 2, GUI_BLUE);
		break;
	case WM_USER_LOGIN:
		if (PASSWORDS_GetCurrentLevel() < USER_LEVEL)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_COPY);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_PASTE);
			WM_HideWindow(hItem);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelInputsWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int Sel;
	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHANNEL_TYPE_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(26));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHANNEL_TYPE_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(28));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(29));
		DROPDOWN_AddString(hItem, "Modbus TCP");
		DROPDOWN_AddString(hItem, "Demo (TEST)");
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetSel(hItem, tempChannels[Selection].source.type);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_CHANNEL_TYPE_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				Sel = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, Id));
				switch (Sel)
				{
				case 0:
					WM_DeleteWindow(ChannelType);
					break;
				case 1:
					WM_DeleteWindow(ChannelType);
					ChannelType = CreateChannelInputsMeasurment(pMsg->hWin);
					break;
				case 2:
					WM_DeleteWindow(ChannelType);
					ChannelType = CreateChannelInputsComputed(pMsg->hWin);
					break;
				case 3:
					WM_DeleteWindow(ChannelType);
					ChannelType = CreateChannelModbus(pMsg->hWin);
					break;
				case 4:
					WM_DeleteWindow(ChannelType);
					ChannelType = CreateChannelInputsDemo(pMsg->hWin);
					break;
				}
				break;
			}
			break;
		}
		break;
	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHANNEL_TYPE_DROPDOWN);
		tempChannels[pMsg->Data.v].source.type = DROPDOWN_GetSel(hItem);

		if (tempChannels[pMsg->Data.v].source.type == 1)
		{
			hItem = WM_GetDialogItem(ChannelType, ID_SLOT_NUMBER_DROPDOWN);
			tempChannels[pMsg->Data.v].source.board = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_INPUT_NUMBER_DROPDOWN);
			tempChannels[pMsg->Data.v].source.number = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_CHARACTERISTIC_DROPDOWN);
			tempChannels[pMsg->Data.v].CharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_DROPDOWN_USER);
			tempChannels[pMsg->Data.v].UserCharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowSignalValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighSignalValue = atof(GUITextBuffer);
		}
		else if (tempChannels[pMsg->Data.v].source.type == 2)
		{
			hItem = WM_GetDialogItem(ChannelType, ID_CHARACTERISTIC_DROPDOWN);
			tempChannels[pMsg->Data.v].CharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_DROPDOWN_USER);
			tempChannels[pMsg->Data.v].UserCharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowSignalValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighSignalValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_FORMULA);
			EDIT_GetText(hItem, FormulaTextBuffer, FORMULA_RPN_SIZE);
			strcpy(tempChannels[pMsg->Data.v].formula.userFormula, FormulaTextBuffer);
		}
		else if (tempChannels[pMsg->Data.v].source.type == 3)
		{
			hItem = WM_GetDialogItem(ChannelType, ID_INPUT_NUMBER_DROPDOWN);
			tempChannels[pMsg->Data.v].source.number = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_CHARACTERISTIC_DROPDOWN);
			tempChannels[pMsg->Data.v].CharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_DROPDOWN_USER);
			tempChannels[pMsg->Data.v].UserCharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowSignalValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighSignalValue = atof(GUITextBuffer);
		}
		else if (tempChannels[pMsg->Data.v].source.type == 4)
		{
			hItem = WM_GetDialogItem(ChannelType, ID_CHARACTERISTIC_DROPDOWN);
			tempChannels[pMsg->Data.v].CharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_DROPDOWN_USER);
			tempChannels[pMsg->Data.v].UserCharacteristicType = DROPDOWN_GetSel(hItem);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SCALE);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighScaleValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_LOW_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].LowSignalValue = atof(GUITextBuffer);

			hItem = WM_GetDialogItem(ChannelType, ID_EDIT_HIGH_SIGNAL);
			EDIT_GetText(hItem, GUITextBuffer, 30);
			tempChannels[pMsg->Data.v].HighSignalValue = atof(GUITextBuffer);
		}
		else
		{
			tempChannels[pMsg->Data.v].source.board = 0;
			tempChannels[pMsg->Data.v].source.number = 0;
		}

		break;

	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHANNEL_TYPE_DROPDOWN);
		Sel = DROPDOWN_GetSel(hItem);

		if (Sel == tempChannels[pMsg->Data.v].source.type)
		{
			switch (Sel)
			{
			case 0:
				WM_DeleteWindow(ChannelType);
				break;
			case 1:
				WM_DeleteWindow(ChannelType);
				ChannelType = CreateChannelInputsMeasurment(pMsg->hWin);
				break;
			case 2:
				WM_DeleteWindow(ChannelType);
				ChannelType = CreateChannelInputsComputed(pMsg->hWin);
				break;
			case 3:
				WM_DeleteWindow(ChannelType);
				ChannelType = CreateChannelModbus(pMsg->hWin);
				break;
			case 4:
				WM_DeleteWindow(ChannelType);
				ChannelType = CreateChannelInputsDemo(pMsg->hWin);
				break;
			}
		}
		else
		{
			DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].source.type);
			Sel = DROPDOWN_GetSel(hItem);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelGeneralWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[20];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UNIT);
		TEXT_SetText(hItem, GUI_LANG_GetText(33));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_UNIT);
		EDIT_SetText(hItem, tempChannels[0].unit);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ARCH_STATE);
		TEXT_SetText(hItem, GUI_LANG_GetText(37));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ARCH_STATE);
		CHECKBOX_SetText(hItem, "");
		CHECKBOX_SetTextColor(hItem, 0x0000c000);
		CHECKBOX_SetState(hItem, tempChannels[0].ToArchive);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TIMEBASE);
		TEXT_SetText(hItem, GUI_LANG_GetText(114));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TIMEBASE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, " ");
		DROPDOWN_AddString(hItem, "/s");
		DROPDOWN_AddString(hItem, "/min");
		DROPDOWN_AddString(hItem, "/h");
		DROPDOWN_SetSel(hItem, tempChannels[0].Tot1.period);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MAX);
		EDIT_SetFloat(hItem, tempChannels[0].graph_max_value, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_GRAPH_MIN);
		TEXT_SetText(hItem, GUI_LANG_GetText(195));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_GRAPH_MAX);
		TEXT_SetText(hItem, GUI_LANG_GetText(36));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MIN);
		EDIT_SetFloat(hItem, tempChannels[0].graph_min_value, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RES);
		TEXT_SetText(hItem, GUI_LANG_GetText(34));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FAILURE_MODE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(197));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(110));
		DROPDOWN_SetSel(hItem, tempChannels[0].failureMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FAILURE_VALUE);
		EDIT_SetFloat(hItem, tempChannels[0].failureValue, 4);
		if (tempChannels[0].failureMode == 0)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FAILURE_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(112));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILTER);
		TEXT_SetText(hItem, GUI_LANG_GetText(111));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FILTER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, "2 s");
		DROPDOWN_AddString(hItem, "5 s");
		DROPDOWN_AddString(hItem, "10 s");
		DROPDOWN_AddString(hItem, "20 s");
		DROPDOWN_AddString(hItem, "30 s");
		DROPDOWN_AddString(hItem, "1 min");
		DROPDOWN_AddString(hItem, "2 min");
		DROPDOWN_AddString(hItem, "3 min");
		DROPDOWN_AddString(hItem, "5 min");
		DROPDOWN_SetSel(hItem, tempChannels[0].filterType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RES);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "0");
		DROPDOWN_AddString(hItem, "0.0");
		DROPDOWN_AddString(hItem, "0.00");
		DROPDOWN_AddString(hItem, "0.000");
		DROPDOWN_AddString(hItem, "0.0000");
		DROPDOWN_SetSel(hItem, tempChannels[0].resolution);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_RES:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_DROPDOWN_TIMEBASE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_CHECKBOX_ARCH_STATE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_EDIT_UNIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 10);
				break;
			}
			break;

		case ID_EDIT_GRAPH_MAX:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;

		case ID_EDIT_GRAPH_MIN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_DROPDOWN_FAILURE_MODE:
			switch (NCode)
			{
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);
				if (DROPDOWN_GetSel(pMsg->hWinSrc))
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FAILURE_VALUE);
					WM_ShowWindow(hItem);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FAILURE_VALUE);
					WM_HideWindow(hItem);
				}
				break;
			}
			break;
		case ID_EDIT_FAILURE_VALUE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:

		DRAW_RoundedFrame(210, 8, 170, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(210, 188, 150, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(570, 188, 150, 38, 4, 2, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FAILURE_MODE);
		if (DROPDOWN_GetSel(hItem) == 1)
		{
			DRAW_Text("→", 410, 145, GUI_BLUE, GUI_TRANSPARENT, &GUI_FontLato30);
			DRAW_RoundedFrame(430, 143, 175, 38, 4, 2, GUI_BLUE);
		}

		break;
	case WM_DELETE:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_UNIT);
		EDIT_GetText(hItem, tempChannels[Selection].unit, UNIT_SIZE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RES);
		tempChannels[Selection].resolution = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MAX);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[Selection].graph_max_value = atoff(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MIN);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[Selection].graph_min_value = atoff(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FAILURE_VALUE);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[Selection].failureValue = atoff(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ARCH_STATE);
		tempChannels[Selection].ToArchive = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FAILURE_MODE);
		tempChannels[Selection].failureMode = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FILTER);
		tempChannels[Selection].filterType = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TIMEBASE);
		tempChannels[Selection].Tot1.period = DROPDOWN_GetSel(hItem);
		tempChannels[Selection].Tot2.period = tempChannels[Selection].Tot1.period;

		break;

	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_UNIT);
		EDIT_GetText(hItem, tempChannels[pMsg->Data.v].unit, UNIT_SIZE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RES);
		tempChannels[pMsg->Data.v].resolution = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MAX);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[pMsg->Data.v].graph_max_value = (float) atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MIN);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[pMsg->Data.v].graph_min_value = (float) atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ARCH_STATE);
		tempChannels[pMsg->Data.v].ToArchive = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FAILURE_MODE);
		tempChannels[pMsg->Data.v].failureMode = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FILTER);
		tempChannels[pMsg->Data.v].filterType = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FAILURE_VALUE);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[pMsg->Data.v].failureValue = (float) atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TIMEBASE);
		tempChannels[pMsg->Data.v].Tot1.period = DROPDOWN_GetSel(hItem);
		tempChannels[pMsg->Data.v].Tot2.period = tempChannels[pMsg->Data.v].Tot1.period;

		break;

	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_UNIT);
		EDIT_SetText(hItem, tempChannels[pMsg->Data.v].unit);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RES);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].resolution);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FAILURE_MODE);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].failureMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_FILTER);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].filterType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MAX);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFloat(hItem, tempChannels[pMsg->Data.v].graph_max_value, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAPH_MIN);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFloat(hItem, tempChannels[pMsg->Data.v].graph_min_value, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FAILURE_VALUE);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFloat(hItem, tempChannels[pMsg->Data.v].failureValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ARCH_STATE);
		CHECKBOX_SetState(hItem, tempChannels[pMsg->Data.v].ToArchive);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TIMEBASE);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].Tot1.period);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelTotlizersWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	TOTALIZER *selectedTotalizer;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_MULTIPLER);
		TEXT_SetText(hItem, GUI_LANG_GetText(113));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_TYPE);
		TEXT_SetText(hItem, GUI_LANG_GetText(39));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_UNIT);
		TEXT_SetText(hItem, GUI_LANG_GetText(33));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_RES);
		TEXT_SetText(hItem, GUI_LANG_GetText(34));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_ARCH_STATE);
		TEXT_SetText(hItem, GUI_LANG_GetText(37));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TOTALIZER_EMAIL_REPORT);
		TEXT_SetText(hItem, GUI_LANG_GetText(211));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		if (Tot1Win == 0)
		{
			Tot1Win = pMsg->hWin;
			selectedTotalizer = &tempChannels[0].Tot1;
		}
		else if (Tot2Win == 0)
		{
			Tot2Win = pMsg->hWin;
			selectedTotalizer = &tempChannels[0].Tot2;
		}
		else
			break;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOTALIZER_UNIT);
		EDIT_SetText(hItem, selectedTotalizer->unit);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_TYPE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(40));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(41));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(43));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(44));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(45));
		DROPDOWN_SetSel(hItem, selectedTotalizer->type);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_MULTIPLER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "0.001");
		DROPDOWN_AddString(hItem, "1");
		DROPDOWN_AddString(hItem, "1000");
		DROPDOWN_SetSel(hItem, selectedTotalizer->multipler);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_RES);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "0");
		DROPDOWN_AddString(hItem, "0.0");
		DROPDOWN_AddString(hItem, "0.00");
		DROPDOWN_AddString(hItem, "0.000");
		DROPDOWN_AddString(hItem, "0.0000");
		DROPDOWN_SetUpMode(hItem, 1);
		DROPDOWN_SetSel(hItem, selectedTotalizer->resolution);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_ARCH_STATE);
		CHECKBOX_SetText(hItem, "");
		CHECKBOX_SetTextColor(hItem, 0x0000c000);
		CHECKBOX_SetState(hItem, selectedTotalizer->ToArchive);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_EMAIL_REPORT);
		CHECKBOX_SetText(hItem, "");
		CHECKBOX_SetState(hItem, selectedTotalizer->sendEmail);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_TOTALIZER_UNIT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 10);
				break;
			}
			break;

		case ID_DROPDOWN_TOTALIZER_TYPE:
		case ID_DROPDOWN_TOTALIZER_MULTIPLER:
		case ID_DROPDOWN_TOTALIZER_RES:
		case ID_CHECKBOX_TOTALIZER_ARCH_STATE:
		case ID_CHECKBOX_TOTALIZER_EMAIL_REPORT:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(180, 53, 200, 38, 4, 2, GUI_BLUE);
		break;

	case WM_DELETE:
		if (Tot1Win == pMsg->hWin)
			selectedTotalizer = &tempChannels[Selection].Tot1;
		else if (Tot2Win == pMsg->hWin)
			selectedTotalizer = &tempChannels[Selection].Tot2;
		else
			break;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOTALIZER_UNIT);
		EDIT_GetText(hItem, selectedTotalizer->unit, UNIT_SIZE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_TYPE);
		selectedTotalizer->type = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_MULTIPLER);
		selectedTotalizer->multipler = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_RES);
		selectedTotalizer->resolution = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_ARCH_STATE);
		selectedTotalizer->ToArchive = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_EMAIL_REPORT);
		selectedTotalizer->sendEmail = CHECKBOX_GetState(hItem);
		break;

	case WM_USER_SAVE_TEMP:

		if (Tot1Win == pMsg->hWin)
			selectedTotalizer = &tempChannels[pMsg->Data.v].Tot1;
		else if (Tot2Win == pMsg->hWin)
			selectedTotalizer = &tempChannels[pMsg->Data.v].Tot2;
		else
			break;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOTALIZER_UNIT);
		EDIT_GetText(hItem, selectedTotalizer->unit, UNIT_SIZE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_TYPE);
		selectedTotalizer->type = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_MULTIPLER);
		selectedTotalizer->multipler = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_RES);
		selectedTotalizer->resolution = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_ARCH_STATE);
		selectedTotalizer->ToArchive = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_EMAIL_REPORT);
		selectedTotalizer->sendEmail = CHECKBOX_GetState(hItem);
		break;

	case WM_USER_REFRESH:
		if (Tot1Win == pMsg->hWin)
			selectedTotalizer = &tempChannels[pMsg->Data.v].Tot1;
		else if (Tot2Win == pMsg->hWin)
			selectedTotalizer = &tempChannels[pMsg->Data.v].Tot2;
		else
			break;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_TOTALIZER_UNIT);
		EDIT_SetText(hItem, selectedTotalizer->unit);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_TYPE);
		DROPDOWN_SetSel(hItem, selectedTotalizer->type);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_MULTIPLER);
		DROPDOWN_SetSel(hItem, selectedTotalizer->multipler);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TOTALIZER_RES);
		DROPDOWN_SetSel(hItem, selectedTotalizer->resolution);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_ARCH_STATE);
		CHECKBOX_SetState(hItem, selectedTotalizer->ToArchive);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_TOTALIZER_EMAIL_REPORT);
		CHECKBOX_SetState(hItem, selectedTotalizer->sendEmail);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelAlarmsWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int AlarmInstance = 0;
	uint8_t boardType = 0;
	char GUITextBuffer[20];

	switch (pMsg->MsgId)
	{

	case WM_INIT_DIALOG:

		if (Alarm1Win == 0)
		{
			Alarm1Win = pMsg->hWin;
			AlarmInstance = 0;
		}
		else if (Alarm2Win == 0)
		{
			Alarm2Win = pMsg->hWin;
			AlarmInstance = 1;
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_TYPE);
		TEXT_SetText(hItem, GUI_LANG_GetText(39));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(105));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_LEVEL);
		TEXT_SetText(hItem, GUI_LANG_GetText(46));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_TYPE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(102));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(103));
		DROPDOWN_SetSel(hItem, tempChannels[0].alarm[AlarmInstance].type);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_MODE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(104));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(108));
		DROPDOWN_SetSel(hItem, tempChannels[0].alarm[AlarmInstance].mode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_HISTER);
		TEXT_SetText(hItem, GUI_LANG_GetText(47));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_COLOR);
		TEXT_SetText(hItem, GUI_LANG_GetText(48));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_LEVEL);
		EDIT_SetFloat(hItem, tempChannels[0].alarm[AlarmInstance].level, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_HISTER);
		EDIT_SetFloat(hItem, tempChannels[0].alarm[AlarmInstance].hysteresis, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(109));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_COLOR);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(83));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(101));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(82));
		DROPDOWN_SetSel(hItem, tempChannels[0].alarm[AlarmInstance].color);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "--");
		DROPDOWN_AddString(hItem, "A");
		DROPDOWN_AddString(hItem, "B");
		DROPDOWN_AddString(hItem, "C");
		DROPDOWN_AddString(hItem, "D");
		DROPDOWN_AddString(hItem, "E");
		DROPDOWN_AddString(hItem, "F");
		DROPDOWN_AddString(hItem, "G");

		boardType = GetBoardType(tempChannels[0].alarm[AlarmInstance].output.board);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		FillIOBoardDropdownList(hItem, boardType);

		if (tempChannels[0].alarm[AlarmInstance].output.type == 0)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
			DROPDOWN_SetSel(hItem, 0);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
			DROPDOWN_SetSel(hItem, 0);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_BOARD_DESC);
			WM_HideWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
			DROPDOWN_SetSel(hItem, tempChannels[0].alarm[AlarmInstance].output.board + 1);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
			DROPDOWN_SetSel(hItem, tempChannels[0].alarm[AlarmInstance].output.number);
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_BOARD_DESC);
			WM_ShowWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_BOARD_DESC);
		TEXT_SetText(hItem, GetExpansionCardDescription(boardType));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_LOG);
		TEXT_SetText(hItem, GUI_LANG_GetText(106));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_FREQ);
		TEXT_SetText(hItem, GUI_LANG_GetText(107));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_EMAIL);
		TEXT_SetText(hItem, GUI_LANG_GetText(211));
		TEXT_SetTextColor(hItem, GUI_BLUE);


		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_LOG);
		CHECKBOX_SetState(hItem, tempChannels[0].alarm[AlarmInstance].logEvent);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_FREQ);
		CHECKBOX_SetState(hItem, tempChannels[0].alarm[AlarmInstance].ChangeArchivizationFrequency);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_EMAIL);
		CHECKBOX_SetState(hItem, tempChannels[0].alarm[AlarmInstance].sendEmail);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_ALARM_TYPE:
		case ID_DROPDOWN_ALARM_MODE:
		case ID_DROPDOWN_ALARM_COLOR:
		case ID_DROPDOWN_ALARM_BOARD:
		case ID_CHECKBOX_ALARM_EMAIL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_DROPDOWN_ALARM_CHANNEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				if (DROPDOWN_GetSel(pMsg->hWinSrc) == 0)
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
					WM_HideWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_BOARD_DESC);
					WM_HideWindow(hItem);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
					boardType = GetBoardType(DROPDOWN_GetSel(pMsg->hWinSrc) - 1);
					FillIOBoardDropdownList(hItem, boardType);
					WM_ShowWindow(hItem);

					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_BOARD_DESC);
					TEXT_SetText(hItem, GetExpansionCardDescription(boardType));
					WM_ShowWindow(hItem);
				}
				break;
			}
			break;

		case ID_EDIT_ALARM_LEVEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_ALARM_HISTER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;

		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(130, 53, 200, 39, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(520, 53, 200, 39, 4, 2, GUI_BLUE);

		break;
	case WM_DELETE:
		if (Alarm1Win == pMsg->hWin)
			AlarmInstance = 0;
		else if (Alarm2Win == pMsg->hWin)
			AlarmInstance = 1;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_TYPE);
		tempChannels[Selection].alarm[AlarmInstance].type = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_MODE);
		tempChannels[Selection].alarm[AlarmInstance].mode = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_LEVEL);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[Selection].alarm[AlarmInstance].level = atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_HISTER);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[Selection].alarm[AlarmInstance].hysteresis = atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
		if (DROPDOWN_GetSel(hItem) != 0)
		{
			tempChannels[Selection].alarm[AlarmInstance].output.type = 1;
			tempChannels[Selection].alarm[AlarmInstance].output.board = DROPDOWN_GetSel(hItem) - 1;
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
			tempChannels[Selection].alarm[AlarmInstance].output.number = DROPDOWN_GetSel(hItem);
		}
		else
		{
			tempChannels[Selection].alarm[AlarmInstance].output.type = 0;
			tempChannels[Selection].alarm[AlarmInstance].output.board = 0;
			tempChannels[Selection].alarm[AlarmInstance].output.number = 0;
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_COLOR);
		tempChannels[Selection].alarm[AlarmInstance].color = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_LOG);
		tempChannels[Selection].alarm[AlarmInstance].logEvent = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_FREQ);
		tempChannels[Selection].alarm[AlarmInstance].ChangeArchivizationFrequency = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_EMAIL);
		tempChannels[Selection].alarm[AlarmInstance].sendEmail = CHECKBOX_GetState(hItem);

		break;
	case WM_USER_SAVE_TEMP:
		if (Alarm1Win == pMsg->hWin)
			AlarmInstance = 0;
		else if (Alarm2Win == pMsg->hWin)
			AlarmInstance = 1;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_TYPE);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].type = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_MODE);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].mode = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_LEVEL);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].level = atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_HISTER);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].hysteresis = atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
		if (DROPDOWN_GetSel(hItem))
		{
			tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.type = 1;
			tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.board = DROPDOWN_GetSel(hItem) - 1;
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
			tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.number = DROPDOWN_GetSel(hItem);
		}
		else
		{
			tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.type = 0;
			tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.board = 0;
			tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.number = 0;
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_COLOR);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].color = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_LOG);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].logEvent = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_FREQ);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].ChangeArchivizationFrequency = CHECKBOX_GetState(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_EMAIL);
		tempChannels[pMsg->Data.v].alarm[AlarmInstance].sendEmail = CHECKBOX_GetState(hItem);

		break;

	case WM_USER_REFRESH:
		if (Alarm1Win == pMsg->hWin)
			AlarmInstance = 0;
		else if (Alarm2Win == pMsg->hWin)
			AlarmInstance = 1;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_TYPE);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].type);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_MODE);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].mode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_LEVEL);
		EDIT_SetFloat(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].level, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ALARM_HISTER);
		EDIT_SetFloat(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].hysteresis, 4);

		if (tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.type)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
			DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.board + 1);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
			WM_ShowWindow(hItem);
			DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].output.number);
			WM_ShowWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_CHANNEL);
			DROPDOWN_SetSel(hItem, 0);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_BOARD);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ALARM_OUTPUT_BOARD_DESC);
			WM_HideWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ALARM_COLOR);
		DROPDOWN_SetSel(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].color);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_LOG);
		CHECKBOX_SetState(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].logEvent);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_FREQ);
		CHECKBOX_SetState(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].ChangeArchivizationFrequency);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_ALARM_EMAIL);
		CHECKBOX_SetState(hItem, tempChannels[pMsg->Data.v].alarm[AlarmInstance].sendEmail);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelInputsMeasurmentWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_HWIN handler;
	int NCode;
	int Id;
	uint8_t SelectedBoard = 0;
	uint8_t SelectedInput = 0;
	uint8_t boardType = 0;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		SelectedBoard = tempChannels[Selection].source.board;
		SelectedInput = tempChannels[Selection].source.number;
		boardType = GetBoardType(SelectedBoard);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SOURCE_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(84));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_SLOT_NUMBER_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "A");
		DROPDOWN_AddString(hItem, "B");
		DROPDOWN_AddString(hItem, "C");
		DROPDOWN_AddString(hItem, "D");
		DROPDOWN_AddString(hItem, "E");
		DROPDOWN_AddString(hItem, "F");
		DROPDOWN_AddString(hItem, "G");
		DROPDOWN_SetSel(hItem, SelectedBoard);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INPUT_BOARD_DESC);
		TEXT_SetText(hItem, GetExpansionCardDescription(boardType));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_INPUT_NUMBER_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		FillIOBoardDropdownList(hItem, boardType);
		DROPDOWN_SetSel(hItem, SelectedInput);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHARACTERISTIC);
		TEXT_SetText(hItem, GUI_LANG_GetText(32));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		SelectCharacteristicList(hItem, SelectedBoard, SelectedInput, tempChannels[Selection].CharacteristicType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for(int i=0;i<10;i++)
			DROPDOWN_AddString(hItem, tempUserCharacteristics[i].name);
		DROPDOWN_SetSel(hItem, tempChannels[Selection].UserCharacteristicType);

		setCharacteristicsWidgets(pMsg->hWin, SelectedBoard, SelectedInput);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_VIEW);
		BUTTON_SetText(hItem, GUI_LANG_GetText(191));

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_USER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_SLOT_NUMBER_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				SelectedBoard = DROPDOWN_GetSel(pMsg->hWinSrc);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_INPUT_NUMBER_DROPDOWN);
				boardType = GetBoardType(SelectedBoard);
				FillIOBoardDropdownList(hItem, boardType);
				SelectedInput = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INPUT_BOARD_DESC);
				TEXT_SetText(hItem, GetExpansionCardDescription(boardType));

				setCharacteristicsWidgets(pMsg->hWin, SelectedBoard, SelectedInput);

				tempChannels[Selection].source.board = SelectedBoard;
				tempChannels[Selection].source.number = SelectedInput;
				break;
			}
			break;
		case ID_INPUT_NUMBER_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				SelectedBoard = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_SLOT_NUMBER_DROPDOWN));
				SelectedInput = DROPDOWN_GetSel(pMsg->hWinSrc);
				setCharacteristicsWidgets(pMsg->hWin, SelectedBoard, SelectedInput);
				tempChannels[Selection].source.number = SelectedInput;
				break;
			}
			break;

		case ID_CHARACTERISTIC_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);

				if (DROPDOWN_GetSel(pMsg->hWinSrc) == 1)
				{
					HideLowScaleElements(pMsg->hWin);
					HideHighScaleElements(pMsg->hWin);
					ShowUserCharElements(pMsg->hWin);
				}
				else if (DROPDOWN_GetSel(pMsg->hWinSrc) == 0)
				{
					ShowLowScaleElements(pMsg->hWin);
					ShowHighScaleElements(pMsg->hWin);
					HideUserCharElements(pMsg->hWin);
				}
				else
				{
					HideLowScaleElements(pMsg->hWin);
					HideHighScaleElements(pMsg->hWin);
					HideUserCharElements(pMsg->hWin);
				}
				break;
			}
			break;
		case ID_BUTTON_VIEW:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER));
				handler = WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(pMsg->hWin))));
				CreateWindowUserChar(handler, DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER)));
				break;
			}
			break;
		case ID_EDIT_LOW_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_LOW_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;

		}
		break;
	case WM_POST_PAINT:
		SelectedBoard = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_SLOT_NUMBER_DROPDOWN));
		boardType = GetBoardType(SelectedBoard);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		if (boardType == SPI_CARD_IN6D)
		{
			SelectedInput = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_INPUT_NUMBER_DROPDOWN));
			if (bkExpansionCards[SelectedBoard].settings.IOcard.channels[SelectedInput].type == 3)
				DrawLowScaleDecorations();
			else
			{
				if (DROPDOWN_GetSel(hItem) == 0)
				{
					DrawLowScaleDecorations();
					DrawHighScaleDecorations();
				}
			}
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
			if (DROPDOWN_GetSel(hItem) == 0)
			{
				DrawHighScaleDecorations();
				DrawLowScaleDecorations();
			}
		}
		break;
	case WM_REFRESH_CHAR:
		hItem = WM_GetDialogItem(WM_GetActiveWindow(), ID_DROPDOWN_USER);
		int index = pMsg->Data.v;
		DROPDOWN_DeleteItem(hItem, index);
		DROPDOWN_InsertString(hItem, tempUserCharacteristics[index].name, index);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelInputsComputedWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_HWIN handler;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SOURCE_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(129));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DOTS);
		TEXT_SetBkColor(hItem, EDIT_GetDefaultBkColor(EDIT_CI_ENABELD));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FORMULA);
		EDIT_SetText(hItem, tempChannels[Selection].formula.userFormula);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHARACTERISTIC);
		TEXT_SetText(hItem, GUI_LANG_GetText(32));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(126));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(190));
		DROPDOWN_AddString(hItem, "1:1");
		DROPDOWN_SetListHeight(hItem, 90);
		if (tempChannels[Selection].CharacteristicType > 2)
			tempChannels[Selection].CharacteristicType = 0;
		DROPDOWN_SetSel(hItem, tempChannels[Selection].CharacteristicType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_SIGNAL);
		EDIT_SetFloat(hItem, tempChannels[Selection].LowSignalValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_SCALE);
		EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_SIGNAL);
		EDIT_SetFloat(hItem, tempChannels[Selection].HighSignalValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_SCALE);
		EDIT_SetFloat(hItem,  tempChannels[Selection].HighScaleValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for(int i=0;i<10;i++)
			DROPDOWN_AddString(hItem, tempUserCharacteristics[i].name);
		DROPDOWN_SetSel(hItem, tempChannels[Selection].UserCharacteristicType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_VIEW);
		BUTTON_SetText(hItem, GUI_LANG_GetText(191));

		if (tempChannels[Selection].CharacteristicType == 0)
		{
			HideUserCharElements(pMsg->hWin);
			ShowHighScaleElements(pMsg->hWin);
			ShowLowScaleElements(pMsg->hWin);
		}
		else if (tempChannels[Selection].CharacteristicType == 1)
		{
			ShowUserCharElements(pMsg->hWin);
			HideHighScaleElements(pMsg->hWin);
			HideLowScaleElements(pMsg->hWin);
		}
		else
		{
			HideUserCharElements(pMsg->hWin);
			HideHighScaleElements(pMsg->hWin);
			HideLowScaleElements(pMsg->hWin);
		}
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_USER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_CHARACTERISTIC_DROPDOWN:
			WM_InvalidateWindow(pMsg->hWin);
			if (DROPDOWN_GetSel(pMsg->hWinSrc) == 1)
			{
				HideLowScaleElements(pMsg->hWin);
				HideHighScaleElements(pMsg->hWin);
				ShowUserCharElements(pMsg->hWin);

			}
			else if (DROPDOWN_GetSel(pMsg->hWinSrc) == 0)
			{
				ShowLowScaleElements(pMsg->hWin);
				ShowHighScaleElements(pMsg->hWin);
				HideUserCharElements(pMsg->hWin);
			}
			else
			{
				HideLowScaleElements(pMsg->hWin);
				HideHighScaleElements(pMsg->hWin);
				HideUserCharElements(pMsg->hWin);
			}
			break;
		case ID_BUTTON_VIEW:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER));
				handler = WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(pMsg->hWin))));
				CreateWindowUserChar(handler, DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER)));
				break;
			}
			break;
		case ID_EDIT_LOW_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_LOW_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_HIGH_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_FORMULA:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboardExtended(pMsg->hWinSrc, EDIT, FORMULA_MAX_CHAR);
				break;
			}
			break;
		case ID_TEXT_DOTS:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FORMULA);
				setNumKeyboardExtended(hItem, EDIT, FORMULA_MAX_CHAR);
				break;
			}
			break;
		}
		break;

	case WM_POST_PAINT:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		DRAW_RoundedFrame(248, 0, 204, 39, 4, 2, GUI_BLUE);

		if (DROPDOWN_GetSel(hItem) == 0)
		{
			DrawHighScaleDecorations();
			DrawLowScaleDecorations();
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FORMULA);
		CheckFormulaResolution(hItem);
		if (EDIT_GetNumChars(hItem) > 14)
			ShowDots(pMsg->hWin);
		else
			HideDots(pMsg->hWin);
		break;

	case WM_REFRESH_CHAR:
		hItem = WM_GetDialogItem(WM_GetActiveWindow(), ID_DROPDOWN_USER);
		int index = pMsg->Data.v;
		DROPDOWN_DeleteItem(hItem, index);
		DROPDOWN_InsertString(hItem, tempUserCharacteristics[index].name, index);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelModbusWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_HWIN handler;
	int NCode;
	int Id;
	uint8_t SelectedBoard = 0;
	uint8_t SelectedInput = 0;
	uint8_t boardType = 0;

	char GUITextBuffer[40];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		SelectedInput = tempChannels[Selection].source.number;

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SOURCE_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(84));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_INPUT_NUMBER_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for(int i=0;i<MAX_MODBUS_TCP_REGISTERS;i++)
		{
			if(	0==bkModbusTCPregisters[i].connectionID)
				DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
			else
			{
				mini_snprintf(GUITextBuffer, 40, "%d. (%d|%d.%d.%d.%d:%d)",i+1, bkModbusTCPregisters[i].number,
						bkMBserver[bkModbusTCPregisters[i].connectionID-1].ip[0], bkMBserver[bkModbusTCPregisters[i].connectionID-1].ip[1],
						bkMBserver[bkModbusTCPregisters[i].connectionID-1].ip[2], bkMBserver[bkModbusTCPregisters[i].connectionID-1].ip[3],
						bkMBserver[bkModbusTCPregisters[i].connectionID-1].port);
				DROPDOWN_AddString(hItem, GUITextBuffer);
			}
		}
		DROPDOWN_SetSel(hItem, SelectedInput);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHARACTERISTIC);
		TEXT_SetText(hItem, GUI_LANG_GetText(32));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(126));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(190));
		DROPDOWN_AddString(hItem, "1:1");
		DROPDOWN_SetListHeight(hItem, 90);
		if (tempChannels[Selection].CharacteristicType > 2)
			tempChannels[Selection].CharacteristicType = 0;
		DROPDOWN_SetSel(hItem, tempChannels[Selection].CharacteristicType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_SIGNAL);
		EDIT_SetFloat(hItem, tempChannels[Selection].LowSignalValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_SCALE);
		EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_SIGNAL);
		EDIT_SetFloat(hItem, tempChannels[Selection].HighSignalValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_SCALE);
		EDIT_SetFloat(hItem,  tempChannels[Selection].HighScaleValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for(int i=0;i<10;i++)
			DROPDOWN_AddString(hItem, tempUserCharacteristics[i].name);
		DROPDOWN_SetSel(hItem, tempChannels[Selection].UserCharacteristicType);

		if (tempChannels[Selection].CharacteristicType == 0)
		{
			HideUserCharElements(pMsg->hWin);
			ShowHighScaleElements(pMsg->hWin);
			ShowLowScaleElements(pMsg->hWin);
		}
		else if (tempChannels[Selection].CharacteristicType == 1)
		{
			ShowUserCharElements(pMsg->hWin);
			HideHighScaleElements(pMsg->hWin);
			HideLowScaleElements(pMsg->hWin);
		}
		else
		{
			HideUserCharElements(pMsg->hWin);
			HideHighScaleElements(pMsg->hWin);
			HideLowScaleElements(pMsg->hWin);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_VIEW);
		BUTTON_SetText(hItem, GUI_LANG_GetText(191));

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_USER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_INPUT_NUMBER_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				SelectedInput = DROPDOWN_GetSel(pMsg->hWinSrc);
				tempChannels[Selection].source.number = SelectedInput;
				break;
			}
			break;
		case ID_CHARACTERISTIC_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);

				if (DROPDOWN_GetSel(pMsg->hWinSrc) == 1)
				{
					HideLowScaleElements(pMsg->hWin);
					HideHighScaleElements(pMsg->hWin);
					ShowUserCharElements(pMsg->hWin);
				}
				else if (DROPDOWN_GetSel(pMsg->hWinSrc) == 0)
				{
					ShowLowScaleElements(pMsg->hWin);
					ShowHighScaleElements(pMsg->hWin);
					HideUserCharElements(pMsg->hWin);
				}
				else
				{
					HideLowScaleElements(pMsg->hWin);
					HideHighScaleElements(pMsg->hWin);
					HideUserCharElements(pMsg->hWin);
				}
				break;
			}
			break;
		case ID_BUTTON_VIEW:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER));
				handler = WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(pMsg->hWin))));
				CreateWindowUserChar(handler, DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER)));
				break;
			}
			break;
		case ID_EDIT_LOW_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_LOW_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;

		}
		break;
	case WM_POST_PAINT:
		SelectedBoard = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_SLOT_NUMBER_DROPDOWN));
		boardType = GetBoardType(SelectedBoard);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		if (boardType == SPI_CARD_IN6D)
		{
			SelectedInput = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_INPUT_NUMBER_DROPDOWN));
			if (bkExpansionCards[SelectedBoard].settings.IOcard.channels[SelectedInput].type == 3)
				DrawLowScaleDecorations();
			else
			{
				if (DROPDOWN_GetSel(hItem) == 0)
				{
					DrawLowScaleDecorations();
					DrawHighScaleDecorations();
				}
			}
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
			if (DROPDOWN_GetSel(hItem) == 0)
			{
				DrawHighScaleDecorations();
				DrawLowScaleDecorations();
			}
		}
		break;
	case WM_REFRESH_CHAR:
		hItem = WM_GetDialogItem(WM_GetActiveWindow(), ID_DROPDOWN_USER);
		int index = pMsg->Data.v;
		DROPDOWN_DeleteItem(hItem, index);
		DROPDOWN_InsertString(hItem, tempUserCharacteristics[index].name, index);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetChannelInputsDemoWindow(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_HWIN handler;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SOURCE_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(129));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_FORMULA);
		EDIT_SetText(hItem, "sin(t)");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CHARACTERISTIC);
		TEXT_SetText(hItem, GUI_LANG_GetText(32));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(126));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(190));
		DROPDOWN_AddString(hItem, "1:1");
		DROPDOWN_SetSel(hItem, tempChannels[Selection].CharacteristicType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_SIGNAL);
		EDIT_SetFloat(hItem, tempChannels[Selection].LowSignalValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LOW_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LOW_SCALE);
		EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_SIGNAL);
		EDIT_SetFloat(hItem, tempChannels[Selection].HighSignalValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_HIGH_INPUT_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HIGH_SCALE);
		EDIT_SetFloat(hItem, tempChannels[Selection].HighScaleValue, 4);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[0].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[1].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[2].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[3].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[4].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[5].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[6].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[7].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[8].name);
		DROPDOWN_AddString(hItem, tempUserCharacteristics[9].name);
		DROPDOWN_SetSel(hItem, tempChannels[Selection].UserCharacteristicType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_VIEW);
		BUTTON_SetText(hItem, GUI_LANG_GetText(191));

		if (tempChannels[Selection].CharacteristicType == 0)
		{
			HideUserCharElements(pMsg->hWin);
			ShowHighScaleElements(pMsg->hWin);
			ShowLowScaleElements(pMsg->hWin);
		}
		else if (tempChannels[Selection].CharacteristicType == 1)
		{
			ShowUserCharElements(pMsg->hWin);
			HideHighScaleElements(pMsg->hWin);
			HideLowScaleElements(pMsg->hWin);
		}
		else
		{
			HideUserCharElements(pMsg->hWin);
			HideHighScaleElements(pMsg->hWin);
			HideLowScaleElements(pMsg->hWin);
		}
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_USER:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_CHARACTERISTIC_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);
				if (DROPDOWN_GetSel(pMsg->hWinSrc) == 1)
				{
					HideLowScaleElements(pMsg->hWin);
					HideHighScaleElements(pMsg->hWin);
					ShowUserCharElements(pMsg->hWin);
				}
				else if (DROPDOWN_GetSel(pMsg->hWinSrc) == 0)
				{
					ShowLowScaleElements(pMsg->hWin);
					ShowHighScaleElements(pMsg->hWin);
					HideUserCharElements(pMsg->hWin);
				}
				else
				{
					HideLowScaleElements(pMsg->hWin);
					HideHighScaleElements(pMsg->hWin);
					HideUserCharElements(pMsg->hWin);
				}
				break;
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_SLOT_NUMBER_DROPDOWN:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INPUT_BOARD_DESC);
				TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
				TEXT_SetTextColor(hItem, GUI_BLACK);
				TEXT_SetText(hItem, GetExpansionCardDescription(bkExpansionCards[DROPDOWN_GetSel(pMsg->hWinSrc)].type));
				break;
			}
			break;
		case ID_BUTTON_VIEW:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER));
				handler = WM_GetParent(WM_GetParent(WM_GetParent(WM_GetParent(pMsg->hWin))));
				CreateWindowUserChar(handler, DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_USER)));
				break;
			}
			break;
		case ID_EDIT_LOW_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_HIGH_SCALE:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_LOW_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_HIGH_SIGNAL:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
		}
		break;

	case WM_POST_PAINT:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHARACTERISTIC_DROPDOWN);

		if (DROPDOWN_GetSel(hItem) == 0)
		{
			DrawHighScaleDecorations();
			DrawLowScaleDecorations();
		}

		break;
	case WM_REFRESH_CHAR:
		hItem = WM_GetDialogItem(WM_GetActiveWindow(), ID_DROPDOWN_USER);
		int index = pMsg->Data.v;
		DROPDOWN_DeleteItem(hItem, index);
		DROPDOWN_InsertString(hItem, tempUserCharacteristics[index].name, index);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void DrawLowScaleDecorations(void)
{
	DRAW_RoundedFrame(115, 93, 215, 39, 4, 2, GUI_BLUE);
	DRAW_Text("→", 350, 94, GUI_BLUE, GUI_TRANSPARENT, &GUI_FontLato30);
	DRAW_RoundedFrame(370, 93, 215, 39, 4, 2, GUI_BLUE);
}

static void DrawHighScaleDecorations(void)
{
	DRAW_RoundedFrame(115, 138, 215, 39, 4, 2, GUI_BLUE);
	DRAW_Text("→", 350, 139, GUI_BLUE, GUI_TRANSPARENT, &GUI_FontLato30);
	DRAW_RoundedFrame(370, 138, 215, 39, 4, 2, GUI_BLUE);
}

static void HideLowScaleElements(WM_HWIN hWin)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SCALE);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SIGNAL);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_LOW_INPUT_UNIT);
	WM_HideWindow(hItem);
}

static void HideHighScaleElements(WM_HWIN hWin)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SCALE);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SIGNAL);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_HIGH_INPUT_UNIT);
	WM_HideWindow(hItem);
}

static void ShowLowScaleElements(WM_HWIN hWin)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SCALE);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SIGNAL);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_LOW_INPUT_UNIT);
	WM_ShowWindow(hItem);
}

static void ShowHighScaleElements(WM_HWIN hWin)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SCALE);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SIGNAL);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_TEXT_HIGH_INPUT_UNIT);
	WM_ShowWindow(hItem);
}

WM_HWIN CreateSetChannels(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelWindow, GUI_COUNTOF(_aSetChannelWindow), _cbSetChannelWindow, hParent, 0, 0);
	USERMESSAGE_ChangeWinTitle(GUI_LANG_GetText(62));
	return hWin;
}

static WM_HWIN CreateChannelGeneral(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelGeneralWindow, GUI_COUNTOF(_aSetChannelGeneralWindow), _cbSetChannelGeneralWindow, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateChannelInputs(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelInputsWindow, GUI_COUNTOF(_aSetChannelInputsWindow), _cbSetChannelInputsWindow, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateChannelInputsMeasurment(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelInputsMeasurmentWindow, GUI_COUNTOF(_aSetChannelInputsMeasurmentWindow),
			_cbSetChannelInputsMeasurmentWindow, hParent, 0, 53);
	return hWin;
}

static WM_HWIN CreateChannelInputsComputed(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelInputsComputedWindow, GUI_COUNTOF(_aSetChannelInputsComputedWindow),
			_cbSetChannelInputsComputedWindow, hParent, 0, 53);
	return hWin;
}

static WM_HWIN CreateChannelModbus(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelModbusWindow, GUI_COUNTOF(_aSetChannelModbusWindow),
			_cbSetChannelModbusWindow, hParent, 0, 53);
	return hWin;
}

static WM_HWIN CreateChannelInputsDemo(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelInputsDemoWindow, GUI_COUNTOF(_aSetChannelInputsDemoWindow),
			_cbSetChannelInputsDemoWindow, hParent, 0, 53);
	return hWin;
}

static WM_HWIN CreateChannelTotalizers(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelTotlizersWindow, GUI_COUNTOF(_aSetChannelTotlizersWindow), _cbSetChannelTotlizersWindow, hParent,
			0, 0);
	return hWin;
}

static WM_HWIN CreateChannelAlarms(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetChannelAlarmsWindow, GUI_COUNTOF(_aSetChannelAlarmsWindow), _cbSetChannelAlarmsWindow, hParent, 0, 0);
	return hWin;
}

void FillIOBoardDropdownList(DROPDOWN_Handle hObj, uint8_t boardType)
{
	while (DROPDOWN_GetNumItems(hObj))
		DROPDOWN_DeleteItem(hObj, 0);
	char textBuffer[3];
	int channelsNumber = 0;

	if (SPI_CARD_2RS485 == boardType || SPI_CARD_2RS485I24 == boardType || SPI_CARD_1HRT == boardType)
		channelsNumber = 25;
	else if (SPI_CARD_IN3 == boardType || SPI_CARD_IN3RTD == boardType || SPI_CARD_OUT3 == boardType)
		channelsNumber = 3;
	else
		channelsNumber = 6;

	for (int i = 1; i <= channelsNumber; ++i)
	{
		itoa(i, textBuffer, 10);
		DROPDOWN_AddString(hObj, textBuffer);
	}

	if(SPI_CARD_IN6TC == boardType || SPI_CARD_IN3 == boardType)
	{
		DROPDOWN_AddString(hObj, "CJC °C");
		DROPDOWN_AddString(hObj, "CJC °F");
	}

}

void FillCharacteristicDropdownList(DROPDOWN_Handle hObj, uint8_t channelType, uint8_t CharacteristicType)
{
	while (DROPDOWN_GetNumItems(hObj))
		DROPDOWN_DeleteItem(hObj, 0);

	switch (channelType)
	{
	case RTD:
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(126));
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(190));
		DROPDOWN_AddString(hObj, "1:1");
		DROPDOWN_SetListHeight(hObj, 120);
		DROPDOWN_AddString(hObj, "Pt100(°C)");
		DROPDOWN_AddString(hObj, "Pt100(°F)");
		DROPDOWN_AddString(hObj, "Pt200(°C)");
		DROPDOWN_AddString(hObj, "Pt200(°F)");
		DROPDOWN_AddString(hObj, "Pt500(°C)");
		DROPDOWN_AddString(hObj, "Pt500(°F)");
		DROPDOWN_AddString(hObj, "Pt1000(°C)");
		DROPDOWN_AddString(hObj, "Pt1000(°F)");
		DROPDOWN_AddString(hObj, "Ni100(°C)");
		DROPDOWN_AddString(hObj, "Ni100(°F)");
		DROPDOWN_AddString(hObj, "Ni120(°C)");
		DROPDOWN_AddString(hObj, "Ni120(°F)");
		DROPDOWN_AddString(hObj, "Ni1000(°C)");
		DROPDOWN_AddString(hObj, "Ni1000(°F)");
		DROPDOWN_AddString(hObj, "Cu50(°C)");
		DROPDOWN_AddString(hObj, "Cu50(°F)");
		DROPDOWN_AddString(hObj, "Cu53(°C)");
		DROPDOWN_AddString(hObj, "Cu53(°F)");
		DROPDOWN_AddString(hObj, "Cu100(°C)");
		DROPDOWN_AddString(hObj, "Cu100(°F)");
		DROPDOWN_AddString(hObj, "KTY81(°C)");
		DROPDOWN_AddString(hObj, "KTY81(°F)");
		DROPDOWN_AddString(hObj, "KTY83(°C)");
		DROPDOWN_AddString(hObj, "KTY83(°F)");
		DROPDOWN_AddString(hObj, "KTY84(°C)");
		DROPDOWN_AddString(hObj, "KTY84(°F)");
		break;
	case TC:
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(126));
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(190));
		DROPDOWN_AddString(hObj, "1:1");
		DROPDOWN_SetListHeight(hObj, 120);
		DROPDOWN_AddString(hObj, "J(°C)");
		DROPDOWN_AddString(hObj, "J(°F)");
		DROPDOWN_AddString(hObj, "K(°C)");
		DROPDOWN_AddString(hObj, "K(°F)");
		DROPDOWN_AddString(hObj, "N(°C)");
		DROPDOWN_AddString(hObj, "N(°F)");
		DROPDOWN_AddString(hObj, "R(°C)");
		DROPDOWN_AddString(hObj, "R(°F)");
		DROPDOWN_AddString(hObj, "S(°C)");
		DROPDOWN_AddString(hObj, "S(°F)");
		DROPDOWN_AddString(hObj, "T(°C)");
		DROPDOWN_AddString(hObj, "T(°F)");
		DROPDOWN_AddString(hObj, "E(°C)");
		DROPDOWN_AddString(hObj, "E(°F)");
		DROPDOWN_AddString(hObj, "B(°C)");
		DROPDOWN_AddString(hObj, "B(°F)");
		DROPDOWN_AddString(hObj, "L(°C)");
		DROPDOWN_AddString(hObj, "L(°F)");
		DROPDOWN_AddString(hObj, "U(°C)");
		DROPDOWN_AddString(hObj, "U(°F)");
		break;
	case STATE:
		DROPDOWN_SetListHeight(hObj, 30);
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(181));
		break;
	case COUNTER:
		DROPDOWN_SetListHeight(hObj, 30);
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(183));
		break;
	default:
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(126));
		DROPDOWN_AddString(hObj, GUI_LANG_GetText(190));
		DROPDOWN_AddString(hObj, "1:1");
		DROPDOWN_SetListHeight(hObj, 90);
		break;
	}
	DROPDOWN_SetSel(hObj, CharacteristicType);
}

void SelectCharacteristicList(DROPDOWN_Handle hItem, uint8_t Board, uint8_t Input, uint8_t CharacteristicType)
{
	switch (bkExpansionCards[Board].type)
	{
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		FillCharacteristicDropdownList(hItem, RTD, CharacteristicType);
		break;
	case SPI_CARD_IN6TC:
		if(6 > Input)
			FillCharacteristicDropdownList(hItem, TC, CharacteristicType);
		else
			FillCharacteristicDropdownList(hItem, OTHER, CharacteristicType);
		break;
	case SPI_CARD_IN3:
		if(3<=Input)
			FillCharacteristicDropdownList(hItem, OTHER, CharacteristicType);
		else if (bkExpansionCards[Board].settings.IOcard.channels[Input].type == 5)
			FillCharacteristicDropdownList(hItem, TC, CharacteristicType);
		else if (bkExpansionCards[Board].settings.IOcard.channels[Input].type >= 6)
			FillCharacteristicDropdownList(hItem, RTD, CharacteristicType);
		else
			FillCharacteristicDropdownList(hItem, OTHER, CharacteristicType);
		break;
	case SPI_CARD_IN6D:
		if (bkExpansionCards[Board].settings.IOcard.channels[Input].type == 1)
			FillCharacteristicDropdownList(hItem, STATE, CharacteristicType);
		else if (bkExpansionCards[Board].settings.IOcard.channels[Input].type == 3)
			FillCharacteristicDropdownList(hItem, COUNTER, CharacteristicType);
		else
			FillCharacteristicDropdownList(hItem, OTHER, CharacteristicType);
		break;
	case SPI_CARD_IN4SG:
		if(5==Input)
			FillCharacteristicDropdownList(hItem, STATE, CharacteristicType);
		else
			FillCharacteristicDropdownList(hItem, OTHER, CharacteristicType);
		break;
	default:
		FillCharacteristicDropdownList(hItem, OTHER, CharacteristicType);
		break;
	}
}

static int CheckChannelSettings(CHANNEL *Dst, CHANNEL *Src)
{
	if (Dst->source.type != Src->source.type || Dst->source.number != Src->source.number || Dst->source.board != Src->source.board)
	{
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}

	if (Dst->CharacteristicType != Src->CharacteristicType || Dst->HighScaleValue != Src->HighScaleValue
			|| Dst->HighSignalValue != Src->HighSignalValue || Dst->LowScaleValue != Src->LowScaleValue
			|| Dst->LowSignalValue != Src->LowSignalValue || Dst->ToArchive != Src->ToArchive || strcmp(Dst->description, Src->description)
			|| strcmp(Dst->unit, Src->unit) || Dst->resolution != Src->resolution || Dst->failureMode != Src->failureMode
			|| Dst->failureValue != Src->failureValue || Dst->filterType != Src->filterType || strcmp(Dst->formula.userFormula, Src->formula.userFormula) )
	{
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}

	if (Dst->Tot1.type != Src->Tot1.type || strcmp(Dst->Tot1.unit, Src->Tot1.unit) || Dst->Tot1.resolution != Src->Tot1.resolution
			|| Dst->Tot1.period != Src->Tot1.period || Dst->Tot1.multipler != Src->Tot1.multipler || Dst->Tot1.ToArchive != Src->Tot1.ToArchive
			|| Dst->Tot1.sendEmail != Src->Tot1.sendEmail)
	{
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}

	if (Dst->Tot2.type != Src->Tot2.type || strcmp(Dst->Tot2.unit, Src->Tot2.unit) || Dst->Tot2.resolution != Src->Tot2.resolution
			|| Dst->Tot2.period != Src->Tot2.period || Dst->Tot2.multipler != Src->Tot2.multipler || Dst->Tot2.ToArchive != Src->Tot2.ToArchive
			|| Dst->Tot2.sendEmail != Src->Tot2.sendEmail)
	{
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}

	if (Dst->alarm[0].mode != Src->alarm[0].mode || Dst->alarm[0].type != Src->alarm[0].type || Dst->alarm[0].level != Src->alarm[0].level
			|| Dst->alarm[0].hysteresis != Src->alarm[0].hysteresis || Dst->alarm[0].output.type != Src->alarm[0].output.type
			|| Dst->alarm[0].output.board != Src->alarm[0].output.board || Dst->alarm[0].output.number != Src->alarm[0].output.number
			|| Dst->alarm[0].logEvent != Src->alarm[0].logEvent
			|| Dst->alarm[0].ChangeArchivizationFrequency != Src->alarm[0].ChangeArchivizationFrequency
			|| Dst->alarm[0].sendEmail != Src->alarm[0].sendEmail)
	{
		RestartDevice = 1;
		return 1;
	}

	if (Dst->alarm[1].mode != Src->alarm[1].mode || Dst->alarm[1].type != Src->alarm[1].type || Dst->alarm[1].level != Src->alarm[1].level
			|| Dst->alarm[1].hysteresis != Src->alarm[1].hysteresis || Dst->alarm[1].output.type != Src->alarm[1].output.type
			|| Dst->alarm[1].output.board != Src->alarm[1].output.board || Dst->alarm[1].output.number != Src->alarm[1].output.number
			|| Dst->alarm[1].logEvent != Src->alarm[1].logEvent
			|| Dst->alarm[1].ChangeArchivizationFrequency != Src->alarm[1].ChangeArchivizationFrequency
			|| Dst->alarm[1].sendEmail != Src->alarm[1].sendEmail)
	{
		RestartDevice = 1;
		return 1;
	}

	if(Dst->graph_max_value != Src->graph_max_value || Dst->graph_min_value != Src->graph_min_value)
		return 1;

	return 0;
}

static void setCharacteristicsWidgets(WM_HWIN hWin, uint8_t SelectedBoard, uint8_t SelectedInput)
{
	WM_HWIN hItem;

	int boardType = GetBoardType(SelectedBoard);
	uint8_t measureMethod;
	if(SPI_CARD_2RS485 == boardType || SPI_CARD_2RS485I24 == boardType)
		measureMethod = 0;
	else if(SPI_CARD_OUT3 == boardType)
		measureMethod = bkExpansionCards[SelectedBoard].settings.OUTcard.channels[SelectedInput].type;
	else if(SPI_CARD_IN6TC == boardType)
	{
		if(6 == SelectedInput)
			measureMethod = 0xF0;
		else if (7 == SelectedInput)
			measureMethod = 0xF1;
		else
			measureMethod = bkExpansionCards[SelectedBoard].settings.IOcard.channels[SelectedInput].type;
	}
	else if(SPI_CARD_IN3 == boardType)
	{
		if(3 == SelectedInput)
			measureMethod = 0xF0;
		else if (4 == SelectedInput)
			measureMethod = 0xF1;
		else
			measureMethod = bkExpansionCards[SelectedBoard].settings.IOcard.channels[SelectedInput].type;
	}
	else
		measureMethod = bkExpansionCards[SelectedBoard].settings.IOcard.channels[SelectedInput].type;

	hItem = WM_GetDialogItem(hWin, ID_CHARACTERISTIC_DROPDOWN);
	uint8_t SelectedCharacteristicType = DROPDOWN_GetSel(hItem);
	SelectCharacteristicList(hItem, SelectedBoard, SelectedInput, SelectedCharacteristicType);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_LOW_INPUT_UNIT);
	if((SPI_CARD_IN4SG == boardType)&&(5==SelectedInput))
		TEXT_SetText(hItem, " ");
	else
		SetCardUnit(hItem, boardType, measureMethod);

	hItem = WM_GetDialogItem(hWin, ID_TEXT_HIGH_INPUT_UNIT);
	if((SPI_CARD_IN4SG == boardType)&&(5==SelectedInput))
		TEXT_SetText(hItem, " ");
	else
		SetCardUnit(hItem, boardType, measureMethod);

	WM_InvalidateWindow(hWin);
	if (boardType == SPI_CARD_IN6D)
	{
		if (measureMethod == 1)
		{
			ShowLowScaleElements(hWin);
			ShowHighScaleElements(hWin);
			HideUserCharElements(hWin);

			hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SIGNAL);
			EDIT_SetText(hItem, "0");
			WM_DisableWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SCALE);
			EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 6);

			hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SIGNAL);
			EDIT_SetText(hItem, "1");
			WM_DisableWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SCALE);
			EDIT_SetFloat(hItem, tempChannels[Selection].HighScaleValue, 6);
			return;
		}
		else if (measureMethod == 3)
		{
			ShowLowScaleElements(hWin);
			HideHighScaleElements(hWin);
			HideUserCharElements(hWin);

			hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SIGNAL);
			EDIT_SetText(hItem, "1");
			WM_DisableWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SCALE);
			EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 6);
			return;
		}
		else
		{
			if (SelectedCharacteristicType == 0)
			{
				ShowLowScaleElements(hWin);
				ShowHighScaleElements(hWin);
				HideUserCharElements(hWin);
			}

			else if (SelectedCharacteristicType == 1)
			{
				ShowUserCharElements(hWin);
				HideHighScaleElements(hWin);
				HideLowScaleElements(hWin);
			}
			else
			{
				HideLowScaleElements(hWin);
				HideHighScaleElements(hWin);
				HideUserCharElements(hWin);
			}
		}
	}
	else if (boardType == SPI_CARD_IN4SG)
	{
		if(5 == SelectedInput)
		{
			ShowLowScaleElements(hWin);
			ShowHighScaleElements(hWin);
			HideUserCharElements(hWin);

			hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SIGNAL);
			EDIT_SetText(hItem, "0");
			WM_DisableWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SCALE);
			EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 6);

			hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SIGNAL);
			EDIT_SetText(hItem, "1");
			WM_DisableWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SCALE);
			EDIT_SetFloat(hItem, tempChannels[Selection].HighScaleValue, 6);
			return;
		}
		else
		{
			if (SelectedCharacteristicType == 0)
			{
				ShowLowScaleElements(hWin);
				ShowHighScaleElements(hWin);
				HideUserCharElements(hWin);
			}

			else if (SelectedCharacteristicType == 1)
			{

				ShowUserCharElements(hWin);
				HideHighScaleElements(hWin);
				HideLowScaleElements(hWin);
			}
			else
			{
				HideLowScaleElements(hWin);
				HideHighScaleElements(hWin);
				HideUserCharElements(hWin);
			}
		}
	}
	else
	{
		if (SelectedCharacteristicType == 0)
		{
			ShowLowScaleElements(hWin);
			ShowHighScaleElements(hWin);
			HideUserCharElements(hWin);
		}

		else if (SelectedCharacteristicType == 1)
		{

			ShowUserCharElements(hWin);
			HideHighScaleElements(hWin);
			HideLowScaleElements(hWin);
		}
		else
		{
			HideLowScaleElements(hWin);
			HideHighScaleElements(hWin);
			HideUserCharElements(hWin);
		}
	}
	hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SIGNAL);
	WM_EnableWindow(hItem);
	EDIT_SetFloat(hItem, tempChannels[Selection].LowSignalValue, 6);
	hItem = WM_GetDialogItem(hWin, ID_EDIT_LOW_SCALE);
	EDIT_SetFloat(hItem, tempChannels[Selection].LowScaleValue, 6);

	hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SIGNAL);
	WM_EnableWindow(hItem);
	EDIT_SetFloat(hItem, tempChannels[Selection].HighSignalValue, 6);
	hItem = WM_GetDialogItem(hWin, ID_EDIT_HIGH_SCALE);
	EDIT_SetFloat(hItem, tempChannels[Selection].HighScaleValue, 6);
}

static void MultipageMessage(WM_HWIN hWinSrc, int msgId)
{
	for (int i = 0; i < 7; i++)
	{
		UserMessage.MsgId = msgId;
		UserMessage.hWinSrc = hWinSrc;
		UserMessage.Data.v = Selection;
		UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_SET_CHANNEL), i);
		WM_SendMessage(UserMessage.hWin, &UserMessage);
	}
}

static void ShowUserCharElements(WM_HWIN hWin)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_USER);
	WM_ShowWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_BUTTON_VIEW);
	WM_ShowWindow(hItem);
}

static void HideUserCharElements(WM_HWIN hWin)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_USER);
	WM_HideWindow(hItem);
	hItem = WM_GetDialogItem(hWin, ID_BUTTON_VIEW);
	WM_HideWindow(hItem);
}

static void HideDots (WM_HWIN hWin) {
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_TEXT_DOTS);
	WM_HideWindow(hItem);
}

static void ShowDots (WM_HWIN hWin) {
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(hWin, ID_TEXT_DOTS);
	WM_ShowWindow(hItem);
}

/* funkcja OBCINA jedynie liczby, NIE SPRAWDZA poprawnosci formuly */
static void CheckFormulaResolution (WM_HWIN hFormula) {
	char formulaBuffer[FORMULA_USER_SIZE];
	char newFormulaBuffer[FORMULA_USER_SIZE];
	EDIT_GetText(hFormula, formulaBuffer, FORMULA_USER_SIZE);
	int i = 0;
	int j = 0;
	int k = 0;
	int formulaResolution = MAX_ROUND_SIZE;
	while(formulaBuffer[i] != '\0')
	{
		newFormulaBuffer[j++] = formulaBuffer[i++];
		if (formulaBuffer[i-1] == '.') {
			while (isdigit(formulaBuffer[i]))
			{
				if (k < formulaResolution) {
					newFormulaBuffer[j++] = formulaBuffer[i++];
					k++;
				}
				else
					i++;
			}
			k = 0;
		}
	}
	newFormulaBuffer[j] = '\0';
	EDIT_SetText(hFormula, newFormulaBuffer);
}

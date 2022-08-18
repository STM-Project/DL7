#include <ExpansionCards.h>
#include "SetIOBoardsDLG.h"
#include <stdlib.h>
#include <string.h>
#include "draw.h"
#include "dtos.h"
#include "skins.h"
#include "KeyboardDLG.h"
#include "UserMessage.h"
#include "buzzer.h"
#include "parameters.h"

#include "ExpansionCards_descriptions.h"
#include "mini-printf.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_IO_BOARDS   (GUI_ID_USER + 0x201)
#define ID_BUTTON_OK      		(GUI_ID_USER + 0x202)
#define ID_MULTIPAGE_IO    		(GUI_ID_USER + 0x203)
#define ID_BUTTON_CANCEL  		(GUI_ID_USER + 0x204)
#define ID_DROPDOWN_SLOT     	(GUI_ID_USER + 0x205)
#define ID_TEXT_CARD_DESC     (GUI_ID_USER + 0x206)
#define ID_TEXT_CARD_TYPE     (GUI_ID_USER + 0x207)

#define ID_WINDOW_IO      								(GUI_ID_USER + 0x208)
#define ID_TEXT_MODE      								(GUI_ID_USER + 0x209)
#define ID_TEXT_RESISTOR      						(GUI_ID_USER + 0x300)
#define ID_TEXT_PREAMBLE      						(GUI_ID_USER + 0x301)
#define ID_DROPDOWN_RESISTOR  						(GUI_ID_USER + 0x302)
#define ID_DROPDOWN_MODE  								(GUI_ID_USER + 0x303)
#define ID_EDIT_PREAMBLE  								(GUI_ID_USER + 0x304)
#define ID_LISTVIEW_ADDRESSES							(GUI_ID_USER + 0x305)
#define ID_BUTTON_ADDRESS_CONFIRM					(GUI_ID_USER + 0x306)
#define ID_BUTTON_ADDRESS_ANNUL						(GUI_ID_USER + 0x307)
#define ID_FRAMEWIN_ADDRESSES							(GUI_ID_USER + 0x308)

#define ID_TEXT_ADDRESS_MODE      				(GUI_ID_USER + 0x309)
#define ID_DROPDOWN_ADDRESS_MODE      		(GUI_ID_USER + 0x30a)
#define ID_TEXT_SHORT_ADDR      					(GUI_ID_USER + 0x30b)
#define ID_EDIT_SHORT_ADDR      					(GUI_ID_USER + 0x30c)
#define ID_TEXT_LONG_ADDR      						(GUI_ID_USER + 0x30d)
#define ID_EDIT_LONG_ADDR      						(GUI_ID_USER + 0x30e)

#define ID_DROPDOWN_READ_VALUE      			(GUI_ID_USER + 0x30f)
#define ID_TEXT_READ_VALUE_ADDR						(GUI_ID_USER + 0x310)
#define ID_FRAMEWIN_READ_VALUE						(GUI_ID_USER + 0x311)
#define ID_TEXT_READ_VALUE_TYPE						(GUI_ID_USER + 0x312)
#define ID_DROPDOWN_VALUE_TYPE      			(GUI_ID_USER + 0x313)
#define ID_TEXT_VARIABLE_ADDR							(GUI_ID_USER + 0x314)
#define ID_EDIT_VARIABLE_ADDR      				(GUI_ID_USER + 0x315)
#define ID_TEXT_STATUS_VAR								(GUI_ID_USER + 0x316)
#define ID_DROPDOWN_STATUS_VAR      			(GUI_ID_USER + 0x317)
#define ID_DROPDOWN_COMMAND_01_OR_03      (GUI_ID_USER + 0x318)
#define ID_TEXT_COMMAND_01_OR_03      		(GUI_ID_USER + 0x319)
#define ID_TEXT_UNIT_PREAMBLE  						(GUI_ID_USER + 0x31a)

#define ID_TEXT_ADJUSMENT_DESC    				(GUI_ID_USER + 0x20A)
#define ID_TEXT_ADJUSMENT_UNIT    				(GUI_ID_USER + 0x20B)
#define ID_TEXT_COMPENSATION_DESC 				(GUI_ID_USER + 0x20C)
#define ID_EDIT_ADJUSMENT        					(GUI_ID_USER + 0x20D)
#define ID_DROPDOWN_COMPENSATION_CHANNEL	(GUI_ID_USER + 0x20F)

#define ID_EDIT_COMPENSATION_VALUE		(GUI_ID_USER + 0x210)
#define ID_TEXT_COMPENSATION_UNIT			(GUI_ID_USER + 0x211)

#define ID_WINDOW_DEBOUNCE				(GUI_ID_USER + 0x212)
#define ID_DROPDOWN_DEBOUNCE			(GUI_ID_USER + 0x213)
#define ID_TEXT_DEBOUNCE					(GUI_ID_USER + 0x214)

#define ID_WINDOW_RS485_PORT      			(GUI_ID_USER + 0x215)
#define ID_TEXT_RS485_PORT_1      			(GUI_ID_USER + 0x216)
#define ID_TEXT_RS485_PORT_2      			(GUI_ID_USER + 0x217)
#define ID_TEXT_RS485_BAUDRATE      		(GUI_ID_USER + 0x218)
#define ID_TEXT_RS485_PARITY      			(GUI_ID_USER + 0x219)
#define ID_TEXT_RS485_STOPBITS      		(GUI_ID_USER + 0x21A)
#define ID_TEXT_RS485_TIMEOUT      			(GUI_ID_USER + 0x21B)
#define ID_DROPDOWN_RS485_BAUDRATE_1    (GUI_ID_USER + 0x21C)
#define ID_DROPDOWN_RS485_PARITY_1      (GUI_ID_USER + 0x21D)
#define ID_DROPDOWN_RS485_STOPBITS_1    (GUI_ID_USER + 0x21E)
#define ID_EDIT_RS485_TIMEOUT_1      		(GUI_ID_USER + 0x21F)
#define ID_TEXT_RS485_TIMEOUT_UNIT_1    (GUI_ID_USER + 0x220)
#define ID_DROPDOWN_RS485_BAUDRATE_2    (GUI_ID_USER + 0x221)
#define ID_DROPDOWN_RS485_PARITY_2      (GUI_ID_USER + 0x222)
#define ID_DROPDOWN_RS485_STOPBITS_2    (GUI_ID_USER + 0x223)
#define ID_EDIT_RS485_TIMEOUT_2      		(GUI_ID_USER + 0x224)
#define ID_TEXT_RS485_TIMEOUT_UNIT_2    (GUI_ID_USER + 0x225)

#define ID_WINDOW_RS485_REGISTERS      	(GUI_ID_USER + 0x226)
#define ID_LISTVIEW_RS485_REGISTERS 		(GUI_ID_USER + 0x227)

#define FRAMEWIN_REGISTER								(GUI_ID_USER + 0x228)
#define ID_TEXT_REGISTER_PORT      			(GUI_ID_USER + 0x229)
#define ID_TEXT_REGISTER_DEVICE_ADDRESS	(GUI_ID_USER + 0x22A)
#define ID_TEXT_REGISTER_ADDRESS    		(GUI_ID_USER + 0x22B)
#define ID_TEXT_REGISTER_TYPE      			(GUI_ID_USER + 0x22C)
#define ID_DROPDOWN_REGISTER_PORT   		(GUI_ID_USER + 0x22D)
#define ID_EDIT_DEVICE_ADDRESS					(GUI_ID_USER + 0x23E)
#define ID_EDIT_REGISTER_ADDRESS    		(GUI_ID_USER + 0x22F)
#define ID_DROPDOWN_REGISTER_TYPE   		(GUI_ID_USER + 0x230)
#define ID_BUTTON_CONFIRM								(GUI_ID_USER + 0x231)
#define ID_BUTTON_ANNUL									(GUI_ID_USER + 0x232)
#define ID_TEXT_REGISTER_INFO      			(GUI_ID_USER + 0x233)

#define ID_WINDOW_ANALOG_OUTPUT 					(GUI_ID_USER + 0x234)
#define ID_TEXT_ANALOG_OUTPUT_MODE 				(GUI_ID_USER + 0x235)
#define ID_DROPDOWN_ANALOG_OUTPUT_MODE 		(GUI_ID_USER + 0x236)
#define ID_TEXT_ANALOG_OUTPUT_SOURCE 			(GUI_ID_USER + 0x237)
#define ID_DROPDOWN_ANALOG_OUTPUT_SOURCE	(GUI_ID_USER + 0x238)
#define ID_EDIT_ANALOG_OUT_LOW 						(GUI_ID_USER + 0x239)
#define ID_TEXT_ANALOG_OUT_LOW 						(GUI_ID_USER + 0x23A)
#define ID_EDIT_ANALOG_OUT_HIGH 					(GUI_ID_USER + 0x23B)
#define ID_TEXT_ANALOG_OUT_HIGH 					(GUI_ID_USER + 0x23C)
#define ID_TEXT_ANALOG_OUT_FAILURE 				(GUI_ID_USER + 0x23D)
#define ID_DROPDOWN_ANALOG_OUT_FAILURE 		(GUI_ID_USER + 0x23E)
#define ID_EDIT_ANALOG_OUT_FAILURE 				(GUI_ID_USER + 0x23F)
#define ID_TEXT_ANALOG_OUT_TITLE 					(GUI_ID_USER + 0x240)
#define ID_TEXT_ANALOG_OUT_FAILURE_UNIT		(GUI_ID_USER + 0x241)

#define ID_DROPDOWN_TARA			(GUI_ID_USER + 0x242)
#define ID_TEXT_TARA					(GUI_ID_USER + 0x243)

static int ChcekExpansionCardsSettings(EXPANSION_CARD *old, EXPANSION_CARD *new);
static void DeleteIOPages(MULTIPAGE_Handle hObj, uint8_t CardType);
static void RefreshIOPages(WM_HWIN hWinSrc, uint8_t CardType);
static void CreateIOPages(MULTIPAGE_Handle hObj, uint8_t CardType);
static void SetAdjusmentParametersVisibility(WM_HWIN hWin, EXPANSION_CARD *card, uint8_t BoardChannelNo);
static void SetConstantCompensationValueVisibility(WM_HWIN hWin, char state);
static void InitCompensationParametersValues(WM_HWIN hWin, const EXPANSION_CARD *card, uint8_t BoardChannelNo);
static void SetCompensationParametersVisibility(WM_HWIN hWin, uint8_t cardType);
static void SaveSettingsFromIOPages(WM_HWIN hWinSrc, uint8_t CardType);
static void PaintIOBoardDecoration(WM_HWIN hWin);
static WM_HWIN CreateSetRegister(WM_HWIN hParent, int regNo);
static int CheckParameterLimits(EDIT_Handle hObj, int min, int max);
static void LISTVIEW_InitRSchannels(LISTVIEW_Handle hObj, RS_CARD_SETTINGS *rsCard);
static void LISTVIEW_RefreshRSChannel(LISTVIEW_Handle hObj, unsigned Row, RS_CHANNEL_SETTINGS *rsChannel);
static void SendMessageToPage(WM_HWIN hWinSrc, int MsgId, int PageNumber);

static void SetOutputFailureValueVisibility(WM_HWIN hWin, int mode);
static void SetOutputElementsVisibility(WM_HWIN hWin, int mode);
static void DrawOutputDecorations(WM_HWIN hWin);
static void SetOutputScaleDescryptions(WM_HWIN hWin, int mode);

static EXPANSION_CARD tempExpansionCards[7] __attribute__ ((section(".sdram")));
static RS_CHANNEL_SETTINGS tempRSchannelSettings;
static int CurrentInputNo = 0;
static int CurrentBoardNo = 0;
static const char rsChannelTypeDesc[10][20] =
{ "uint (16bit)", "int (16bit)", "uint (32bit)", "uint (32bit) sw", "int (32bit)",
		"int (32bit) sw", "float (32bit)", "float (32bit) sw", "int (64bit)",	"double (64bit)" };

static int tempIndex;

static const GUI_WIDGET_CREATE_INFO _aSetIOBoards[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_IO_BOARDS, 0, 0, 740, 420, 0, 0x0, 0 },
{ MULTIPAGE_CreateIndirect, "", ID_MULTIPAGE_IO, 5, 58, 730, 290, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_SLOT, 10, 10, 200, 280, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CARD_DESC, 220, 10, 155, 35, TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_CARD_TYPE, 390, 10, 340, 35, TEXT_CF_LEFT | TEXT_CF_VCENTER, 0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_OK, 496, 360, 120, 58, 0, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CANCEL, 618, 360, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetIOBoard[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_IO, 0, 0, 730, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_MODE, 10, 10, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_MODE, 210, 10, 240, 160, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ADJUSMENT_DESC, 10, 55, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_ADJUSMENT, 215, 55, 160, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ADJUSMENT_UNIT, 380, 55, 65, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_COMPENSATION_DESC, 10, 100, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_COMPENSATION_CHANNEL, 210, 100, 240, 120, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_COMPENSATION_VALUE, 475, 100, 160, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "°C/°F", ID_TEXT_COMPENSATION_UNIT, 640, 100, 70, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetGeneralIOBoard[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_DEBOUNCE, 0, 0, 730, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_DEBOUNCE, 10, 10, 300, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_DEBOUNCE, 210, 10, 240, 160, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_TARA, 10, 55, 300, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_TARA, 210, 55, 240, 160, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetRS485port[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_RS485_PORT, 0, 0, 730, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RS485_PORT_1, 280, 10, 200, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RS485_PORT_2, 500, 10, 200, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RS485_BAUDRATE, 10, 55, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RS485_PARITY, 10, 100, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RS485_STOPBITS, 10, 145, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RS485_TIMEOUT, 10, 190, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RS485_BAUDRATE_1, 280, 55, 200, 160, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RS485_PARITY_1, 280, 100, 200, 160, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RS485_STOPBITS_1, 280, 145, 200, 160, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_RS485_TIMEOUT_1, 285, 190, 160, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "ms", ID_TEXT_RS485_TIMEOUT_UNIT_1, 445, 190, 30, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RS485_BAUDRATE_2, 500, 55, 200, 160, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RS485_PARITY_2, 500, 100, 200, 160, 0, 0x0, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RS485_STOPBITS_2, 500, 145, 200, 160, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_RS485_TIMEOUT_2, 505, 190, 160, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "ms", ID_TEXT_RS485_TIMEOUT_UNIT_2, 665, 190, 30, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetRS485registers[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_RS485_REGISTERS, 0, 0, 730, 255, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_RS485_REGISTERS, 10, 10, 710, 235, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetRegister[] =
{
{ FRAMEWIN_CreateIndirect, "", FRAMEWIN_REGISTER, 10, 10, 710, 235, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_PORT, 10, 5, 190, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_DEVICE_ADDRESS, 210, 5, 130, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_ADDRESS, 350, 5, 120, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_TYPE, 480, 5, 216, 35, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_REGISTER_PORT, 10, 45, 190, 90, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_DEVICE_ADDRESS, 215, 45, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_REGISTER_ADDRESS, 355, 45, 110, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_REGISTER_TYPE, 480, 45, 216, 90, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_REGISTER_INFO, 25, 90, 450, 80, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_CONFIRM, 462, 171, 120, 58, 2, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_ANNUL, 584, 171, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetAnalogOutputBoard[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_ANALOG_OUTPUT, 0, 0, 730, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ANALOG_OUTPUT_MODE, 10, 10, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ANALOG_OUTPUT_MODE, 210, 10, 240, 160, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ANALOG_OUTPUT_SOURCE, 10, 55, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ANALOG_OUTPUT_SOURCE, 210, 55, 240, 160, 0, 0x0, 0 },

{ EDIT_CreateIndirect, "0.0000", ID_EDIT_ANALOG_OUT_LOW, 115, 100, 170, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 15 },
{ TEXT_CreateIndirect, "→ 4 mA", ID_TEXT_ANALOG_OUT_LOW, 300, 100, 100, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "0.0000", ID_EDIT_ANALOG_OUT_HIGH, 420, 100, 170, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 15 },
{ TEXT_CreateIndirect, "→ 20 mA", ID_TEXT_ANALOG_OUT_HIGH, 605, 100, 100, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

{ TEXT_CreateIndirect, "", ID_TEXT_ANALOG_OUT_FAILURE, 10, 145, 190, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ANALOG_OUT_FAILURE, 210, 145, 180, 60, 0, 0x0, 0 },
{ EDIT_CreateIndirect, "0.0000", ID_EDIT_ANALOG_OUT_FAILURE, 420, 145, 120, 35, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 15 },
{ TEXT_CreateIndirect, "mA", ID_TEXT_ANALOG_OUT_FAILURE_UNIT, 550, 145, 40, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },

};

static const GUI_WIDGET_CREATE_INFO _aSetGeneralHARTBoard[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_IO, 0, 0, 730, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_MODE, 10, 10, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_MODE, 245, 10, 240, 60, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_RESISTOR, 10, 60, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_RESISTOR, 245, 60, 240, 60, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_PREAMBLE, 10, 110, 200, 32, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_PREAMBLE, 250, 109, 173, 32, GUI_TA_RIGHT | GUI_TA_VCENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_UNIT_PREAMBLE, 410, 110, 62, 32, GUI_TA_CENTER, 0x64, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetValuesHARTBoard[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_IO, 0, 0, 730, 255, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_MODE, 10, 10, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_ADDRESSES, 5, 10, 720, 235, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aSetDevicesHARTBoard[] =
{
{ WINDOW_CreateIndirect, "", ID_WINDOW_IO, 0, 0, 730, 255, 0, 0x0, 0 },
{ LISTVIEW_CreateIndirect, "", ID_LISTVIEW_ADDRESSES, 5, 10, 720, 235, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aEditHARTAddresses[] =
{
{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_ADDRESSES, 5, 9, 720, 245, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_ADDRESS_MODE, 10, 10, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_ADDRESS_MODE, 245, 10, 240, 70, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_LONG_ADDR, 10, 60, 200, 32, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_LONG_ADDR, 250, 60, 230, 32, GUI_TA_CENTER, 0x64, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_SHORT_ADDR, 250, 95, 200, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_ADDRESS_CONFIRM, 472, 165, 120, 58, 2, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_ADDRESS_ANNUL, 594, 165, 120, 58, 0, 0x0, 0 }, };

static const GUI_WIDGET_CREATE_INFO _aEditHARTReadValues[] =
{
{ FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_READ_VALUE, 5, 9, 720, 244, 0, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_READ_VALUE_ADDR, 10, 10, 150, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_READ_VALUE, 160, 10, 190, 35, GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_READ_VALUE_TYPE, 10, 70, 150, 35, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_VALUE_TYPE, 160, 70, 190, 35, GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_VARIABLE_ADDR, 420, 70, 120, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ EDIT_CreateIndirect, "", ID_EDIT_VARIABLE_ADDR, 550, 70, 75, 30, GUI_TA_HCENTER | GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_STATUS_VAR, 420, 10, 120, 32, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_STATUS_VAR, 547, 10, 163, 32, GUI_TA_VCENTER, 0x0, 0 },
{ TEXT_CreateIndirect, "", ID_TEXT_COMMAND_01_OR_03, 10, 130, 150, 30, GUI_TA_LEFT | GUI_TA_VCENTER, 0x64, 0 },
{ DROPDOWN_CreateIndirect, "", ID_DROPDOWN_COMMAND_01_OR_03, 160, 130, 80, 60, GUI_TA_VCENTER, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_ADDRESS_CONFIRM, 472, 165, 120, 57, 2, 0x0, 0 },
{ BUTTON_CreateIndirect, "", ID_BUTTON_ADDRESS_ANNUL, 594, 165, 120, 57, 0, 0x0, 0 }, };

static void _cbSetIOBoards(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	int i;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		for (i = 0; i < 7; i++)
			tempExpansionCards[i] = bkExpansionCards[i];

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_DESC);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(86));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_SLOT);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, "A");
		DROPDOWN_AddString(hItem, "B");
		DROPDOWN_AddString(hItem, "C");
		DROPDOWN_AddString(hItem, "D");
		DROPDOWN_AddString(hItem, "E");
		DROPDOWN_AddString(hItem, "F");
		DROPDOWN_AddString(hItem, "G");
		CurrentBoardNo = 0;
		DROPDOWN_SetSel(hItem, CurrentBoardNo);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE);
		TEXT_SetText(hItem, GetExpansionCardDescription(tempExpansionCards[CurrentBoardNo].type));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_IO);
		CreateIOPages(hItem, tempExpansionCards[CurrentBoardNo].type);
		RefreshIOPages(pMsg->hWin, tempExpansionCards[CurrentBoardNo].type);
		CurrentInputNo = 0;
		MULTIPAGE_SelectPage(hItem, CurrentInputNo);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_SLOT:
			switch (NCode)
			{
			case WM_NOTIFICATION_LOST_FOCUS:
				DROPDOWN_Collapse(pMsg->hWinSrc);
				break;

			case WM_NOTIFICATION_SEL_CHANGED:
				SaveSettingsFromIOPages(pMsg->hWin, tempExpansionCards[CurrentBoardNo].type);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_IO);
				DeleteIOPages(hItem, tempExpansionCards[CurrentBoardNo].type);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CARD_TYPE);
				CurrentBoardNo = DROPDOWN_GetSel(pMsg->hWinSrc);
				TEXT_SetText(hItem, GetExpansionCardDescription(tempExpansionCards[CurrentBoardNo].type));

				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_IO);
				CreateIOPages(hItem, tempExpansionCards[CurrentBoardNo].type);
				RefreshIOPages(pMsg->hWin, tempExpansionCards[CurrentBoardNo].type);
				break;
			}
			break;
		case ID_BUTTON_OK:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				SaveSettingsFromIOPages(pMsg->hWin, tempExpansionCards[CurrentBoardNo].type);
				for (i = 0; i < 7; i++)
				{
					if (ChcekExpansionCardsSettings(&bkExpansionCards[i], &tempExpansionCards[i]))
						bkExpansionCards[i] = tempExpansionCards[i];
				}
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;

		case ID_MULTIPAGE_IO:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_IO);
				CurrentInputNo = MULTIPAGE_GetSelection(hItem);
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
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbSetIOBoard(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(87));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ADJUSMENT_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(151));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ADJUSMENT_UNIT);
		TEXT_SetText(hItem, "");
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ADJUSMENT);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COMPENSATION_DESC);
		TEXT_SetText(hItem, GUI_LANG_GetText(152));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_COMPENSATION_CHANNEL);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(110));
		for (int j = 1; j <= 100; j++)
		{
			mini_snprintf(GUITextBuffer, 50, "%s %d", GUI_LANG_GetText(6), j);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COMPENSATION_VALUE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COMPENSATION_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);
		break;

	case WM_POST_PAINT:
		PaintIOBoardDecoration(pMsg->hWin);
		break;

	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[pMsg->Data.v].type = DROPDOWN_GetSel(hItem);

		if (tempExpansionCards[CurrentBoardNo].type != SPI_CARD_OUT6RL && tempExpansionCards[CurrentBoardNo].type != SPI_CARD_IN6D)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ADJUSMENT);
			EDIT_GetText(hItem, GUITextBuffer, 10);
			tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[pMsg->Data.v].adjusment = atof(GUITextBuffer);

			if(tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN4SG)
			{
				if(3==pMsg->Data.v)
					tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[4].adjusment = tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[0].adjusment +
																																										 tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[1].adjusment +
																																										 tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[2].adjusment +
																																										 tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[3].adjusment;
			}
		}

		if (tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN6TC || tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN3)
		{
			hItem = WM_GetDialogItem(pMsg->hWin,
			ID_DROPDOWN_COMPENSATION_CHANNEL);
			tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[pMsg->Data.v].compChannel = DROPDOWN_GetSel(hItem) - 2;
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_COMPENSATION_VALUE);
			EDIT_GetText(hItem, GUITextBuffer, 10);
			tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[pMsg->Data.v].compensationConstantValue = atof(GUITextBuffer);
		}
		break;

	case WM_USER_REFRESH:
		WM_InvalidateWindow(pMsg->hWin);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		FillInputModeList(hItem, tempExpansionCards[CurrentBoardNo].type);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.IOcard.channels[pMsg->Data.v].type);
		SetAdjusmentParametersVisibility(pMsg->hWin, &tempExpansionCards[CurrentBoardNo], pMsg->Data.v);
		InitCompensationParametersValues(pMsg->hWin, &tempExpansionCards[CurrentBoardNo], pMsg->Data.v);
		SetCompensationParametersVisibility(pMsg->hWin, tempExpansionCards[CurrentBoardNo].type);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_ADJUSMENT:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_EDIT_COMPENSATION_VALUE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 0);
				break;
			}
			break;
		case ID_DROPDOWN_COMPENSATION_CHANNEL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);
				if (DROPDOWN_GetSel(pMsg->hWinSrc) == 1 && WM_IsVisible(pMsg->hWinSrc))
					SetConstantCompensationValueVisibility(pMsg->hWin, 1);
				else
					SetConstantCompensationValueVisibility(pMsg->hWin, 0);
				break;
			}
			break;
		case ID_DROPDOWN_MODE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				SetAdjusmentParametersVisibility(pMsg->hWin, &tempExpansionCards[CurrentBoardNo], CurrentInputNo);
				SetCompensationParametersVisibility(pMsg->hWin, tempExpansionCards[CurrentBoardNo].type);
				WM_InvalidateWindow(pMsg->hWin);
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

static void _cbSetGeneralHARTBoard(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(221));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		DROPDOWN_AddString(hItem, "Primary");
		DROPDOWN_AddString(hItem, "Secondary");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RESISTOR);
		TEXT_SetText(hItem, GUI_LANG_GetText(222));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RESISTOR);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_PREAMBLE);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(223));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_PREAMBLE);
		mini_snprintf(GUITextBuffer, 3, "%d", tempExpansionCards[CurrentBoardNo].settings.HARTcard.preambleLenght);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_UNIT_PREAMBLE);
		TEXT_SetText(hItem, "B");
		TEXT_SetBkColor(hItem, GUI_TRANSPARENT);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		break;

	case WM_POST_PAINT:
		DRAW_RoundedFrame(245, 105, 240, 38, 4, 2, GUI_BLUE);
		break;

	case WM_USER_REFRESH:
		WM_InvalidateWindow(pMsg->hWin);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		DROPDOWN_AddString(hItem, "Primary");
		DROPDOWN_AddString(hItem, "Secondary");
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.isSecondary);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RESISTOR);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.isResistor);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_PREAMBLE);
		mini_snprintf(GUITextBuffer, 3, "%d", tempExpansionCards[CurrentBoardNo].settings.HARTcard.preambleLenght);
		EDIT_SetText(hItem, GUITextBuffer);
		break;

	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_MODE);
		tempExpansionCards[CurrentBoardNo].settings.HARTcard.isSecondary = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RESISTOR);
		tempExpansionCards[CurrentBoardNo].settings.HARTcard.isResistor = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_PREAMBLE);
		EDIT_GetText(hItem, GUITextBuffer, 3);
		if ((atoi(GUITextBuffer) > 0) && (atoi(GUITextBuffer) > 4) && (atoi(GUITextBuffer) > 0) && (atoi(GUITextBuffer) < 21))
			tempExpansionCards[CurrentBoardNo].settings.HARTcard.preambleLenght = atoi(GUITextBuffer);
		else
			tempExpansionCards[CurrentBoardNo].settings.HARTcard.preambleLenght = 5;

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_MODE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;

		case ID_DROPDOWN_RESISTOR:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_EDIT_PREAMBLE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 2);
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

static int StringToHartAddr(char *buf, uint64_t *addr)
{
	uint64_t val;

	for (int i = 0; i < strlen(buf); ++i)
	{
		if (((buf[i] > 0x2F) && (buf[i] < 0x3A)) || ((buf[i] > 0x40) && (buf[i] < 0x47)) || ((buf[i] > 0x60) && (buf[i] < 0x67)))
			;
		else
			return 0;
	}

	val = strtoull(buf, NULL, 16);

	if (buf[1] == 0)
	{
		if ((buf[0] > 0x2F) && (buf[0] < 0x3A)) 	//adres krotki
		{
			*addr = 0x0F & buf[0];
			return 1;
		}
		else
			return 0;
	}

	if ((buf[0] == 0x31) && (buf[2] == 0))
	{
		if ((buf[1] > 0x2F) && (buf[1] < 0x36))	 //adres krotki
		{
			*addr = 10 + (0x0F & buf[1]);
			return 1;
		}
		else	//adres dlugi
		{
			*addr = val;
			return 1;
		}
	}
	else	//adres dlugi
	{
		*addr = val;
		if (val <= 0x3fffffffff)
			return 1;
		else
			return 0;
	}
}

static void HartAddrTostring(uint64_t addr, char *buf)
{
	uint8_t bufHex[5]={0};
	if (addr < 16)
	{
		mini_snprintf(buf, 3, "%d", addr);
	}
	else
	{
		bufHex[0] = (addr >> (4 * 8)) & 0xff;
		bufHex[1] = (addr >> (3 * 8)) & 0xff;
		bufHex[2] = (addr >> (2 * 8)) & 0xff;
		bufHex[3] = (addr >> (1 * 8)) & 0xff;
		bufHex[4] = (addr >> (0 * 8)) & 0xff;
		mini_snprintf(buf, 11, "%02x%02x%02x%02x%02x", bufHex[0], bufHex[1], bufHex[2], bufHex[3], bufHex[4]);
	}
}

static void _cbSetGeneralIOBoard(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[11];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DEBOUNCE);
		if(tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN4SG)
			TEXT_SetText(hItem, GUI_LANG_GetText(276));
		else
			TEXT_SetText(hItem, GUI_LANG_GetText(184));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DEBOUNCE);
		DROPDOWN_SetTextSkin(hItem);
		if(tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN4SG)
		{
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));
		}
		else
		{
			DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
			DROPDOWN_AddString(hItem, "1 ms");
			DROPDOWN_AddString(hItem, "3 ms");
		}
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.IOcard.generalSet&0x0f);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TARA);
		mini_snprintf(GUITextBuffer, 11, "%s",GUI_LANG_GetText(279));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		if(tempExpansionCards[CurrentBoardNo].type != SPI_CARD_IN4SG)
			WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TARA);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(277));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(278));
		DROPDOWN_SetSel(hItem, (tempExpansionCards[CurrentBoardNo].settings.IOcard.generalSet>>4)&0x0f);
		if(tempExpansionCards[CurrentBoardNo].type != SPI_CARD_IN4SG)
			WM_HideWindow(hItem);
		break;

	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DEBOUNCE);
		tempExpansionCards[CurrentBoardNo].settings.IOcard.generalSet = DROPDOWN_GetSel(hItem);

		if(tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN4SG)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TARA);
			tempExpansionCards[CurrentBoardNo].settings.IOcard.generalSet |= (DROPDOWN_GetSel(hItem)<<4)&0xf0;
		}
		break;

	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_DEBOUNCE);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.IOcard.generalSet&0x0f);

		if(tempExpansionCards[CurrentBoardNo].type == SPI_CARD_IN4SG)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_TARA);
			DROPDOWN_SetSel(hItem, (tempExpansionCards[CurrentBoardNo].settings.IOcard.generalSet>>4)&0x0f);
		}
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_DEBOUNCE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;

		case ID_DROPDOWN_TARA:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
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

static void DROPDOWN_AddRSPortStrings(DROPDOWN_Handle hObj)
{

	DROPDOWN_AddString(hObj, GUI_LANG_GetText(27));
	DROPDOWN_AddString(hObj, "1200");
	DROPDOWN_AddString(hObj, "2400");
	DROPDOWN_AddString(hObj, "4800");
	DROPDOWN_AddString(hObj, "9600");
	DROPDOWN_AddString(hObj, "19200");
	DROPDOWN_AddString(hObj, "38400");
	DROPDOWN_AddString(hObj, "57600");
	DROPDOWN_AddString(hObj, "115200");
}

static void DROPDOWN_AddRSParityStrings(DROPDOWN_Handle hObj)
{
	DROPDOWN_AddString(hObj, "ODD");
	DROPDOWN_AddString(hObj, "EVEN");
	DROPDOWN_AddString(hObj, "NONE");
}

static void DROPDOWN_AddRSstopBitsStrings(DROPDOWN_Handle hObj)
{
	DROPDOWN_AddString(hObj, "1b");
	DROPDOWN_AddString(hObj, "2b");
}

static void _cbSetRS485port(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[10];
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RS485_PORT_1);
		mini_snprintf(GUITextBuffer, 10, "%s 1", GUI_LANG_GetText(188));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RS485_PORT_2);
		mini_snprintf(GUITextBuffer, 10, "%s 2", GUI_LANG_GetText(188));
		TEXT_SetText(hItem, GUITextBuffer);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RS485_BAUDRATE);
		TEXT_SetText(hItem, GUI_LANG_GetText(70));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_BAUDRATE_1);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddRSPortStrings(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].baudrate);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_BAUDRATE_2);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddRSPortStrings(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].baudrate);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RS485_PARITY);
		TEXT_SetText(hItem, GUI_LANG_GetText(71));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_PARITY_1);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddRSParityStrings(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].parity);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_PARITY_2);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddRSParityStrings(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].parity);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RS485_STOPBITS);
		TEXT_SetText(hItem, GUI_LANG_GetText(189));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_STOPBITS_1);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddRSstopBitsStrings(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].stopBits);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_STOPBITS_2);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddRSstopBitsStrings(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].stopBits);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_RS485_TIMEOUT);
		TEXT_SetText(hItem, GUI_LANG_GetText(73));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_RS485_TIMEOUT_1);
		itoa(tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].timeout, GUITextBuffer, 10);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_RS485_TIMEOUT_2);
		itoa(tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].timeout, GUITextBuffer, 10);
		EDIT_SetText(hItem, GUITextBuffer);

		break;

	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_BAUDRATE_1);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].baudrate = DROPDOWN_GetSel(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_BAUDRATE_2);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].baudrate = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_PARITY_1);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].parity = DROPDOWN_GetSel(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_PARITY_2);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].parity = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_STOPBITS_1);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].stopBits = DROPDOWN_GetSel(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_RS485_STOPBITS_2);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].stopBits = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_RS485_TIMEOUT_1);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[0].timeout = atoi(GUITextBuffer);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_RS485_TIMEOUT_2);
		EDIT_GetText(hItem, GUITextBuffer, 10);
		tempExpansionCards[CurrentBoardNo].settings.RScard.port[1].timeout = atoi(GUITextBuffer);
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(280, 187, 200, 39, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(500, 187, 200, 39, 4, 2, GUI_BLUE);
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_DROPDOWN_RS485_BAUDRATE_1:
		case ID_DROPDOWN_RS485_BAUDRATE_2:
		case ID_DROPDOWN_RS485_PARITY_1:
		case ID_DROPDOWN_RS485_PARITY_2:
		case ID_DROPDOWN_RS485_STOPBITS_1:
		case ID_DROPDOWN_RS485_STOPBITS_2:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			}
			break;
		case ID_EDIT_RS485_TIMEOUT_1:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 6);
				break;
			}
			break;
		case ID_EDIT_RS485_TIMEOUT_2:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 6);
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

static void _cbSetRS485registers(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_RS485_REGISTERS);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		LISTVIEW_AddColumn(hItem, 40, "IO", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 150, GUI_LANG_GetText(188),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 150, GUI_LANG_GetText(185),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 150, GUI_LANG_GetText(186),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 180, GUI_LANG_GetText(105),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 47);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_GRAY);
		LISTVIEW_InitRSchannels(hItem, &tempExpansionCards[CurrentBoardNo].settings.RScard);
		break;
	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_RS485_REGISTERS);
		unsigned row = LISTVIEW_GetSel(hItem);
		LISTVIEW_RefreshRSChannel(hItem, row, &tempExpansionCards[CurrentBoardNo].settings.RScard.channels[row]);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTVIEW_RS485_REGISTERS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				CreateSetRegister(pMsg->hWin, LISTVIEW_GetSel(pMsg->hWinSrc));
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

static void _cbSetRegister(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[10];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;
		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_LIGHTBLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetTitleHeight(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_PORT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		TEXT_SetText(hItem, GUI_LANG_GetText(188));
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_DEVICE_ADDRESS);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		TEXT_SetText(hItem, GUI_LANG_GetText(185));
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_ADDRESS);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		TEXT_SetText(hItem, GUI_LANG_GetText(186));
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_TYPE);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, GUI_LANG_GetText(105));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_PORT);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, "1");
		DROPDOWN_AddString(hItem, "2");
		DROPDOWN_SetSel(hItem, tempRSchannelSettings.port);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ADDRESS);
		itoa(tempRSchannelSettings.deviceAdr, GUITextBuffer, 10);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_REGISTER_ADDRESS);
		itoa(tempRSchannelSettings.registerAdr, GUITextBuffer, 10);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_TYPE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for (int i = 0; i < 10; i++)
			DROPDOWN_AddString(hItem, rsChannelTypeDesc[i]);
		DROPDOWN_SetSel(hItem, tempRSchannelSettings.registerType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_REGISTER_INFO);
		TEXT_SetTextColor(hItem, GUI_BLUE);
		TEXT_SetText(hItem, "300000-365535 - Input Registers\n400000-465535 - Holding Registers");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CONFIRM);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ANNUL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_CONFIRM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_PORT);
				tempRSchannelSettings.port = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ADDRESS);
				EDIT_GetText(hItem, GUITextBuffer, 4);
				tempRSchannelSettings.deviceAdr = atoi(GUITextBuffer);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_REGISTER_ADDRESS);
				EDIT_GetText(hItem, GUITextBuffer, 7);
				tempRSchannelSettings.registerAdr = atoi(GUITextBuffer);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_REGISTER_TYPE);
				tempRSchannelSettings.registerType = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DEVICE_ADDRESS);
				if (0 == CheckParameterLimits(hItem, 1, 247))
					break;
				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_REGISTER_ADDRESS);
				if (0 == CheckParameterLimits(hItem, 30000, 49999) && 0 == CheckParameterLimits(hItem, 300000, 365535)
						&& 0 == CheckParameterLimits(hItem, 400000, 465535))
					break;

				tempExpansionCards[CurrentBoardNo].settings.RScard.channels[CurrentInputNo] = tempRSchannelSettings;

				UserMessage.MsgId = WM_USER_REFRESH;
				UserMessage.hWinSrc = pMsg->hWin;
				UserMessage.Data.v = 0;
				WM_SendToParent(pMsg->hWin, &UserMessage);

				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_BUTTON_ANNUL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_EDIT_DEVICE_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 3);
				break;
			}
			break;
		case ID_EDIT_REGISTER_ADDRESS:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 6);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(210, 43, 129, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(350, 43, 119, 38, 4, 2, GUI_BLUE);
		break;
	}
}

static void _cbSetAnalogOutputBoard(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ANALOG_OUTPUT_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(87));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUTPUT_MODE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, "0-20 mA");
		DROPDOWN_AddString(hItem, "4-20 mA");
		DROPDOWN_AddString(hItem, "0-24 mA");
		DROPDOWN_AddString(hItem, "0-5 V");
		DROPDOWN_AddString(hItem, "0-10 V");
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[0].type);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ANALOG_OUTPUT_SOURCE);
		TEXT_SetText(hItem, GUI_LANG_GetText(196));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUTPUT_SOURCE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		for (int j = 1; j <= 100; j++)
		{
			mini_snprintf(GUITextBuffer, 30, "%s %d", GUI_LANG_GetText(6), j);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[0].source);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_LOW);
		float2stri(GUITextBuffer, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[0].lowScaleValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_HIGH);
		float2stri(GUITextBuffer, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[0].highScaleValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ANALOG_OUT_FAILURE);
		TEXT_SetText(hItem, GUI_LANG_GetText(112));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUT_FAILURE);
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(197));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(110));
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[0].failureMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_FAILURE);
		float2stri(GUITextBuffer, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[0].failureValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetTextColor(hItem, GUI_BLUE);

		break;

	case WM_POST_PAINT:
		DrawOutputDecorations(pMsg->hWin);
		break;

	case WM_USER_SAVE_TEMP:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUTPUT_MODE);
		tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].type = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUTPUT_SOURCE);
		tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].source = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUT_FAILURE);
		tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].failureMode = DROPDOWN_GetSel(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_FAILURE);
		EDIT_GetText(hItem, GUITextBuffer, 20);
		tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].failureValue = atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_LOW);
		EDIT_GetText(hItem, GUITextBuffer, 20);
		tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].lowScaleValue = atof(GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_HIGH);
		EDIT_GetText(hItem, GUITextBuffer, 20);
		tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].highScaleValue = atof(GUITextBuffer);

		break;

	case WM_USER_REFRESH:
		WM_InvalidateWindow(pMsg->hWin);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUTPUT_MODE);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].type);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUTPUT_SOURCE);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].source);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_LOW);
		float2stri(GUITextBuffer, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].lowScaleValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_HIGH);
		float2stri(GUITextBuffer, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].highScaleValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ANALOG_OUT_FAILURE);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].failureMode);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_ANALOG_OUT_FAILURE);
		float2stri(GUITextBuffer, tempExpansionCards[CurrentBoardNo].settings.OUTcard.channels[pMsg->Data.v].failureValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);

		SetOutputElementsVisibility(pMsg->hWin, DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin,
		ID_DROPDOWN_ANALOG_OUTPUT_MODE)));

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_EDIT_ANALOG_OUT_LOW:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_ANALOG_OUT_HIGH:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_EDIT_ANALOG_OUT_FAILURE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 20);
				break;
			}
			break;
		case ID_DROPDOWN_ANALOG_OUTPUT_MODE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);
				SetOutputElementsVisibility(pMsg->hWin, DROPDOWN_GetSel(pMsg->hWinSrc));
				SetOutputScaleDescryptions(pMsg->hWin, DROPDOWN_GetSel(pMsg->hWinSrc));
				break;
			}
			break;
		case ID_DROPDOWN_ANALOG_OUT_FAILURE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				WM_InvalidateWindow(pMsg->hWin);
				SetOutputFailureValueVisibility(pMsg->hWin, DROPDOWN_GetSel(pMsg->hWinSrc));
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

static void _cbEditHARTAddresses(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[20];
	int id_txt, Idx;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;

		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_LIGHTBLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetTitleHeight(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ADDRESS_MODE);
		TEXT_SetText(hItem, GUI_LANG_GetText(39));
		TEXT_SetTextColor(hItem, GUI_BLUE);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ADDRESS_MODE);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[tempIndex].activity);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LONG_ADDR);
		TEXT_SetText(hItem, GUI_LANG_GetText(186));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LONG_ADDR);
		HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[tempIndex].address, GUITextBuffer);
		EDIT_SetText(hItem, GUITextBuffer);
		EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_BLACK);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADDRESS_CONFIRM);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADDRESS_ANNUL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_ADDRESS_CONFIRM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_ADDRESS_MODE);
				tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[tempIndex].activity = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LONG_ADDR);
				memset(GUITextBuffer, 0, 20);
				EDIT_GetText(hItem, GUITextBuffer, 11);
				id_txt = StringToHartAddr(GUITextBuffer, &tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[tempIndex].address);
				if (0 < id_txt)
				{
					UserMessage.MsgId = WM_USER_REFRESH;
					UserMessage.hWinSrc = pMsg->hWin;
					UserMessage.Data.v = 0;
					WM_SendToParent(pMsg->hWin, &UserMessage);
					hItem=WM_GetDialogItem(MULTIPAGE_GetWindow(WM_GetParent(WM_GetParent(WM_GetParent(pMsg->hWin))),2),ID_LISTVIEW_ADDRESSES);
					for (int i = 0; i < MAX_HART_SENSOR_FOR_CARD; i++)
					{
						Idx = tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].sensorIdx;
						if(Idx==0)
							strcpy(GUITextBuffer,GUI_LANG_GetText(227));
						else
							HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[Idx-1].address, GUITextBuffer);
						LISTVIEW_SetItemText(hItem, 1, i, GUITextBuffer);
					}
					WM_DeleteWindow(pMsg->hWin);
				}
				else
				{
					EDIT_SetText(hItem, GUITextBuffer);
					EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
				}

				break;
			}
			break;
		case ID_BUTTON_ADDRESS_ANNUL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_EDIT_LONG_ADDR:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				setKeyboard(pMsg->hWinSrc, EDIT, PASSOFF, 10);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		DRAW_RoundedFrame(245, 55, 240, 38, 4, 2, GUI_BLUE);
		break;
	default:
		break;
	}
}

static WM_HWIN CreateEditHARTAddresses(WM_HWIN hParent, int regNo)
{
	WM_HWIN hWin;
	tempIndex = regNo;
	hWin = GUI_CreateDialogBox(_aEditHARTAddresses, GUI_COUNTOF(_aEditHARTAddresses), _cbEditHARTAddresses, hParent, 0, 0);
	return hWin;
}

static void _cbSetDevicesHARTBoard(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	char GUITextBuffer[30];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ADDRESSES);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		LISTVIEW_AddColumn(hItem, 400, GUI_LANG_GetText(39),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 130, GUI_LANG_GetText(186),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 47);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_GRAY);

		for (int i = 0; i < MAX_HART_SENSOR_FOR_CARD; i++)
		{
			LISTVIEW_AddRow(hItem, NULL);
			if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[i].activity == 0)
				mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(27));
			else
				mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(154));
			LISTVIEW_SetItemText(hItem, 0, i, GUITextBuffer);
			HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[i].address, GUITextBuffer);
			LISTVIEW_SetItemText(hItem, 1, i, GUITextBuffer);
		}
		break;

	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ADDRESSES);
		unsigned row = LISTVIEW_GetSel(hItem);

		if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[row].activity == 0)
			mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(27));
		else
			mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(154));
		LISTVIEW_SetItemText(hItem, 0, row, GUITextBuffer);

		HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[row].address, GUITextBuffer);
		LISTVIEW_SetItemText(hItem, 1, row, GUITextBuffer);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTVIEW_ADDRESSES:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				CreateEditHARTAddresses(pMsg->hWin, LISTVIEW_GetSel(pMsg->hWinSrc));
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

static void _cbEditHARTReadValues(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;
	char GUITextBuffer[60];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = pMsg->hWin;

		FRAMEWIN_SetFont(hItem, &GUI_FontLato30);
		FRAMEWIN_SetBarColor(hItem, 1, GUI_LIGHTBLUE);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetTitleHeight(hItem, 0);
		FRAMEWIN_SetBorderSize(hItem, 1);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_READ_VALUE_ADDR);
		TEXT_SetText(hItem, GUI_LANG_GetText(231));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_READ_VALUE);

		DROPDOWN_SetListHeight(hItem, 180);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(227));
		for (int i = 0; i < MAX_HART_SENSOR_FOR_CARD; i++)
		{
			HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[i].address, &GUITextBuffer[30]);
			mini_snprintf(GUITextBuffer, 14, "%s", &GUITextBuffer[30]);
			DROPDOWN_AddString(hItem, GUITextBuffer);
		}
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].sensorIdx);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_READ_VALUE_TYPE);
		TEXT_SetText(hItem, GUI_LANG_GetText(105));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
		DROPDOWN_SetListHeight(hItem, 110);
		DROPDOWN_AddString(hItem, "PV");
		DROPDOWN_AddString(hItem, "SV");
		DROPDOWN_AddString(hItem, "TV");
		DROPDOWN_AddString(hItem, "FV");
		DROPDOWN_AddString(hItem, "DVC");
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].valueType);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VARIABLE_ADDR);
		TEXT_SetText(hItem, GUI_LANG_GetText(232));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
		mini_snprintf(GUITextBuffer, 4, "%d", tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].valueAddr);
		EDIT_SetText(hItem, GUITextBuffer);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_STATUS_VAR);
		TEXT_SetText(hItem, GUI_LANG_GetText(233));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR);
		DROPDOWN_SetListHeight(hItem, 60);
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].statusMode);
		if(4==tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].valueType)
			WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR));

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COMMAND_01_OR_03);
		TEXT_SetText(hItem, GUI_LANG_GetText(234));
		TEXT_SetTextColor(hItem, GUI_BLUE);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_COMMAND_01_OR_03);
		DROPDOWN_SetListHeight(hItem, 60);
		DROPDOWN_AddString(hItem, "01");
		DROPDOWN_AddString(hItem, "03");
		DROPDOWN_SetTextSkin(hItem);
		DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].commandType);
		WM_HideWindow(hItem);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
		if (4 == DROPDOWN_GetSel(hItem))
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VARIABLE_ADDR);
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
			WM_ShowWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VARIABLE_ADDR);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
			WM_HideWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
		if (0 == DROPDOWN_GetSel(hItem))
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COMMAND_01_OR_03);
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_COMMAND_01_OR_03);
			WM_ShowWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_COMMAND_01_OR_03);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_COMMAND_01_OR_03);
			WM_HideWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADDRESS_CONFIRM);
		BUTTON_SetBitmapEx(hItem, 0, &bmOK, 41, 16);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADDRESS_ANNUL);
		BUTTON_SetBitmapEx(hItem, 0, &bmNO, 47, 16);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_BUTTON_ADDRESS_CONFIRM:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_READ_VALUE);
				tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].sensorIdx = DROPDOWN_GetSel(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
				tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].valueType = DROPDOWN_GetSel(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR);
				tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].statusMode = DROPDOWN_GetSel(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin,
				ID_DROPDOWN_COMMAND_01_OR_03);
				tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].commandType = DROPDOWN_GetSel(hItem);

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
				if (4 == DROPDOWN_GetSel(hItem))
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
					EDIT_GetText(hItem, GUITextBuffer, 4);
					if (strstr(GUITextBuffer, ".") || strstr(GUITextBuffer, "-") || (atoi(GUITextBuffer) > 255))
					{
						EDIT_SetText(hItem, GUITextBuffer);
						EDIT_SetTextColor(hItem, EDIT_CI_ENABLED, GUI_RED);
					}
					else
					{
						tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].valueAddr = atoi(GUITextBuffer);
						UserMessage.MsgId = WM_USER_REFRESH;
						UserMessage.hWinSrc = pMsg->hWin;
						UserMessage.Data.v = 0;
						WM_SendToParent(pMsg->hWin, &UserMessage);
						WM_DeleteWindow(pMsg->hWin);
					}
				}
				else
				{
					UserMessage.MsgId = WM_USER_REFRESH;
					UserMessage.hWinSrc = pMsg->hWin;
					UserMessage.Data.v = 0;
					WM_SendToParent(pMsg->hWin, &UserMessage);
					WM_DeleteWindow(pMsg->hWin);
				}
				break;
			}
			break;
		case ID_BUTTON_ADDRESS_ANNUL:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				WM_DeleteWindow(pMsg->hWin);
				break;
			}
			break;
		case ID_EDIT_VARIABLE_ADDR:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				setNumKeyboard(pMsg->hWinSrc, EDIT, 3);
				break;
			}
			break;
		case ID_DROPDOWN_VALUE_TYPE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
				if (4 == DROPDOWN_GetSel(hItem))
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VARIABLE_ADDR);
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR);
					tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].statusMode=0;
					DROPDOWN_SetSel(hItem, tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[tempIndex].statusMode);
					WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR));
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_VARIABLE_ADDR);
					WM_HideWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
					WM_HideWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR);
					WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_STATUS_VAR));
				}

				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_VALUE_TYPE);
				if (0 == DROPDOWN_GetSel(hItem))
				{
					hItem = WM_GetDialogItem(pMsg->hWin,
					ID_TEXT_COMMAND_01_OR_03);
					WM_ShowWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin,
					ID_DROPDOWN_COMMAND_01_OR_03);
					WM_ShowWindow(hItem);
				}
				else
				{
					hItem = WM_GetDialogItem(pMsg->hWin,
					ID_TEXT_COMMAND_01_OR_03);
					WM_HideWindow(hItem);
					hItem = WM_GetDialogItem(pMsg->hWin,
					ID_DROPDOWN_COMMAND_01_OR_03);
					WM_HideWindow(hItem);
				}
				WM_InvalidateWindow(pMsg->hWin);
				break;
			}
			break;
		}
		break;
	case WM_POST_PAINT:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_VARIABLE_ADDR);
		if (WM_IsVisible(hItem))
			DRAW_RoundedFrame(548, 65, 82, 38, 4, 2, GUI_BLUE);
		break;
	}
}

static WM_HWIN CreateEditHARTReadValues(WM_HWIN hParent, int regNo)
{
	WM_HWIN hWin;
	tempIndex = regNo;
	hWin = GUI_CreateDialogBox(_aEditHARTReadValues, GUI_COUNTOF(_aEditHARTReadValues), _cbEditHARTReadValues, hParent, 0, 0);
	return hWin;
}

static void _cbSetValuesHARTBoard(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id, Idx;

	char GUITextBuffer[40];

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ADDRESSES);
		LISTVIEW_SetHeaderHeight(hItem, 40);
		LISTVIEW_AddColumn(hItem, 53, "#", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 153, GUI_LANG_GetText(231),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 103, GUI_LANG_GetText(105),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 103, GUI_LANG_GetText(232),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 118, GUI_LANG_GetText(234),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 138, GUI_LANG_GetText(233),
		GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 47);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL, GUI_GRAY);
		LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS, GUI_GRAY);

		for (int i = 0; i < 25; i++)
		{
			LISTVIEW_AddRow(hItem, NULL);

			mini_snprintf(GUITextBuffer, 8, "%d", i + 1);
			LISTVIEW_SetItemText(hItem, 0, i, GUITextBuffer);
			Idx = tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].sensorIdx;
			if (Idx == 0)
				mini_snprintf(GUITextBuffer, 25, GUI_LANG_GetText(227));
			else
				HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[Idx - 1].address, GUITextBuffer);
			LISTVIEW_SetItemText(hItem, 1, i, GUITextBuffer);

			switch (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].valueType)
			{
			case 0:
				mini_snprintf(GUITextBuffer, 10, "PV");
				break;
			case 1:
				mini_snprintf(GUITextBuffer, 10, "SV");
				break;
			case 2:
				mini_snprintf(GUITextBuffer, 10, "TV");
				break;
			case 3:
				mini_snprintf(GUITextBuffer, 10, "FV");
				break;
			case 4:
				mini_snprintf(GUITextBuffer, 10, "DVC");
				break;
			}
			LISTVIEW_SetItemText(hItem, 2, i, GUITextBuffer);

			if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].valueType == 4)
				mini_snprintf(GUITextBuffer, 4, "%d", tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].valueAddr);
			else
				mini_snprintf(GUITextBuffer, 4, "--");
			LISTVIEW_SetItemText(hItem, 3, i, GUITextBuffer);

			if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].valueType == 4)
				mini_snprintf(GUITextBuffer, 20, "09");
			else if ((tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].valueType > 0)
					&& (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].valueType < 4))
				mini_snprintf(GUITextBuffer, 20, "03");
			else
			{
				if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].commandType == 0)
					mini_snprintf(GUITextBuffer, 20, "01");
				else
					mini_snprintf(GUITextBuffer, 20, "03");
			}
			LISTVIEW_SetItemText(hItem, 4, i, GUITextBuffer);

			if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].statusMode == 0)
				mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(27));
			else if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[i].statusMode == 1)
				mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(154));
			LISTVIEW_SetItemText(hItem, 5, i, GUITextBuffer);
		}
		break;

	case WM_USER_REFRESH:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_ADDRESSES);
		unsigned row = LISTVIEW_GetSel(hItem);

		Idx = tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].sensorIdx;
		if (Idx == 0)
			mini_snprintf(GUITextBuffer, 25, GUI_LANG_GetText(227));
		else
			HartAddrTostring(tempExpansionCards[CurrentBoardNo].settings.HARTcard.sensors[Idx - 1].address, GUITextBuffer);

		LISTVIEW_SetItemText(hItem, 1, row, GUITextBuffer);

		switch (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueType)
		{
		case 0:
			mini_snprintf(GUITextBuffer, 10, "PV");
			break;
		case 1:
			mini_snprintf(GUITextBuffer, 10, "SV");
			break;
		case 2:
			mini_snprintf(GUITextBuffer, 10, "TV");
			break;
		case 3:
			mini_snprintf(GUITextBuffer, 10, "FV");
			break;
		case 4:
			mini_snprintf(GUITextBuffer, 10, "DVC");
			break;
		}
		LISTVIEW_SetItemText(hItem, 2, row, GUITextBuffer);

		if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueType == 4)
			mini_snprintf(GUITextBuffer, 4, "%d", tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueAddr);
		else
		{
			mini_snprintf(GUITextBuffer, 4, "--");
			tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueAddr = 0;

		}
		LISTVIEW_SetItemText(hItem, 3, row, GUITextBuffer);

		if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueType == 4)
			mini_snprintf(GUITextBuffer, 20, "09");
		else if ((tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueType > 0)
				&& (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].valueType < 4))
			mini_snprintf(GUITextBuffer, 20, "03");
		else
		{
			if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].commandType == 0)
				mini_snprintf(GUITextBuffer, 20, "01");
			else
				mini_snprintf(GUITextBuffer, 20, "03");
		}
		LISTVIEW_SetItemText(hItem, 4, row, GUITextBuffer);

		if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].statusMode == 0)
			mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(27));
		else if (tempExpansionCards[CurrentBoardNo].settings.HARTcard.readChannel[row].statusMode == 1)
			mini_snprintf(GUITextBuffer, 20, GUI_LANG_GetText(154));
		LISTVIEW_SetItemText(hItem, 5, row, GUITextBuffer);

		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_LISTVIEW_ADDRESSES:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				BUZZER_Beep();
				CreateEditHARTReadValues(pMsg->hWin, LISTVIEW_GetSel(pMsg->hWinSrc));
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

WM_HWIN CreateSetIOBoardsWin(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetIOBoards, GUI_COUNTOF(_aSetIOBoards), _cbSetIOBoards, hParent, 0, 0);
	USERMESSAGE_ChangeWinTitle("I/O");
	return hWin;
}

static WM_HWIN SetIOBoard(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetIOBoard, GUI_COUNTOF(_aSetIOBoard), _cbSetIOBoard, hParent, 0, 0);
	return hWin;
}

static WM_HWIN SetGeneralIOBoard(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetGeneralIOBoard, GUI_COUNTOF(_aSetGeneralIOBoard), _cbSetGeneralIOBoard, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetRS485port(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetRS485port, GUI_COUNTOF(_aSetRS485port), _cbSetRS485port, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetRS485registers(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetRS485registers, GUI_COUNTOF(_aSetRS485registers), _cbSetRS485registers, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetRegister(WM_HWIN hParent, int regNo)
{
	WM_HWIN hWin;
	CurrentInputNo = regNo;
	tempRSchannelSettings = tempExpansionCards[CurrentBoardNo].settings.RScard.channels[CurrentInputNo];
	hWin = GUI_CreateDialogBox(_aSetRegister, GUI_COUNTOF(_aSetRegister), _cbSetRegister, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetAnalogOutputBoard(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetAnalogOutputBoard, GUI_COUNTOF(_aSetAnalogOutputBoard), _cbSetAnalogOutputBoard, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetGeneralHARTBoard(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetGeneralHARTBoard, GUI_COUNTOF(_aSetGeneralHARTBoard), _cbSetGeneralHARTBoard, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetDevicesHARTBoard(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetDevicesHARTBoard, GUI_COUNTOF(_aSetDevicesHARTBoard), _cbSetDevicesHARTBoard, hParent, 0, 0);
	return hWin;
}

static WM_HWIN CreateSetValuesHARTBoard(WM_HWIN hParent)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSetValuesHARTBoard, GUI_COUNTOF(_aSetValuesHARTBoard), _cbSetValuesHARTBoard, hParent, 0, 0);
	return hWin;
}

static int CheckHARTcardSettings(HART_CARD_SETTINGS *old, HART_CARD_SETTINGS *new)
{

	if (old->isSecondary != new->isSecondary || old->isResistor != new->isResistor || old->preambleLenght != new->preambleLenght)
	{
		NewSettings = 1;
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}

	for (int j = 0; j < MAX_HART_SENSOR_FOR_CARD; j++)
	{
		if (old->sensors[j].activity != new->sensors[j].activity || old->sensors[j].hartRev != new->sensors[j].hartRev
				|| old->sensors[j].address != new->sensors[j].address)
		{
			NewSettings = 1;
			RestartDevice = 1;
			NewArchive = 1;
			return 1;
		}
	}

	for (int j = 0; j < 25; j++)
	{
		if (old->readChannel[j].sensorIdx != new->readChannel[j].sensorIdx || old->readChannel[j].valueType != new->readChannel[j].valueType
				|| old->readChannel[j].commandType != new->readChannel[j].commandType || old->readChannel[j].valueAddr != new->readChannel[j].valueAddr
				|| old->readChannel[j].statusMode != new->readChannel[j].statusMode)
		{
			NewSettings = 1;
			RestartDevice = 1;
			NewArchive = 1;
			return 1;
		}
	}
	return 0;
}

static int CheckIOcardSettings(IO_CARD_SETTINGS *old, IO_CARD_SETTINGS *new)
{
	if (old->generalSet != new->generalSet)
	{
		NewSettings = 1;
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}
	for (int j = 0; j < 6; j++)
	{
		if (old->channels[j].adjusment != new->channels[j].adjusment || old->channels[j].compChannel != new->channels[j].compChannel
				|| old->channels[j].compensationConstantValue != new->channels[j].compensationConstantValue || old->channels[j].type != new->channels[j].type)
		{
			NewSettings = 1;
			RestartDevice = 1;
			NewArchive = 1;
			return 1;
		}
	}
	return 0;
}

static int CheckOUTcardSettings(OUT_CARD_SETTINGS *old, OUT_CARD_SETTINGS *new)
{
	for (int j = 0; j < 3; j++)
	{
		if (old->channels[j].type != new->channels[j].type || old->channels[j].source != new->channels[j].source
				|| old->channels[j].lowScaleValue != new->channels[j].lowScaleValue || old->channels[j].highScaleValue != new->channels[j].highScaleValue
				|| old->channels[j].failureMode != new->channels[j].failureMode || old->channels[j].failureValue != new->channels[j].failureValue)
		{
			NewSettings = 1;
			RestartDevice = 1;
			NewArchive = 1;
			return 1;
		}
	}
	return 0;
}

static int CheckRScardPortSettings(RS_PORT_SETTINGS *old, RS_PORT_SETTINGS *new)
{
	if (old->baudrate != new->baudrate || old->parity != new->parity || old->stopBits != new->stopBits || old->timeout != new->timeout)
	{
		NewSettings = 1;
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}
	else
		return 0;
}

static int CheckRScardChannelSettings(RS_CHANNEL_SETTINGS *old, RS_CHANNEL_SETTINGS *new)
{
	if (old->deviceAdr != new->deviceAdr || old->port != new->port || old->registerType != new->registerType || old->registerAdr != new->registerAdr)
	{
		NewSettings = 1;
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}
	else
		return 0;
}

static int CheckRScardSettings(RS_CARD_SETTINGS *old, RS_CARD_SETTINGS *new)
{
	if (CheckRScardPortSettings(&old->port[0], &new->port[0]) || CheckRScardPortSettings(&old->port[1], &new->port[1]))
		return 1;

	for (int i = 0; i < 25; ++i)
		if (CheckRScardChannelSettings(&old->channels[i], &new->channels[i]))
			return 1;

	return 0;

}

static int ChcekExpansionCardsSettings(EXPANSION_CARD *old, EXPANSION_CARD *new)
{
	if (old->type != new->type)
	{
		NewSettings = 1;
		RestartDevice = 1;
		NewArchive = 1;
		return 1;
	}
	else
	{
		if (old->type == SPI_CARD_2RS485 || new->type == SPI_CARD_2RS485I24)
			return CheckRScardSettings(&old->settings.RScard, &new->settings.RScard);
		else if (old->type == SPI_CARD_OUT3 || new->type == SPI_CARD_OUT3)
			return CheckOUTcardSettings(&old->settings.OUTcard, &new->settings.OUTcard);
		else if (old->type == SPI_CARD_1HRT)
			return CheckHARTcardSettings(&old->settings.HARTcard, &new->settings.HARTcard);
		else
			return CheckIOcardSettings(&old->settings.IOcard, &new->settings.IOcard);
	}
}

static void DeleteIOPages(MULTIPAGE_Handle hObj, uint8_t CardType)
{
	switch (CardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
	case SPI_CARD_IN6TC:
	case SPI_CARD_IN6V:
	case SPI_CARD_IN6RTD:
	case SPI_CARD_OUT6RL:
		for (int i = 0; i < 6; i++)
			MULTIPAGE_DeletePage(hObj, 0, 1);
		break;
	case SPI_CARD_IN3RTD:
	case SPI_CARD_IN3:
	case SPI_CARD_OUT3:
		for (int i = 0; i < 3; i++)
			MULTIPAGE_DeletePage(hObj, 0, 1);
		break;
	case SPI_CARD_IN6D:
		for (int i = 0; i < 7; i++)
			MULTIPAGE_DeletePage(hObj, 0, 1);
		break;
	case SPI_CARD_2RS485:
	case SPI_CARD_2RS485I24:
		for (int i = 0; i < 2; i++)
			MULTIPAGE_DeletePage(hObj, 0, 1);
		break;
	case SPI_CARD_1HRT:
		for (int i = 0; i < 3; ++i)
			MULTIPAGE_DeletePage(hObj, 0, 1);
		break;
	case SPI_CARD_IN4SG:
		for (int i = 0; i < 5; ++i)
			MULTIPAGE_DeletePage(hObj, 0, 1);
		break;
	default:
		break;
	}
}

static void RefreshIOPages(WM_HWIN hWinSrc, uint8_t CardType)
{
	switch (CardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
	case SPI_CARD_IN6TC:
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN6V:
	case SPI_CARD_OUT6RL:
		for (int i = 0; i < 6; i++)
			SendMessageToPage(hWinSrc, WM_USER_REFRESH, i);
		break;
	case SPI_CARD_IN3RTD:
	case SPI_CARD_IN3:
	case SPI_CARD_OUT3:
		for (int i = 0; i < 3; i++)
			SendMessageToPage(hWinSrc, WM_USER_REFRESH, i);
		break;
	case SPI_CARD_IN6D:
		UserMessage.MsgId = WM_USER_REFRESH;
		UserMessage.hWinSrc = hWinSrc;
		UserMessage.Data.v = 0;
		UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), 0);
		WM_SendMessage(UserMessage.hWin, &UserMessage);
		for (int i = 1; i < 7; i++)
		{
			UserMessage.MsgId = WM_USER_REFRESH;
			UserMessage.hWinSrc = hWinSrc;
			UserMessage.Data.v = i - 1;
			UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), i);
			WM_SendMessage(UserMessage.hWin, &UserMessage);
		}
		break;
	case SPI_CARD_2RS485:
	case SPI_CARD_2RS485I24:
	case SPI_CARD_1HRT:
		for (int i = 0; i < 3; ++i)
			SendMessageToPage(hWinSrc, WM_USER_REFRESH, i);
		break;
	case SPI_CARD_IN4SG:
		UserMessage.MsgId = WM_USER_REFRESH;
		UserMessage.hWinSrc = hWinSrc;
		UserMessage.Data.v = 0;
		UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), 0);
		WM_SendMessage(UserMessage.hWin, &UserMessage);
		for (int i = 1; i < 5; i++)
		{
			UserMessage.MsgId = WM_USER_REFRESH;
			UserMessage.hWinSrc = hWinSrc;
			UserMessage.Data.v = i - 1;
			UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), i);
			WM_SendMessage(UserMessage.hWin, &UserMessage);
		}
		break;

	default:
		break;
	}
}

static void SaveSettingsFromIOPages(WM_HWIN hWinSrc, uint8_t CardType)
{
	switch (CardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
	case SPI_CARD_IN6TC:
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN6V:
	case SPI_CARD_OUT6RL:
		for (int i = 0; i < 6; i++)
			SendMessageToPage(hWinSrc, WM_USER_SAVE_TEMP, i);
		break;
	case SPI_CARD_IN3RTD:
	case SPI_CARD_IN3:
	case SPI_CARD_OUT3:
		for (int i = 0; i < 3; i++)
			SendMessageToPage(hWinSrc, WM_USER_SAVE_TEMP, i);
		break;
	case SPI_CARD_IN6D:
		UserMessage.MsgId = WM_USER_SAVE_TEMP;
		UserMessage.hWinSrc = hWinSrc;
		UserMessage.Data.v = 0;
		UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), 0);
		WM_SendMessage(UserMessage.hWin, &UserMessage);
		for (int i = 1; i < 7; i++)
		{
			UserMessage.MsgId = WM_USER_SAVE_TEMP;
			UserMessage.hWinSrc = hWinSrc;
			UserMessage.Data.v = i - 1;
			UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), i);
			WM_SendMessage(UserMessage.hWin, &UserMessage);
		}
		break;
	case SPI_CARD_2RS485:
	case SPI_CARD_2RS485I24:
		UserMessage.MsgId = WM_USER_SAVE_TEMP;
		UserMessage.hWinSrc = hWinSrc;
		UserMessage.Data.v = 0;
		UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), 0);
		WM_SendMessage(UserMessage.hWin, &UserMessage);
	case SPI_CARD_1HRT:
		for (int i = 0; i < 3; ++i)
			SendMessageToPage(hWinSrc, WM_USER_SAVE_TEMP, i);
		break;
	case SPI_CARD_IN4SG:
		UserMessage.MsgId = WM_USER_SAVE_TEMP;
		UserMessage.hWinSrc = hWinSrc;
		UserMessage.Data.v = 0;
		UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), 0);
		WM_SendMessage(UserMessage.hWin, &UserMessage);
		for (int i = 1; i < 5; i++)
		{
			UserMessage.MsgId = WM_USER_SAVE_TEMP;
			UserMessage.hWinSrc = hWinSrc;
			UserMessage.Data.v = i - 1;
			UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), i);
			WM_SendMessage(UserMessage.hWin, &UserMessage);
		}
		break;
	default:
		break;
	}
}

static void SendMessageToPage(WM_HWIN hWinSrc, int MsgId, int PageNumber)
{
	UserMessage.MsgId = MsgId;
	UserMessage.hWinSrc = hWinSrc;
	UserMessage.Data.v = PageNumber;
	UserMessage.hWin = MULTIPAGE_GetWindow(WM_GetDialogItem(hWinSrc, ID_MULTIPAGE_IO), PageNumber);
	WM_SendMessage(UserMessage.hWin, &UserMessage);
}

static void CreateIOPages(MULTIPAGE_Handle hObj, uint8_t CardType)
{
	MULTIPAGE_EnableScrollbar(hObj, 0);
	switch (CardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
	case SPI_CARD_IN6TC:
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN6V:
	case SPI_CARD_OUT6RL:
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 1");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 2");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 3");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 4");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 5");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 6");

		MULTIPAGE_SetTabWidth(hObj, 122, 0);
		MULTIPAGE_SetTabWidth(hObj, 121, 1);
		MULTIPAGE_SetTabWidth(hObj, 121, 2);
		MULTIPAGE_SetTabWidth(hObj, 121, 3);
		MULTIPAGE_SetTabWidth(hObj, 121, 4);
		MULTIPAGE_SetTabWidth(hObj, 122, 5);

		break;
	case SPI_CARD_IN4SG:
		MULTIPAGE_AddEmptyPage(hObj, SetGeneralIOBoard(hObj), GUI_LANG_GetText(10));
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 1");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 2");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 3");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 4");

		MULTIPAGE_SetTabWidth(hObj, 165, 0);
		MULTIPAGE_SetTabWidth(hObj, 140, 1);
		MULTIPAGE_SetTabWidth(hObj, 140, 2);
		MULTIPAGE_SetTabWidth(hObj, 140, 3);
		MULTIPAGE_SetTabWidth(hObj, 140, 4);

		break;
	case SPI_CARD_IN3RTD:
	case SPI_CARD_IN3:
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 1");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 2");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 3");

		MULTIPAGE_SetTabWidth(hObj, 242, 0);
		MULTIPAGE_SetTabWidth(hObj, 244, 1);
		MULTIPAGE_SetTabWidth(hObj, 242, 2);

		break;
	case SPI_CARD_IN6D:
		MULTIPAGE_AddEmptyPage(hObj, SetGeneralIOBoard(hObj), GUI_LANG_GetText(10));
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 1");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 2");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 3");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 4");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 5");
		MULTIPAGE_AddEmptyPage(hObj, SetIOBoard(hObj), "IO 6");

		MULTIPAGE_SetTabWidth(hObj, 128, 0);
		MULTIPAGE_SetTabWidth(hObj, 100, 1);
		MULTIPAGE_SetTabWidth(hObj, 100, 2);
		MULTIPAGE_SetTabWidth(hObj, 100, 3);
		MULTIPAGE_SetTabWidth(hObj, 100, 4);
		MULTIPAGE_SetTabWidth(hObj, 100, 5);
		MULTIPAGE_SetTabWidth(hObj, 100, 6);
		break;
	case SPI_CARD_2RS485:
	case SPI_CARD_2RS485I24:
		MULTIPAGE_AddEmptyPage(hObj, CreateSetRS485port(hObj), "RS485 COM");
		MULTIPAGE_AddEmptyPage(hObj, CreateSetRS485registers(hObj), GUI_LANG_GetText(187));
		MULTIPAGE_SetTabWidth(hObj, 365, 0);
		MULTIPAGE_SetTabWidth(hObj, 365, 1);

		break;
	case SPI_CARD_OUT3:
		MULTIPAGE_AddEmptyPage(hObj, CreateSetAnalogOutputBoard(hObj), "IO 1");
		MULTIPAGE_AddEmptyPage(hObj, CreateSetAnalogOutputBoard(hObj), "IO 2");
		MULTIPAGE_AddEmptyPage(hObj, CreateSetAnalogOutputBoard(hObj), "IO 3");

		MULTIPAGE_SetTabWidth(hObj, 242, 0);
		MULTIPAGE_SetTabWidth(hObj, 244, 1);
		MULTIPAGE_SetTabWidth(hObj, 242, 2);

		break;
	case SPI_CARD_1HRT:
		MULTIPAGE_AddEmptyPage(hObj, CreateSetGeneralHARTBoard(hObj), GUI_LANG_GetText(10));
		MULTIPAGE_AddEmptyPage(hObj, CreateSetDevicesHARTBoard(hObj), GUI_LANG_GetText(224));
		MULTIPAGE_AddEmptyPage(hObj, CreateSetValuesHARTBoard(hObj), GUI_LANG_GetText(229));

		MULTIPAGE_SetTabWidth(hObj, 242, 0);
		MULTIPAGE_SetTabWidth(hObj, 244, 1);
		MULTIPAGE_SetTabWidth(hObj, 242, 2);
		break;
	default:
		break;
	}
	CurrentInputNo = 0;
	MULTIPAGE_SelectPage(hObj, CurrentInputNo);
}

static void SetAdjusmentParametersVisibility(WM_HWIN hWin, EXPANSION_CARD *card, uint8_t BoardChannelNo)
{
	WM_HWIN hItem;
	char GUITextBuffer[20];
	uint8_t measureMethod = DROPDOWN_GetSel(WM_GetDialogItem(hWin, ID_DROPDOWN_MODE));
	if (measureMethod == 0)
	{
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ADJUSMENT_DESC);
		WM_HideWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ADJUSMENT_UNIT);
		WM_HideWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_EDIT_ADJUSMENT);
		WM_HideWindow(hItem);
	}
	else
	{
		if (card->type == SPI_CARD_OUT6RL || card->type == SPI_CARD_IN6D)
		{
			hItem = WM_GetDialogItem(hWin, ID_TEXT_ADJUSMENT_DESC);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_TEXT_ADJUSMENT_UNIT);
			WM_HideWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_ADJUSMENT);
			WM_HideWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(hWin, ID_TEXT_ADJUSMENT_DESC);
			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_TEXT_ADJUSMENT_UNIT);
			SetCardUnit(hItem, card->type, measureMethod);

			WM_ShowWindow(hItem);
			hItem = WM_GetDialogItem(hWin, ID_EDIT_ADJUSMENT);
			WM_ShowWindow(hItem);
			float2stri(GUITextBuffer, card->settings.IOcard.channels[BoardChannelNo].adjusment, 4);
			EDIT_SetText(hItem, GUITextBuffer);
		}
	}
}

static void SetConstantCompensationValueVisibility(WM_HWIN hWin, char state)
{
	WM_HWIN hItem;
	if (state == 1)
	{
		hItem = WM_GetDialogItem(hWin, ID_EDIT_COMPENSATION_VALUE);
		WM_ShowWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_COMPENSATION_UNIT);
		WM_ShowWindow(hItem);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_EDIT_COMPENSATION_VALUE);
		WM_HideWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_TEXT_COMPENSATION_UNIT);
		WM_HideWindow(hItem);
	}
}

static void InitCompensationParametersValues(WM_HWIN hWin, const EXPANSION_CARD *card, uint8_t BoardChannelNo)
{
	WM_HWIN hItem;
	char GUITextBuffer[20];
	if (card->type == SPI_CARD_IN6TC || card->type == SPI_CARD_IN3)
	{
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_COMPENSATION_CHANNEL);
		DROPDOWN_SetSel(hItem, card->settings.IOcard.channels[BoardChannelNo].compChannel + 2);

		hItem = WM_GetDialogItem(hWin, ID_EDIT_COMPENSATION_VALUE);
		float2stri(GUITextBuffer, card->settings.IOcard.channels[BoardChannelNo].compensationConstantValue, 4);
		EDIT_SetText(hItem, GUITextBuffer);
	}
}

static void SetCompensationParametersVisibility(WM_HWIN hWin, uint8_t cardType)
{
	WM_HWIN hItem;

	if (cardType == SPI_CARD_IN6TC && DROPDOWN_GetSel(WM_GetDialogItem(hWin, ID_DROPDOWN_MODE)) == 1)
	{
		hItem = WM_GetDialogItem(hWin, ID_TEXT_COMPENSATION_DESC);
		WM_ShowWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_COMPENSATION_CHANNEL);
		WM_ShowWindow(hItem);
		if (DROPDOWN_GetSel(hItem) == 1)
			SetConstantCompensationValueVisibility(hWin, 1);
		else
			SetConstantCompensationValueVisibility(hWin, 0);
	}
	else if (cardType == SPI_CARD_IN3 && DROPDOWN_GetSel(WM_GetDialogItem(hWin, ID_DROPDOWN_MODE)) == 5)
	{
		hItem = WM_GetDialogItem(hWin, ID_TEXT_COMPENSATION_DESC);
		WM_ShowWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_COMPENSATION_CHANNEL);
		WM_ShowWindow(hItem);
		if (DROPDOWN_GetSel(hItem) == 1)
			SetConstantCompensationValueVisibility(hWin, 1);
		else
			SetConstantCompensationValueVisibility(hWin, 0);
	}
	else
	{
		hItem = WM_GetDialogItem(hWin, ID_TEXT_COMPENSATION_DESC);
		WM_HideWindow(hItem);
		hItem = WM_GetDialogItem(hWin, ID_DROPDOWN_COMPENSATION_CHANNEL);
		WM_HideWindow(hItem);
		SetConstantCompensationValueVisibility(hWin, 0);
	}
}

static void PaintIOBoardDecoration(WM_HWIN hWin)
{
	WM_HWIN hItem = WM_GetDialogItem(hWin, ID_EDIT_ADJUSMENT);
	if (WM_IsVisible(hItem))
		DRAW_RoundedFrame(210, 52, 240, 39, 4, 2, GUI_BLUE);

	hItem = WM_GetDialogItem(hWin, ID_EDIT_COMPENSATION_VALUE);
	if (WM_IsVisible(hItem))
		DRAW_RoundedFrame(470, 97, 250, 39, 4, 2, GUI_BLUE);
}

static int CheckParameterLimits(EDIT_Handle hObj, int min, int max)
{
	char GUITextBuffer[10];
	EDIT_GetText(hObj, GUITextBuffer, 10);
	int temp = atoi(GUITextBuffer);
	if (temp < min || max < temp)
	{
		EDIT_SetTextColor(hObj, EDIT_CI_ENABLED, GUI_RED);
		return 0;
	}
	else
	{
		EDIT_SetTextColor(hObj, EDIT_CI_ENABLED, SKINS_GetTextColor());
		return 1;
	}
}

static void LISTVIEW_RefreshRSChannel(LISTVIEW_Handle hObj, unsigned Row, RS_CHANNEL_SETTINGS *rsChannel)
{
	char GUITextBuffer[10];
	itoa(rsChannel->port, GUITextBuffer, 10);
	LISTVIEW_SetItemText(hObj, 1, Row, GUITextBuffer);
	itoa(rsChannel->deviceAdr, GUITextBuffer, 10);
	LISTVIEW_SetItemText(hObj, 2, Row, GUITextBuffer);
	itoa(rsChannel->registerAdr, GUITextBuffer, 10);
	LISTVIEW_SetItemText(hObj, 3, Row, GUITextBuffer);
	LISTVIEW_SetItemText(hObj, 4, Row, rsChannelTypeDesc[rsChannel->registerType]);
}

static void LISTVIEW_InitRSchannels(LISTVIEW_Handle hObj, RS_CARD_SETTINGS *rsCard)
{
	char GUITextBuffer[3];
	for (int i = 0; i < 25; i++)
	{
		LISTVIEW_AddRow(hObj, NULL);
		itoa(i + 1, GUITextBuffer, 10);
		LISTVIEW_SetItemText(hObj, 0, i, GUITextBuffer);
		LISTVIEW_RefreshRSChannel(hObj, i, &rsCard->channels[i]);
	}
}

static void SetOutputFailureValueVisibility(WM_HWIN hWin, int mode)
{
	if (mode)
	{
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_FAILURE));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT));
	}
	else
	{
		WM_HideWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_FAILURE));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT));
	}
}

static void SetOutputElementsVisibility(WM_HWIN hWin, int mode)
{
	if (mode)
	{
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUTPUT_SOURCE));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_DROPDOWN_ANALOG_OUTPUT_SOURCE));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_HIGH));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_LOW));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE));
		WM_ShowWindow(WM_GetDialogItem(hWin, ID_DROPDOWN_ANALOG_OUT_FAILURE));
		SetOutputFailureValueVisibility(hWin, DROPDOWN_GetSel(WM_GetDialogItem(hWin,
		ID_DROPDOWN_ANALOG_OUT_FAILURE)));
	}
	else
	{
		WM_HideWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUTPUT_SOURCE));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_DROPDOWN_ANALOG_OUTPUT_SOURCE));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_HIGH));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_LOW));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_DROPDOWN_ANALOG_OUT_FAILURE));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_EDIT_ANALOG_OUT_FAILURE));
		WM_HideWindow(WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT));
	}
}

static void DrawOutputDecorations(WM_HWIN hWin)
{
	if (DROPDOWN_GetSel(WM_GetDialogItem(hWin, ID_DROPDOWN_ANALOG_OUTPUT_MODE)))
	{
		DRAW_RoundedFrame(110, 98, 180, 38, 4, 2, GUI_BLUE);
		DRAW_RoundedFrame(415, 98, 180, 38, 4, 2, GUI_BLUE);
		if (DROPDOWN_GetSel(WM_GetDialogItem(hWin, ID_DROPDOWN_ANALOG_OUT_FAILURE)))
			DRAW_RoundedFrame(415, 143, 180, 38, 4, 2, GUI_BLUE);
	}
}

static void SetOutputScaleDescryptions(WM_HWIN hWin, int mode)
{
	WM_HWIN hItem;
	switch (mode)
	{
	case 1:
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetText(hItem, "→ 0 mA");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetText(hItem, "→ 20 mA");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetText(hItem, "mA");
		break;
	case 2:
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetText(hItem, "→ 4 mA");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetText(hItem, "→ 20 mA");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetText(hItem, "mA");
		break;
	case 3:
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetText(hItem, "→ 0 mA");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetText(hItem, "→ 24 mA");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetText(hItem, "mA");
		break;
	case 4:
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetText(hItem, "→ 0 V");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetText(hItem, "→ 5 V");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetText(hItem, "V");
		break;
	case 5:
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetText(hItem, "→ 0 V");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetText(hItem, "→ 10 V");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetText(hItem, "V");
		break;
	default:
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_LOW);
		TEXT_SetText(hItem, "");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_HIGH);
		TEXT_SetText(hItem, "");
		hItem = WM_GetDialogItem(hWin, ID_TEXT_ANALOG_OUT_FAILURE_UNIT);
		TEXT_SetText(hItem, "");
		break;
	}
}

#include "KeyboardDLG.h"
#include "parameters.h"
#include "buzzer.h"
#include "draw.h"

#include "ComputeChannel.h"
#include "PopUpMessageDLG.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato60;

extern GUI_CONST_STORAGE GUI_BITMAP bmOK;
extern GUI_CONST_STORAGE GUI_BITMAP bmNO;

#define ID_WINDOW_0 		(GUI_ID_USER + 0x500)
#define ID_EDIT_0 			(GUI_ID_USER + 0x501)
#define ID_MULTIEDIT_0 	(GUI_ID_USER + 0x502)
#define ID_BUTTON_0 		(GUI_ID_USER + 0x503)
#define ID_WINDOW_1 		(GUI_ID_USER + 0x504)
#define ID_EDIT_1 			(GUI_ID_USER + 0x505)
#define ID_MULTIEDIT_1 	(GUI_ID_USER + 0x506)

#define CAPS_LOCK_ID 		(GUI_ID_USER + 0x507)
#define SHIFT_ID 				(GUI_ID_USER + 0x508)
#define ALT_ID 					(GUI_ID_USER + 0x509)
#define ENTER_ID 				(GUI_ID_USER + 0x50A)
#define SPECIAL1_ID 		(GUI_ID_USER + 0x50B)
#define REMOVE_ID 			(GUI_ID_USER + 0x50C)

#define MOVEINX 0
#define MOVEINY 25

int extendedKeyboardOn = 0;

/*
 *ponizej znajduja sie tablice odpowiedzialne za znaki w klawiaturze.
 *Odpowiednio podzielone na tablice z id oraz graficzna reprezentacja znaku
 */

static int upperText[] =
{ 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 65, 83, 68, 70, 71, 72, 74, 75, 76, 90, 88, 67, 86, 66, 78, 77, GUI_KEY_BACKSPACE,
CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int upperSigns[] =
{ 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int lowerText[] =
{ 81 + 32, 87 + 32, 69 + 32, 82 + 32, 84 + 32, 89 + 32, 85 + 32, 73 + 32, 79 + 32, 80 + 32, 65 + 32, 83 + 32, 68 + 32, 70 + 32, 71 + 32,
	72 + 32, 74 + 32, 75 + 32, 76 + 32, 90 + 32, 88 + 32, 67 + 32, 86 + 32, 66 + 32, 78 + 32, 77 + 32, GUI_KEY_BACKSPACE, CAPS_LOCK_ID,
	SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int lowerSigns[] =
{ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int special1Text[] =
{ '!', '@', '#', '$', '%', '&', '?', '_', '"', 39, 8800, 8804, 8805, '/', '=', '^', 178, 179, 8721, 8730, '(', ')', ':', 176, 8364, 8486,
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int special1Signs[] =
{ '!', '@', '#', '$', '%', '&', '?', '_', '"', 39, '≠', '≤', '≥', '/', '=', '^', '²', '³', '∑', '√', '(', ')', ':', '°', '€', 'Ω', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int polishUpperText[] =
{ ' ', ' ', 280, ' ', ' ', ' ', ' ', ' ', 211, ' ', 260, 346, ' ', ' ', ' ', ' ', ' ', ' ', 321, 379, 377, 262, ' ', ' ', 323, ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int polishUpperSigns[] =
{ ' ', ' ', 'Ę', ' ', ' ', ' ', ' ', ' ', 'Ó', ' ', 'Ą', 'Ś', ' ', ' ', ' ', ' ', ' ', ' ', 'Ł', 'Ż', 'Ź', 'Ć', ' ', ' ', 'Ń', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int polishLowerText[] =
{ ' ', ' ', 281, ' ', ' ', ' ', ' ', ' ', 243, ' ', 261, 347, ' ', ' ', ' ', ' ', ' ', ' ', 322, 380, 378, 263, ' ', ' ', 324, ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int polishLowerSigns[] =
{ ' ', ' ', 'ę', ' ', ' ', ' ', ' ', ' ', 'ó', ' ', 'ą', 'ś', ' ', ' ', ' ', ' ', ' ', ' ', 'ł', 'ż', 'ź', 'ć', ' ', ' ', 'ń', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int deutschUpperText[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', 220, ' ', 214, ' ', 196, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int deutschUpperSigns[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', 'Ü', ' ', 'Ö', ' ', 'Ä', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int deutschLowerText[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', 252, ' ', 246, ' ', 228, 223, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int deutschLowerSigns[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', 'ü', ' ', 'ö', ' ', 'ä', 'ß', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int portugueseUpperText[] =
{ ' ', ' ', 201, 202, ' ', ' ', 218, 205, 211, 212, 193, 194, 195, 192, ' ', ' ', ' ', ' ', ' ', ' ', ' ', 199, ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int portugueseUpperSigns[] =
{ ' ', ' ', 'Ȩ', 'Ê', ' ', ' ', 'Ú', 'Í', 'Ó', 'Ô', 'Á', 'Â', 'Ã', 'À', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'Ç', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int portugueseLowerText[] =
{ ' ', ' ', 233, 234, ' ', ' ', ' ', 237, 243, ' ', 225, 226, 227, 224, ' ', ' ', ' ', ' ', ' ', ' ', ' ', 231, ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int portugueseLowerSigns[] =
{ ' ', ' ', 'ȩ', 'ê', ' ', ' ', ' ', 'í', 'ó', ' ', 'á', 'â', 'ã', 'à', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'ç', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int frenchUpperText[] =
{ 203, 201, 202, 200, 220, 217, 219, 206, 207, 212, 376, ' ', 194, 192, ' ', ' ', ' ', ' ', ' ', ' ', ' ', 199, ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int frenchUpperSigns[] =
{ 'Ë', 'É', 'Ê', 'È', 'Ü', 'Ù', 'Û', 'Î', 'Ï', 'Ô', 'Ÿ', ' ', 'Â', 'À', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'Ç', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int frenchLowerText[] =
{ 235, 233, 234, 232, 252, 249, 251, 238, 239, 244, 255, ' ', 226, 224, ' ', ' ', ' ', ' ', ' ', ' ', ' ', 231, ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int frenchLowerSigns[] =
{ 'ë', 'é', 'ê', 'è', 'ü', 'ù', 'û', 'î', 'ï', 'ô', 'ÿ', ' ', 'â', 'à', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'ç', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };


static int italianUpperText[] =
{ ' ', 201, 200, ' ', ' ', ' ', 217, 204, 210, ' ', 192, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int italianUpperSigns[] =
{ ' ', 'É', 'È', ' ', ' ', ' ', 'Ù', 'Ì', 'Ò', ' ', 'À', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int italianLowerText[] =
{ ' ', 233, 232, ' ', ' ', ' ', 249, 236, 242, ' ', 224, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int italianLowerSigns[] =
{ ' ', 'é', 'è', ' ', ' ', ' ', 'ù', 'ì', 'ò', ' ', 'à', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };


static int spanishUpperText[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 209, ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int spanishUpperSigns[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'Ñ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int spanishLowerText[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 241, ' ',
	GUI_KEY_BACKSPACE, CAPS_LOCK_ID, SHIFT_ID, ALT_ID, ENTER_ID, GUI_KEY_ENTER, SPECIAL1_ID, 32, REMOVE_ID };

static int spanishLowerSigns[] =
{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'ñ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

static int alphanumericKeyboardText[] =
{ '1', '2', '3', 247, '4', '5', '6', 215, '7', '8', '9', '-', '=', '0', '.', '+' };

static int alphanumericKeyboardSigns[] =
{ '1', '2', '3', '÷', '4', '5', '6', '×', '7', '8', '9', '-', '=', '0', '.', '+' };

static int numericKeyboardText[] =
{ '1', '2', '3', GUI_KEY_BACKSPACE,
	'4', '5', '6', ' ',
	'7', '8', '9', ENTER_ID,
	'-', '0', '.', REMOVE_ID};

static int numericKeyboardSigns[] =
{ '1', '2', '3', '←',
	'4', '5', '6', ' ',
	'7', '8', '9', ' ',
	'-', '0', '.', ' '
};

static int numericKeyboardTextExtended[] =
{ '(',  ')', '1', '2', '3', GUI_KEY_BACKSPACE,
	8730, 215, '4', '5', '6', '#',
	178,  247, '7', '8', '9', ENTER_ID,
	179,  '+', '-', '0','.',  REMOVE_ID,	'^' };

static int numericKeyboardSignsExtended[] =
{ '(', ')', '1', '2', '3', '←',
	'√', '×', '4', '5', '6', '#',
	'²', '÷', '7', '8', '9', ' ',
	'³', '+', '-', '0', '.', ' ', '^'
};

//enum odpowiedzialny za informowanie programu o tym, ktora klawiatura jest obecnie wyswietlana na ekranie
enum letterCase
{
	upper, lower, alternateupper, alternatelower, special1, special2
} lCase;

enum letterFlag
{
	zero, shifted, alted, shiftalted
} lFlag;

enum letterCapsLock
{
	CapsLocked, NoCapsLocked
} lCapsLock;

unsigned MaxNumChars;
static char * keyboardText[KEYBOARD_TEXT_NUMBER] __attribute__ ((section(".sdram")));
static BUTTON_Handle keyButton[NUMBER_OF_BUTTONS];

static int idWidgetType;

static BUTTON_Handle keyAlphaNumButtons[16];
static BUTTON_Handle keyNumButtons[21];
static BUTTON_Handle keyNumButtonsExtended[27];

static WM_HWIN hWidget;
static WM_HWIN hKeyboardWindow = 0;

static int passCheck;
static int flagBackspace;
static int keyboardIs = 0;
static int isFocused;

static int *langUpperSigns;
static int *langUpperText;
static int *langLowerSigns;
static int *langLowerText;

static void EraseWhiteSpaceFromString(char* stringToCheck);

static void setButtons(BUTTON_Handle keyButtons[], int keyboardLetters[], int keyboardLettersId[], WM_HWIN keyboardHandle);
static void CleanUpKeyboardButtons(BUTTON_Handle keyButtons[]);

static void CloseKeyboardWindow();
static void CloseKeyboardWindowFormula();

static void setNumButtons(BUTTON_Handle keyButtons[], WM_HWIN keyboardHandle);
static void setNumKeyButtons(BUTTON_Handle keyButtons[], int keyboardLetters[], int keyboardLettersId[], WM_HWIN keyboardHandle);
static void setNumKeyButtonsExtended(BUTTON_Handle keyButtons[], int keyboardLetters[], int keyboardLettersId[], WM_HWIN keyboardHandle);

static void setLanguage(uint8_t deviceLanguage, int** upperSignsLanguage, int** upperTextLanguage, int** lowerSignsLanguage,
		int** lowerTextLanguage);

static const GUI_WIDGET_CREATE_INFO _aKeyboardDialogCreate[] =
{
{ WINDOW_CreateIndirect, "Window", ID_WINDOW_1, 0, 195, 740, 300, 0, 0x0, 0 },
{ MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_1, 60, 15, 620, 60, 0, 0x64, 0 }, };

void SetCapslockOn(void)
{
	BUTTON_SetTextColor(keyButton[27], BUTTON_CI_UNPRESSED, GUI_BLUE);
}

static void _cbKeyboardDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	int NCode;
	int Id;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_1);
		MULTIEDIT_SetText(hItem, "123");
		MULTIEDIT_EnableBlink(hItem, 500, 1);
		MULTIEDIT_SetInsertMode(hItem, 1);
		MULTIEDIT_SetWrapWord(hItem);
		lCase = lower;
		lCapsLock = NoCapsLocked;
		lFlag = zero;
		setLanguage(bkGeneralSettings.DeviceLanguage, &langUpperSigns, &langUpperText, &langLowerSigns, &langLowerText);
		break;

	case WM_POST_PAINT:
		DRAW_RoundedRect(0, 0, 736, 282, 4, GUI_GRAY);
		DRAW_RoundedRect(55, 10, 625, 69, 4, GUI_WHITE);
		break;

	case WM_TIMER:
		GUI_SendKeyMsg(GUI_KEY_BACKSPACE, 1);
		if (flagBackspace == 1)
			WM_RestartTimer(pMsg->Data.v, 200);
		else if (flagBackspace == 0)
			WM_DeleteTimer(pMsg->Data.v);
		break;

	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id)
		{
		case ID_MULTIEDIT_1:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				MULTIEDIT_SetTextColor(pMsg->hWinSrc,MULTIEDIT_CI_EDIT, GUI_BLACK);
				break;
			case WM_NOTIFICATION_LOST_FOCUS:
				if (extendedKeyboardOn)
					CloseKeyboardWindowFormula();
				else
					CloseKeyboardWindow();
				isFocused = 1;
				break;
			default:
				break;
			}
			break;
		case GUI_KEY_BACKSPACE:
			switch (NCode)
			{
			case WM_NOTIFICATION_CLICKED:
				flagBackspace = 1;
				WM_CreateTimer(pMsg->hWin, 0, 500, 0);
				BUZZER_Beep();
				break;
			case WM_NOTIFICATION_RELEASED:
				flagBackspace = 0;
				break;
			case	WM_NOTIFICATION_MOVED_OUT:
				flagBackspace = 0;
				break;
			}
			break;
		case ENTER_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if (extendedKeyboardOn)
					CloseKeyboardWindowFormula();
				else
					CloseKeyboardWindow();
				keyboardIs = 0;
				break;
			}
			break;
		case REMOVE_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				deleteKeyboard();
				keyboardIs = 0;
				break;
			}
			break;
		case ALT_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (!(passCheck == PASSON))
				{
					BUZZER_Beep();
					if (lCase == special1)
						break;
					CleanUpKeyboardButtons(keyButton);
					if (lCase == lower)
					{
						setButtons(keyButton, langLowerSigns, langLowerText, pMsg->hWin);
						lCase = alternatelower;
						lFlag = alted;
						if (lCapsLock == CapsLocked)
							SetCapslockOn();
					}
					else if (lCase == alternatelower)
					{
						setButtons(keyButton, lowerSigns, lowerText, pMsg->hWin);
						lCase = lower;
						lFlag = zero;
						if (lCapsLock == CapsLocked)
							SetCapslockOn();
					}
					else if (lCase == upper)
					{
						setButtons(keyButton, langUpperSigns, langUpperText, pMsg->hWin);
						lCase = alternateupper;
						lFlag = shiftalted;
						if (lCapsLock == CapsLocked)
							SetCapslockOn();
					}
					else if (lCase == alternateupper)
					{
						setButtons(keyButton, upperSigns, upperText, pMsg->hWin);
						lCase = upper;
						lFlag = zero;
						if (lCapsLock == CapsLocked)
							SetCapslockOn();
					}
					break;
				}
			}
			break;
		case CAPS_LOCK_ID:
			if (lCase == special1)
				break;
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:

				CleanUpKeyboardButtons(keyButton);
				BUZZER_Beep();
				if (lCase == lower)
				{
					setButtons(keyButton, upperSigns, upperText, pMsg->hWin);
					lCase = upper;
					lCapsLock = CapsLocked;
					SetCapslockOn();
				}
				else if (lCase == upper)
				{
					BUTTON_SetBkColor(keyButton[27], BUTTON_CI_UNPRESSED, GUI_GRAY);
					setButtons(keyButton, lowerSigns, lowerText, pMsg->hWin);
					lCase = lower;
					lCapsLock = NoCapsLocked;

					BUTTON_SetSkin(keyButton[27], BUTTON_SKIN_FLEX);
				}
				else if (lCase == alternatelower)
				{
					setButtons(keyButton, langUpperSigns, langUpperText, pMsg->hWin);
					lCase = alternateupper;
					lCapsLock = CapsLocked;
					SetCapslockOn();
				}
				else if (lCase == alternateupper)
				{
					setButtons(keyButton, langLowerSigns, langLowerText, pMsg->hWin);
					lCase = alternatelower;
					lCapsLock = NoCapsLocked;
				}

				break;
			}
			break;
		case SHIFT_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				if (lCase == special1)
					break;
				BUZZER_Beep();

				if (lCapsLock == NoCapsLocked)
				{
					CleanUpKeyboardButtons(keyButton);
					if (lCase == lower)
					{
						setButtons(keyButton, upperSigns, upperText, pMsg->hWin);
						lCase = upper;
						lFlag = shifted;
					}
					else if (lCase == upper)
					{
						setButtons(keyButton, lowerSigns, lowerText, pMsg->hWin);
						lCase = lower;
						lFlag = zero;
					}
					else if (lCase == alternatelower)
					{
						setButtons(keyButton, langUpperSigns, langUpperText, pMsg->hWin);
						lCase = alternateupper;
						lFlag = shiftalted;
					}
					else if (lCase == alternateupper)
					{
						setButtons(keyButton, langLowerSigns, langLowerText, pMsg->hWin);
						lCase = alternatelower;
						lFlag = zero;
					}
				}
				break;
			}
			break;
		case SPECIAL1_ID:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				CleanUpKeyboardButtons(keyButton);
				BUZZER_Beep();
				if (lCase != special1)
				{
					setButtons(keyButton, special1Signs, special1Text, pMsg->hWin);
					BUTTON_SetText(keyButton[32], "ABC");
					lCase = special1;
				}
				else
				{
					setButtons(keyButton, lowerSigns, lowerText, pMsg->hWin);
					lCase = lower;
					lCapsLock = NoCapsLocked;
					lFlag = zero;
				}
				break;
			}
			break;
		default:
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED:
				BUZZER_Beep();
				if ((lFlag == shifted || lFlag == alted || lFlag == shiftalted) && lCapsLock == NoCapsLocked)
				{
					setButtons(keyButton, lowerSigns, lowerText, pMsg->hWin);
					lCase = lower;
					lFlag = zero;
				}
				else if ((lFlag == shifted || lFlag == alted || lFlag == shiftalted) && lCapsLock == CapsLocked)
				{
					setButtons(keyButton, upperSigns, upperText, pMsg->hWin);
					SetCapslockOn();
					lCase = upper;
					lFlag = zero;
				}
				if (!(((passCheck == PASSON) && (Id == GUI_KEY_ENTER)) || ((idWidgetType == EDIT) && (Id == GUI_KEY_ENTER))))
					GUI_SendKeyMsg(Id, 1);
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

static void CleanUpKeyboardButtons(BUTTON_Handle keyButtons[])
{
	for (int i = 0; i < 27; i++)
		BUTTON_Delete(keyButtons[i]);
}

static void setButtons(BUTTON_Handle keyButtons[], int keyboardLetters[], int keyboardLettersId[], WM_HWIN keyboardHandle)
{
	int i = 0;
	int x0 = 0;
	int y0 = 0;
	char charConv[4] =
	{ 0 };
	for (i = 0; i < NUMBER_OF_BUTTONS - 2; i++)
	{
		//algorytm konwersji int na char * w zależnosci od dlugosci inta
		if (keyboardLetters[i] >= 65536)
		{
			charConv[0] = (keyboardLetters[i] >> 16) & 0xff;
			charConv[1] = (keyboardLetters[i] >> 8) & 0xff;
			charConv[2] = (keyboardLetters[i]) & 0xff;
			charConv[3] = 0;
		}
		if (keyboardLetters[i] >= 256 && keyboardLetters[i] <= 65535)
		{
			charConv[0] = (keyboardLetters[i] >> 8) & 0xff;
			charConv[1] = (keyboardLetters[i]) & 0xff;
			charConv[2] = 0;
		}
		if (keyboardLetters[i] >= 0 && keyboardLetters[i] <= 255)
		{
			charConv[0] = (keyboardLetters[i]) & 0xff;
			charConv[1] = 0;
		}
		//ulozenie przyciskow na klawiaturze
		if (i <= 9)
		{
			x0 = 21 + 47 * (i % 10) + MOVEINX;
			y0 = 70 + 42 * (i / 10) + MOVEINY;
		}
		else if (i >= 10 && i <= 18)
		{
			x0 = 34 + 47 * (i - 10) + MOVEINX;
			y0 = 90 + 27 + MOVEINY;
		}
		else if (i >= 19 && i <= 25)
		{
			x0 = 41 + 27 + 47 * (i - 19) + MOVEINX;
			y0 = 110 + 27 + 27 + MOVEINY;
		}
		else if (i == 26)
		{
			x0 = 21 + 47 * 10 + MOVEINX;
			y0 = 70 + MOVEINY;
		}
		else if (i == 27)
		{
			x0 = 21 + MOVEINX;
			y0 = 110 + 54 + MOVEINY;
		}
		else if (i == 28)
		{
			x0 = 41 + 27 + 47 * (i - 21) + MOVEINX;
			y0 = 110 + 54 + MOVEINY;
		}
		else if (i == 29)
		{
			x0 = 21 + MOVEINX;
			y0 = 130 + 81 + MOVEINY;
		}
		else if (i == 30)
		{
			x0 = 370 + MOVEINX;
			y0 = 130 + 81 + MOVEINY;
		}
		else if (i == 31)
		{
			x0 = 34 + 47 * 9 + MOVEINX;
			y0 = 90 + 27 + MOVEINY;
		}
		else if (i == 32)
		{
			x0 = 88 + MOVEINX;
			y0 = 130 + 81 + MOVEINY;
		}
		else if (i == 34)
		{
			x0 = 451 + MOVEINX;
			y0 = 130 + 81 + MOVEINY;
		}
		//stworzenie przycisku
		if (i == 29 || i == 32)
			keyButtons[i] = BUTTON_CreateAsChild(x0, y0, 60, 40, keyboardHandle, keyboardLettersId[i], WM_CF_HIDE);
		else if (i == 31 || i == 30 || i == 28 || i == 34)
			keyButtons[i] = BUTTON_CreateAsChild(x0, y0, 74, 40, keyboardHandle, keyboardLettersId[i], WM_CF_HIDE);
		else
			keyButtons[i] = BUTTON_CreateAsChild(x0, y0, 40, 40, keyboardHandle, keyboardLettersId[i], WM_CF_HIDE);
		BUTTON_SetText(keyButtons[i], charConv);
		BUTTON_SetFocussable(keyButtons[i], 0);
	}
	//stworzenie spacji
	keyButtons[33] = BUTTON_CreateAsChild(155 + MOVEINX, 130 + 81 + MOVEINY, 208, 40, keyboardHandle, keyboardLettersId[33], WM_CF_HIDE);
	BUTTON_SetText(keyButtons[33], " ");
	BUTTON_SetFocussable(keyButtons[33], 0);

	//nazwanie przyciskow funkcyjnych
	BUTTON_SetText(keyButtons[26], "←");
	BUTTON_SetText(keyButtons[27], "⇪");
	BUTTON_SetText(keyButtons[28], "⇧");
	BUTTON_SetText(keyButtons[29], "Alt");
	BUTTON_SetText(keyButtons[30], "");
	BUTTON_SetText(keyButtons[31], "⮠");
	BUTTON_SetText(keyButtons[32], "!@#");
	BUTTON_SetText(keyButtons[34], "");

	BUTTON_SetBitmapEx(keyButtons[30], 0, &bmOK, 19, 8);
	BUTTON_SetBitmapEx(keyButtons[34], 0, &bmNO, 24, 8);

	keyButtons[35] = BUTTON_CreateAsChild(10, 10, 40, 70, keyboardHandle, GUI_KEY_UP, WM_CF_SHOW);
	BUTTON_SetText(keyButtons[35], "▲");
	BUTTON_SetFocussable(keyButtons[35], 0);
	keyButtons[36] = BUTTON_CreateAsChild(686, 10, 40, 70, keyboardHandle, GUI_KEY_DOWN, WM_CF_SHOW);
	BUTTON_SetText(keyButtons[36], "▼");
	BUTTON_SetFocussable(keyButtons[36], 0);

	for (i = 0; i < NUMBER_OF_BUTTONS; i++)
	{
		WM_ShowWindow(keyButtons[i]);
	}
}

static void setNumButtons(BUTTON_Handle keyButtons[], WM_HWIN keyboardHandle)
{
	int x0 = 0;
	int y0 = 0;
	char charConvBuffer[4] =
	{ 0 };
	for (int i = 0; i < 16; i++)
	{
		if (alphanumericKeyboardSigns[i] >= 65536)
		{
			charConvBuffer[0] = (alphanumericKeyboardSigns[i] >> 16) & 0xff;
			charConvBuffer[1] = (alphanumericKeyboardSigns[i] >> 8) & 0xff;
			charConvBuffer[2] = (alphanumericKeyboardSigns[i]) & 0xff;
			charConvBuffer[3] = 0;
		}
		if (alphanumericKeyboardSigns[i] >= 256 && alphanumericKeyboardSigns[i] <= 65535)
		{
			charConvBuffer[0] = (alphanumericKeyboardSigns[i] >> 8) & 0xff;
			charConvBuffer[1] = (alphanumericKeyboardSigns[i]) & 0xff;
			charConvBuffer[2] = 0;
		}
		if (alphanumericKeyboardSigns[i] >= 0 && alphanumericKeyboardSigns[i] <= 255)
		{
			charConvBuffer[0] = (alphanumericKeyboardSigns[i]) & 0xff;
			charConvBuffer[1] = 0;
		}
		if (i >= 0 && i < 16)
		{
			x0 = 550 + 47 * (i % 4) + MOVEINX;
			y0 = 70 + 47 * (i / 4) + MOVEINY;
		}
		keyButtons[i] = BUTTON_CreateAsChild(x0, y0, 40, 40, keyboardHandle, alphanumericKeyboardText[i], WM_CF_SHOW);
		BUTTON_SetText(keyButtons[i], charConvBuffer);
		BUTTON_SetFocussable(keyButtons[i], 0);
	}
}

static void setNumKeyButtons(BUTTON_Handle keyButtons[], int keyboardLetters[], int keyboardLettersId[], WM_HWIN keyboardHandle)
{
	int x0 = 0;
	int y0 = 0;
	char charConvBuffer[4] = { 0 };
	for (int i = 0; i < 16; i++)
	{
		if(i==7)
			continue;

		if (keyboardLetters[i] >= 65536)
		{
			charConvBuffer[0] = (keyboardLetters[i] >> 16) & 0xff;
			charConvBuffer[1] = (keyboardLetters[i] >> 8) & 0xff;
			charConvBuffer[2] = (keyboardLetters[i]) & 0xff;
			charConvBuffer[3] = 0;
		}
		if (keyboardLetters[i] >= 256 && keyboardLetters[i] <= 65535)
		{
			charConvBuffer[0] = (keyboardLetters[i] >> 8) & 0xff;
			charConvBuffer[1] = (keyboardLetters[i]) & 0xff;
			charConvBuffer[2] = 0;
		}
		if (keyboardLetters[i] >= 0 && keyboardLetters[i] <= 255)
		{
			charConvBuffer[0] = (keyboardLetters[i]) & 0xff;
			charConvBuffer[1] = 0;
		}

		x0 = 202 + 85 * (i % 4);
		y0 = 90 + 45 * (i / 4);

		keyButtons[i] = BUTTON_CreateAsChild(x0, y0, 80, 40, keyboardHandle, keyboardLettersId[i], WM_CF_SHOW);
		BUTTON_SetText(keyButtons[i], charConvBuffer);
		BUTTON_SetFocussable(keyButtons[i], 0);
	}

	BUTTON_SetBitmapEx(keyButtons[11], 0, &bmOK, 21, 7);
	BUTTON_SetBitmapEx(keyButtons[15], 0, &bmNO, 27, 7);

}

static void setNumKeyButtonsExtended(BUTTON_Handle keyButtons[], int keyboardLetters[], int keyboardLettersId[], WM_HWIN keyboardHandle)
{
	int x0 = 0;
	int y0 = 0;
	char charConvBuffer[4] =
	{ 0 };
	for (int i = 0; i < 24; i++)
	{
		if (keyboardLetters[i] >= 65536)
		{
			charConvBuffer[0] = (keyboardLetters[i] >> 16) & 0xff;
			charConvBuffer[1] = (keyboardLetters[i] >> 8) & 0xff;
			charConvBuffer[2] = (keyboardLetters[i]) & 0xff;
			charConvBuffer[3] = 0;
		}
		if (keyboardLetters[i] >= 256 && keyboardLetters[i] <= 65535)
		{
			charConvBuffer[0] = (keyboardLetters[i] >> 8) & 0xff;
			charConvBuffer[1] = (keyboardLetters[i]) & 0xff;
			charConvBuffer[2] = 0;
		}
		if (keyboardLetters[i] >= 0 && keyboardLetters[i] <= 255)
		{
			charConvBuffer[0] = (keyboardLetters[i]) & 0xff;
			charConvBuffer[1] = 0;
		}

		x0 = 158 + 85 * (i % 6);
		y0 = 90 + 45 * (i / 6);

		keyButtons[i] = BUTTON_CreateAsChild(x0, y0, 80, 40, keyboardHandle, keyboardLettersId[i], WM_CF_SHOW);
		BUTTON_SetText(keyButtons[i], charConvBuffer);
		BUTTON_SetFocussable(keyButtons[i], 0);
	}

	BUTTON_SetBitmapEx(keyButtons[17], 0, &bmOK, 21, 7);
	BUTTON_SetBitmapEx(keyButtons[23], 0, &bmNO, 27, 7);

	keyButtons[24] = BUTTON_CreateAsChild(73, 90, 80, 40, keyboardHandle, keyboardLettersId[24], WM_CF_SHOW);
	BUTTON_SetText(keyButtons[24], "^");
	BUTTON_SetFocussable(keyButtons[24], 0);

	keyButtons[25] = BUTTON_CreateAsChild(10, 10, 40, 70, keyboardHandle, GUI_KEY_UP, WM_CF_SHOW);
	BUTTON_SetText(keyButtons[25], "▲");
	BUTTON_SetFocussable(keyButtons[25], 0);
	keyButtons[26] = BUTTON_CreateAsChild(686, 10, 40, 70, keyboardHandle, GUI_KEY_DOWN, WM_CF_SHOW);
	BUTTON_SetText(keyButtons[26], "▼");
	BUTTON_SetFocussable(keyButtons[26], 0);
}

void setKeyboard(WM_HWIN hcurrWidget, int idWidgetWin, int passMode, unsigned int MaxChars)
{
	WM_HWIN hItem;
	int CursorPosition = 0;

	passCheck = passMode;
	idWidgetType = idWidgetWin;
	MaxNumChars = MaxChars;
	flagBackspace = 0;

	hWidget = hcurrWidget;

	if (idWidgetType == MULTIEDIT)
	{
		CursorPosition = MULTIEDIT_GetCursorCharPos(hWidget) + 1;
		MULTIEDIT_GetText(hWidget, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
	}
	else if (idWidgetType == EDIT)
	{
		CursorPosition = EDIT_GetCursorCharPos(hWidget) + 1;
		EDIT_GetText(hWidget, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
	}
	else if (idWidgetType == LISTVIEW)
	{
		int col = LISTVIEW_GetSelCol(hWidget);
		int row = LISTVIEW_GetSel(hWidget);
		CursorPosition = LISTVIEW_GetItemTextLen(hWidget, col, row);
		LISTVIEW_GetItemText(hWidget, col, row, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
	}

	EraseWhiteSpaceFromString((char*)keyboardText);

	hKeyboardWindow = GUI_CreateDialogBox(_aKeyboardDialogCreate, GUI_COUNTOF(_aKeyboardDialogCreate), _cbKeyboardDialog, WM_HBKWIN, 0, 0);

	setButtons(keyButton, lowerSigns, lowerText, hKeyboardWindow);
	setNumButtons(keyAlphaNumButtons, hKeyboardWindow);

	hItem = WM_GetDialogItem(hKeyboardWindow, ID_MULTIEDIT_1);
	MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
	MULTIEDIT_SetMaxNumChars(hItem, MaxNumChars);
	if (passMode == PASSON)
		MULTIEDIT_SetPasswordMode(hItem, 1);
	MULTIEDIT_SetText(hItem, (char*)keyboardText);
	MULTIEDIT_SetCursorOffset(hItem, CursorPosition);

	WM_MakeModal(hKeyboardWindow);

}

void setNumKeyboard(WM_HWIN hcurrWidget, int idWidgetWin, unsigned int MaxChars)
{
	WM_HWIN hItem;
	int CursorPosition = 0;

	passCheck = PASSOFF;
	idWidgetType = idWidgetWin;
	flagBackspace = 0;

	hWidget = hcurrWidget;

	if (idWidgetType == MULTIEDIT)
	{
		CursorPosition = MULTIEDIT_GetCursorCharPos(hWidget);
		MULTIEDIT_GetText(hWidget, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
	}
	else if (idWidgetType == EDIT)
	{
		CursorPosition = EDIT_GetCursorCharPos(hWidget);
		EDIT_GetText(hWidget, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
	}
	else if (idWidgetType == LISTVIEW)
	{
		CursorPosition = LISTVIEW_GetItemTextLen(hWidget, LISTVIEW_GetSelCol(hWidget),  LISTVIEW_GetSel(hWidget));
		LISTVIEW_GetItemTextSorted(hWidget, LISTVIEW_GetSelCol(hWidget),  LISTVIEW_GetSel(hWidget), (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
	}

	hKeyboardWindow = GUI_CreateDialogBox(_aKeyboardDialogCreate, GUI_COUNTOF(_aKeyboardDialogCreate), _cbKeyboardDialog, WM_HBKWIN, 0, 0);

	setNumKeyButtons(keyNumButtons, numericKeyboardSigns, numericKeyboardText, hKeyboardWindow);

	hItem = WM_GetDialogItem(hKeyboardWindow, ID_MULTIEDIT_1);
	MULTIEDIT_SetFont(hItem, &GUI_FontLato60);
	if (MaxChars)
		MULTIEDIT_SetMaxNumChars(hItem, MaxChars);
	MULTIEDIT_SetText(hItem, (char*)keyboardText);
	MULTIEDIT_SetCursorOffset(hItem, CursorPosition);

	WM_MakeModal(hKeyboardWindow);
}

void setNumKeyboardExtended(WM_HWIN hcurrWidget, int idWidgetWin, unsigned int MaxChars)
{
	if(extendedKeyboardOn==0) {
		WM_HWIN hItem;
		int CursorPosition = 0;
		extendedKeyboardOn = 1;
		passCheck = PASSOFF;
		idWidgetType = idWidgetWin;
		flagBackspace = 0;

		hWidget = hcurrWidget;

		if (idWidgetType == MULTIEDIT)
		{
			CursorPosition = MULTIEDIT_GetCursorCharPos(hWidget);
			MULTIEDIT_GetText(hWidget, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
		}
		else if (idWidgetType == EDIT)
		{
			CursorPosition = EDIT_GetCursorCharPos(hWidget);
			EDIT_GetText(hWidget, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
		}
		else if (idWidgetType == LISTVIEW)
		{
			CursorPosition = LISTVIEW_GetItemTextLen(hWidget, LISTVIEW_GetSelCol(hWidget),  LISTVIEW_GetSel(hWidget));
			LISTVIEW_GetItemTextSorted(hWidget, LISTVIEW_GetSelCol(hWidget),  LISTVIEW_GetSel(hWidget), (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
		}


		hKeyboardWindow = GUI_CreateDialogBox(_aKeyboardDialogCreate, GUI_COUNTOF(_aKeyboardDialogCreate), _cbKeyboardDialog, WM_HBKWIN, 0, 0);

		setNumKeyButtonsExtended(keyNumButtonsExtended, numericKeyboardSignsExtended, numericKeyboardTextExtended, hKeyboardWindow);

		hItem = WM_GetDialogItem(hKeyboardWindow, ID_MULTIEDIT_1);
		MULTIEDIT_SetFont(hItem, &GUI_FontLato30);
		if (MaxChars)
			MULTIEDIT_SetMaxNumChars(hItem, MaxChars);
		MULTIEDIT_SetText(hItem, (char*)keyboardText);
		MULTIEDIT_SetCursorOffset(hItem, CursorPosition);

		WM_MakeModal(hKeyboardWindow);
	}
}

static void CloseKeyboardWindow(void)
{
	WM_HWIN hItem;
	if (hKeyboardWindow)
	{
		hItem = WM_GetDialogItem(hKeyboardWindow, ID_MULTIEDIT_1);
		//wysylanie tekstu do okna inicjalizujacego
		MULTIEDIT_GetText(hItem, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
		if (!(passCheck == PASSON))
			CorrectNullString((char*)keyboardText);

		if (idWidgetType == EDIT)
			EDIT_SetText(hWidget, (char*)keyboardText);

		if (idWidgetType == MULTIEDIT)
			MULTIEDIT_SetText(hWidget, (char*)keyboardText);

		if (idWidgetType == LISTVIEW) {
			 LISTVIEW_SetItemText(hWidget, LISTVIEW_GetSelCol(hWidget),  LISTVIEW_GetSelUnsorted(hWidget), (char*)keyboardText);
			 WM_InvalidateWindow(hWidget);
		}
		WM_MakeModal(0);

		WM_DeleteWindow(hKeyboardWindow);
		hKeyboardWindow = 0;
	}
}

static void CloseKeyboardWindowFormula(void)
{
	WM_HWIN hItem;
	char formula[FORMULA_USER_SIZE];
	if (hKeyboardWindow)
	{
		hItem = WM_GetDialogItem(hKeyboardWindow, ID_MULTIEDIT_1);
		//wysylanie tekstu do okna inicjalizujacego
		MULTIEDIT_GetText(hItem, (char*)keyboardText, KEYBOARD_TEXT_NUMBER);
		convertSpecialCase(formula, (char*)keyboardText);
		if(CheckFormulaCorrectness(formula))
				MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_EDIT, GUI_RED);
		else {
			if (!(passCheck == PASSON))
				CorrectNullString((char*)keyboardText);

			if (idWidgetType == EDIT)
				EDIT_SetText(hWidget, (char*)keyboardText);

			if (idWidgetType == MULTIEDIT)
				MULTIEDIT_SetText(hWidget, (char*)keyboardText);

			if (idWidgetType == LISTVIEW) {
				 LISTVIEW_SetItemText(hWidget, LISTVIEW_GetSelCol(hWidget),  LISTVIEW_GetSelUnsorted(hWidget), (char*)keyboardText);
				 WM_InvalidateWindow(hWidget);
			}

			WM_MakeModal(0);

			WM_DeleteWindow(hKeyboardWindow);
			extendedKeyboardOn = 0;
			hKeyboardWindow = 0;
			}
	}
}

WM_HWIN GetKeyboardHandle(void)
{
	return hKeyboardWindow;
}

void deleteKeyboard(void)
{
	if (hKeyboardWindow)
	{
		WM_DeleteWindow(hKeyboardWindow);
		extendedKeyboardOn = 0;
		hKeyboardWindow = 0;
	}
}

static void setLanguage(uint8_t deviceLanguage, int** upperSignsLanguage, int** upperTextLanguage, int** lowerSignsLanguage,
		int** lowerTextLanguage)
{
	switch (deviceLanguage)
	{
	case 0:
		*upperSignsLanguage = upperSigns;
		*upperTextLanguage = upperText;
		*lowerSignsLanguage = lowerSigns;
		*lowerTextLanguage = lowerText;
		break;
	case 1:
		*upperSignsLanguage = deutschUpperSigns;
		*upperTextLanguage = deutschUpperText;
		*lowerSignsLanguage = deutschLowerSigns;
		*lowerTextLanguage = deutschLowerText;
		break;
	case 2:
		*upperSignsLanguage = spanishUpperSigns;
		*upperTextLanguage = spanishUpperText;
		*lowerSignsLanguage = spanishLowerSigns;
		*lowerTextLanguage = spanishLowerText;
		break;
	case 3:
		*upperSignsLanguage = frenchUpperSigns;
		*upperTextLanguage = frenchUpperText;
		*lowerSignsLanguage = frenchLowerSigns;
		*lowerTextLanguage = frenchLowerText;
		break;
	case 4:
		*upperSignsLanguage = italianUpperSigns;
		*upperTextLanguage = italianUpperText;
		*lowerSignsLanguage = italianLowerSigns;
		*lowerTextLanguage = italianLowerText;
		break;
	case 5:
		*upperSignsLanguage = polishUpperSigns;
		*upperTextLanguage = polishUpperText;
		*lowerSignsLanguage = polishLowerSigns;
		*lowerTextLanguage = polishLowerText;
		break;
	case 6:
		*upperSignsLanguage = portugueseUpperSigns;
		*upperTextLanguage = portugueseUpperText;
		*lowerSignsLanguage = portugueseLowerSigns;
		*lowerTextLanguage = portugueseLowerText;
		break;
	default:
		*upperSignsLanguage = upperSigns;
		*upperTextLanguage = upperText;
		*lowerSignsLanguage = lowerSigns;
		*lowerTextLanguage = lowerText;
	break;
	}
}

static void EraseWhiteSpaceFromString(char* stringToCheck)
{
	if (*stringToCheck == ' ' && *(++stringToCheck) == '\0')
	{
		*(--stringToCheck) = '\0';
	}
}

void CorrectNullString(char* stringToCorrect)
{
	if (*stringToCorrect == '\0')
	{
		*(stringToCorrect++) = ' ';
		*stringToCorrect = '\0';
	}
}

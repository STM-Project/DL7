/*
 * KeyboardDLG.h
 *
 *  Created on: 12.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_KEYBOARDDLG_H_
#define GUI_KEYBOARDDLG_H_

#include "DIALOG.h"

#define NUMBER_OF_BUTTONS 37
#define KEYBOARD_TEXT_NUMBER 0x800
#define EDIT			100
#define MULTIEDIT		101
#define PASSON   102
#define PASSOFF   103
#define PASSWORD   102
#define LISTVIEW 104

void setKeyboard(WM_HWIN hcurrWidget, int idWidgetWin, int passMode, unsigned int MaxChars);
void setNumKeyboard(WM_HWIN hcurrWidget, int idWidgetWin, unsigned int MaxChars);
void setNumKeyboardExtended(WM_HWIN hcurrWidget, int idWidgetWin, unsigned int MaxChars);
void deleteKeyboard(void);
WM_HWIN GetKeyboardHandle(void);

void CorrectNullString(char* stringToCorrect);

#endif /* GUI_KEYBOARDDLG_H_ */

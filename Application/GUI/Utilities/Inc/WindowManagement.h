/*
 * WindowManagment.h
 *
 *  Created on: 12.11.2020
 *      Author: RafalMar
 */

#ifndef GUI_UTILITIES_WINDOWMANAGEMENT_H_
#define GUI_UTILITIES_WINDOWMANAGEMENT_H_

#include "DIALOG.h"

#define INFO      		1
#define USERVALUES		2
#define USERTRENDS 		3
#define ARCHIVE     	4
#define SETTINGS    	5
#define SINGLE_VALUE	6
#define ALARMS				7

extern int WindowsState;
extern int WindowInstance;
extern WM_HWIN hActualWindow;

void CreateGUITask(void);
WM_HWIN ChangeWindow(WM_HWIN ActiveWindow, int NewWindowState);

#endif /* GUI_UTILITIES_WINDOWMANAGEMENT_H_ */

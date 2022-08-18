/*
 * PopUpMessageDLG.h
 *
 *  Created on: 12.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_POPUPMESSAGEDLG_H_
#define GUI_POPUPMESSAGEDLG_H_

#include "DIALOG.h"

#define DEFAULT_COLOR 0xFFFFFFFF

typedef void MESSAGE_FUNCTION(void);

void CreateMessage(const char * pText, GUI_COLOR bkColor, GUI_COLOR textColor);
WM_HWIN CreateConfirmMessage(const char * pText, GUI_COLOR bkColor, GUI_COLOR textColor, MESSAGE_FUNCTION *accept,MESSAGE_FUNCTION *cancel);

#endif /* GUI_POPUPMESSAGEDLG_H_ */

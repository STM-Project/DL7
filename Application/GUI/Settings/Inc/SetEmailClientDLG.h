/*
 * SetEmailClientDLG.h
 *
 *  Created on: 26.02.2019
 *      Author: TomaszSok
 */

#ifndef GUI_SETTINGS_SETEMAILCLIENTDLG_H_
#define GUI_SETTINGS_SETEMAILCLIENTDLG_H_

#include "DIALOG.h"
#include "smtp_netconn.h"

extern EMAIL_SETTINGS tempEmailSettings;

int EMAIL_UpdateEmailParameters(MULTIPAGE_Handle hObj);

WM_HWIN CreateSetEmailParameters(WM_HWIN hParent);
WM_HWIN CreateSetEmailRecivers(WM_HWIN hParent);
WM_HWIN CreateSetEmailReportParameters(WM_HWIN hParent);


#endif /* GUI_SETTINGS_SETEMAILCLIENTDLG_H_ */

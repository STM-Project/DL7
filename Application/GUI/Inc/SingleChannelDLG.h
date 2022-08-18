/*
 * SingleChannelDLG.h
 *
 *  Created on: 15.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_SINGLECHANNELDLG_H_
#define GUI_SINGLECHANNELDLG_H_

#include "DIALOG.h"
#include "channels_typedef.h"

WM_HWIN CreateSingleChannelWindow(int instance);

void SetChannelValue(WM_HWIN hEdit, CHANNEL * kanal);

#endif /* GUI_SINGLECHANNELDLG_H_ */

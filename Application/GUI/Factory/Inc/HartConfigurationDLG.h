/*
 * HartConfigurationDLG.h
 *
 *  Created on: 03.05.2020
 *      Author: RM
 */

#ifndef GUI_FACTORY_HARTCONFIGURATIONDLG_H_
#define GUI_FACTORY_HARTCONFIGURATIONDLG_H_

#include "DIALOG.h"
#include <stdint.h>

WM_HWIN CreateHARTConfigurationWin(WM_HWIN hParent, uint8_t boardNumber, uint8_t InputsMode);

#endif /* GUI_FACTORY_HARTCONFIGURATIONDLG_H_ */

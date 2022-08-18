/*
 * CardCalibrationDLG.h
 *
 *  Created on: 15.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_FACTORY_CARDCALIBRATIONDLG_H_
#define GUI_FACTORY_CARDCALIBRATIONDLG_H_

#include "DIALOG.h"
#include <stdint.h>

WM_HWIN CreateCalibrationWin(WM_HWIN hParent, uint8_t boardNumber, uint8_t InputsMode);

#endif /* GUI_FACTORY_CARDCALIBRATIONDLG_H_ */

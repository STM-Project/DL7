/*
 * CardsBoot.h
 *
 *  Created on: 25.08.2020
 *      Author: RafalMar
 */

#ifndef GUI_FACTORY_UPGRADEBOARDFIRMWAREDLG_H_
#define GUI_FACTORY_UPGRADEBOARDFIRMWAREDLG_H_


#include "DIALOG.h"
#include "WM.h"
#include <stdint.h>

extern WM_HWIN CardBootWindow;

WM_HWIN CreateCardBootWin(WM_HWIN hParent, uint8_t boardNumber);
uint8_t GetBoardNumber(void);

#endif /* GUI_FACTORY_UPGRADEBOARDFIRMWAREDLG_H_ */

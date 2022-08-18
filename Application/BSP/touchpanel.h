/*
 * touchpanel.h
 *
 *  Created on: 23.03.2018
 *      Author: Tomaszs
 */

#ifndef TOUCHPANEL_H_
#define TOUCHPANEL_H_

#include <stdint.h>

void TOUCHPANEL_Init(void);
void TOUCHPANEL_UpdateState(void);

void TP_CALIBRATION_SetDefaultsCoef(void);

void TP_CALIBARION_GetRawValues(uint16_t* X,uint16_t* Y);
void TP_CALIBARION_UpdateCoefs(float AX,float BX, float AY, float BY);
void TP_CALIBRATION_WriteCoefToFRAM(void);
void TP_CALIBRATION_ReadCoefFromFRAM(void);

#endif /* TOUCHPANEL_H_ */

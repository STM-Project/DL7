/*
 * Alarms.h
 *
 *  Created on: 7 sie 2015
 *      Author: Tomaszs
 */

#ifndef ALARMS_H_
#define ALARMS_H_

#include <stdint.h>

void CreateAlarmsTask(void);
uint32_t ALARMS_GetColor(uint8_t AlarmColor);
void ALARMS_AckAll(void);
uint8_t ALARMS_GetState(void);

#endif /* ALARMS_H_ */

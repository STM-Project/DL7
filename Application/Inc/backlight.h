/*
 * backlight.h
 *
 *  Created on: 12.11.2020
 *      Author: RafalMar
 */

#ifndef INC_BACKLIGHT_H_
#define INC_BACKLIGHT_H_

#include <stdint.h>

void BACKLIGHT_Init(void);
void BACKLIGHT_Set(int level);
uint8_t BACKLIGHT_Get(void);

#endif /* INC_BACKLIGHT_H_ */

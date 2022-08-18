/*
 * StartUp.h
 *
 *  Created on: 22.02.2017
 *      Author: Tomaszs
 */

#ifndef STARTUP_H_
#define STARTUP_H_

#include <stdint.h>

void STARTUP_Init(void);

uint32_t STARTUP_WaitForBits(uint32_t bits);

void STARTUP_SetBits(uint32_t bits);

void STARTUP_ClaerBits(uint32_t bits);

#endif /* STARTUP_H_ */

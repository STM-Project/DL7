/*
 * shift_reg.c
 *
 *  Created on: 09.11.2020
 *      Author: RafalMar
 */

#include "spi.h"
#include "shift_reg.h"

uint16_t SHIFT_REG_DATA = 0;

void ShiftReg_WriteVal(uint16_t data)
{
	SPI_Transmit_IT((uint8_t*)&data, 1);
}

void ShiftReg_Init(void)
{
	ShiftReg_SPI_Init();
	ShiftReg_WriteVal(0x0020);
	SHIFT_REG_OE_ON;
}

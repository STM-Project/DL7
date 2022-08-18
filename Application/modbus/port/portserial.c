/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include <stdlib.h>
#include "port.h"
#include "stm32h7xx_hal.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "usart.h"

extern COM_SETTINGS ComSettings __attribute__ ((section(".sdram")));
int interruptFlag = 0;

static void MODBUS_RTS_Set(void)
{
	HAL_GPIO_WritePin(RS2_RTS_GPIO_Port, RS2_RTS_Pin, GPIO_PIN_SET);
}

static void MODBUS_RTS_Reset(void)
{
	HAL_GPIO_WritePin(RS2_RTS_GPIO_Port, RS2_RTS_Pin, GPIO_PIN_RESET);
}

/* ----------------------- Enable USART interrupts -----------------------------*/
void
vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	/* If xRXEnable enable serial receive interrupts. If xTxENable enable
	 * transmitter empty interrupts.
	 */
	if (xRxEnable)
	{
		MODBUS_RTS_Reset();
		MODBUS_DisableTxIT();
		MODBUS_EnableRxIT();
		interruptFlag = 1;
	}
	else
	{
		MODBUS_RTS_Set();
		MODBUS_DisableRxIT();
	}

	if (xTxEnable)
	{
		MODBUS_RTS_Set();
		MODBUS_DisableRxIT();
		MODBUS_EnableTxIT();
		interruptFlag = 2;
	}
	else
	{
		MODBUS_RTS_Reset();
		MODBUS_DisableTxIT();
	}
}

uint32_t ComSetBaudrate(short Baudrate)
{
	switch (Baudrate)
	{
	case 0:
		return 2400;
		break;
	case 1:
		return 4800;
		break;
	case 2:
		return 9600;
		break;
	case 3:
		return 19200;
		break;
	case 4:
		return 38400;
		break;
	case 5:
		return 57600;
		break;
	case 6:
		return 115200;
		break;
	default:
		return 19200;
	}
}

/* ----------------------- Initialize USART ----------------------------------*/
/* Called with databits = 8 for RTU */

BOOL
xMBPortSerialInit()
{
	return MODBUS_UartInit(ComSetBaudrate(ComSettings.ComBaudrate), ComSettings.ComParity);
}

/* -----------------------Send character  ----------------------------------*/
BOOL
xMBPortSerialPutByte(CHAR ucByte)
{
	/* Put a byte in the UARTs transmit buffer. This function is called
	 * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
	 * called. */
	MODBUS_SendByte((uint8_t *) &ucByte);
	return TRUE;
}

/* ----------------------- Get character ----------------------------------*/
BOOL
xMBPortSerialGetByte(CHAR * pucByte)
{
	/* Return the byte in the UARTs receive buffer. This function is called
	 * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
	 */
	MODBUS_RecvByte((uint8_t *)pucByte);
	return TRUE;
}

/* ----------------------- Close Serial Port ----------------------------------*/
void
vMBPortSerialClose(void)
{
	MODBUS_PortSerialClose();
}


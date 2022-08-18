/*
 * FreeModbus Libary: STM32F103 over FREERTOS
 * Copyright (C) 2012 Ken Sarkies <ksarkies@internode.on.net>
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: modbus.c,v 1.0 2012/09/26 Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include <channels.h>
#include <mb.h>
#include <mbport.h>
#include <watchdog.h>
#include "modbus.h"
#include "StartUp.h"
#include "ChannelsSynchronization.h"

#include "usart.h"

xTaskHandle vtaskModbusRTUHandle;
/* ----------------------- Defines ------------------------------------------
 * Adresy poszczególnych wartosci w protokole MODBUS      */
#define REG_INPUT_START 000
#define REG_INPUT_NREGS 1000

#define REG_HOLDING_START       000
#define REG_HOLDING_NREGS       1000

#define REG_COIL_START (3000+1)
#define REG_COIL_NREGS 8
#define REG_COIL_BYTES REG_COIL_NREGS/8

#define REG_DISC_START (4000+1)
#define REG_DISC_NREGS 8
#define REG_DISC_BYTES REG_DISC_NREGS/8

/* ----------------------- Static variables ---------------------------------*/
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS] __attribute__ ((section(".sdram")));
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS] __attribute__ ((section(".sdram")));
static USHORT usRegCoilStart = REG_COIL_START;
static UCHAR usRegCoilBuf[REG_COIL_BYTES];
static USHORT usRegDiscStart = REG_DISC_START;
static UCHAR usRegDiscBuf[REG_DISC_BYTES];

int iRegIndex;
union MeasureValue
{
	float floatValue;
	double doubleValue;
} measureValue;


/* ----------------------- Start implementation -----------------------------*/
void
vtaskModbusRTU(void *pvParameters)
{

	const UCHAR ucSlaveID[] =
	{ 0xAA, 0xBB, 0xCC };
	eMBErrorCode eStatus;
	while (1)
	{
		if (STARTUP_WaitForBits(0x0080))
		{

			STARTUP_ClaerBits( 0x0080);
			STARTUP_SetBits( 0x0100);
			for (;;)
			{

				if ((eStatus = eMBInit()) != MB_ENOERR)
				{
					/* Can not initialize. Add error handling code here. */
				}
				else
				{
					if ((eStatus = eMBSetSlaveID(0x34, TRUE, ucSlaveID, 3)) != MB_ENOERR)
					{
						/* Can not set slave id. Check arguments */
					}
					else if ((eStatus = eMBEnable()) != MB_ENOERR)
					{
						/* Enable failed. */
					}
					else
					{
						do //główna pętla zadania
						{
							eMBPoll();
							/* Here we simply count the number of poll cycles. */
							usRegInputBuf[0]++;

							WDFlags[7] = ALIVE;
							vTaskDelay(200);
						}
						while (1);

						eMBDisable();
						eMBClose();
					}
				}
				WDFlags[7] = ALIVE;
				vTaskDelay(200);
			}
		}
		else
			continue;
	}
}

void CreateModbusRTUTask(void)
{
	xTaskCreate(vtaskModbusRTU, "vtaskModbusRTU", 2000, NULL, ( unsigned portBASE_TYPE )4, &vtaskModbusRTUHandle);
}

/* ----------------------- Get values of registers -----------------------------*/

/* Callback function used if the value of a Input Register is required by the
 protocol stack.
 The starting register address is given by usAddress and the last register is given by
 usAddress + usNRegs - 1. */

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
	eMBErrorCode eStatus = MB_ENOERR;

	if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int) (usAddress - usRegInputStart);

		if ( TakeChannelsMutex(1000))
		{
			for (int n = 0; n <= usNRegs - 1; n++)
			{
				// Realizacja żądania zgodnie z wartosciami ramki mastera
				// Adresy 1 - 199: wynik pomiaru
				// Adresy 201 - 399: licznik 1
				// Adresy 401 - 599: licznik 2
				if (iRegIndex + n <= 199)
				{
					int byteNum = (iRegIndex + n) % 2;

					measureValue.floatValue = Channels[((iRegIndex) + n) / 2].value;
					uint32_t convValue;
					convValue = *((uint32_t *) &measureValue.floatValue);
					usRegInputBuf[iRegIndex + n] = (convValue >> (8 * byteNum * 2)) & 0xffff;
				}
				else if (iRegIndex + n <= 599)
				{
					measureValue.doubleValue = Channels[((iRegIndex - 200) + n) / 4].Tot1.value;

					int byteNum = (iRegIndex + n) % 4;

					uint64_t convValue;
					convValue = *((uint64_t *) &measureValue.doubleValue);
					usRegInputBuf[iRegIndex + n] = (convValue >> (8 * byteNum * 2)) & 0xffff;

				}
				else if (iRegIndex + n <= 999)
				{
					measureValue.doubleValue = Channels[((iRegIndex - 600) + n) / 4].Tot2.value;

					int byteNum = (iRegIndex + n) % 4;
					uint64_t convValue;
					convValue = *((uint64_t *) &measureValue.doubleValue);
					usRegInputBuf[iRegIndex + n] = (convValue >> (8 * byteNum * 2)) & 0xffff;
				}
			}
			GiveChannelsMutex();
		}
		else
		{
			eStatus = MB_ETIMEDOUT;
			return eStatus;
		}

		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (unsigned char) (usRegInputBuf[iRegIndex] >> 8);
			*pucRegBuffer++ = (unsigned char) (usRegInputBuf[iRegIndex] & 0xFF);
			iRegIndex++;
			usNRegs--;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iRegIndex;

	if ((usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
	{
		iRegIndex = (int) (usAddress - usRegHoldingStart);
		switch (eMode)
		{
		/* Pass current register values to the protocol stack. */
		case MB_REG_READ:

			if ( TakeChannelsMutex(1000))
			{
				for (int n = 0; n <= usNRegs - 1; n++)
				{
					// Realizacja żądania zgodnie z wartosciami ramki mastera
					// Adresy 1 - 199: wynik pomiaru
					// Adresy 201 - 399: licznik 1
					// Adresy 401 - 599: licznik 2
					if (iRegIndex + n <= 199)
					{
						int byteNum = (iRegIndex + n) % 2;

						measureValue.floatValue = Channels[((iRegIndex) + n) / 2].value;
						uint32_t convValue;
						convValue = *((uint32_t *) &measureValue.floatValue);
						usRegHoldingBuf[iRegIndex + n] = (convValue >> (8 * byteNum * 2)) & 0xffff;
					}
					else if (iRegIndex + n <= 599)
					{
						measureValue.doubleValue = Channels[((iRegIndex - 200) + n) / 4].Tot1.value;

						int byteNum = (iRegIndex + n) % 4;

						uint64_t convValue;
						convValue = *((uint64_t *) &measureValue.doubleValue);
						usRegHoldingBuf[iRegIndex + n] = (convValue >> (8 * byteNum * 2)) & 0xffff;
					}
					else if (iRegIndex + n <= 999)
					{
						measureValue.doubleValue = Channels[((iRegIndex - 600) + n) / 4].Tot2.value;

						int byteNum = (iRegIndex + n) % 4;
						uint64_t convValue;
						convValue = *((uint64_t *) &measureValue.doubleValue);
						usRegHoldingBuf[iRegIndex + n] = (convValue >> (8 * byteNum * 2)) & 0xffff;
					}
				}
				GiveChannelsMutex();
			}
			else
			{
				eStatus = MB_ETIMEDOUT;
				return eStatus;
			}

			while (usNRegs > 0)
			{
				*pucRegBuffer++ = (UCHAR) (usRegHoldingBuf[iRegIndex] >> 8);
				*pucRegBuffer++ = (UCHAR) (usRegHoldingBuf[iRegIndex] & 0xFF);
				iRegIndex++;
				usNRegs--;
			}
			break;

			/* Update current register values with new values from the
			 * protocol stack. */
		case MB_REG_WRITE:
			while (usNRegs > 0)
			{
				usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

/*
 * Following implementation is not actually checked.
 */

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iIntRegIndex;
	int iIntBufNum;
	int iIntBitNum;
	int iExtRegIndex = 0;
	int iExtBufNum;
	int iExtBitNum;
	UCHAR ucTemp;
	if ((usAddress >= REG_COIL_START) && (usAddress + usNCoils <= REG_COIL_START + REG_COIL_NREGS))
	{
		iIntRegIndex = (int) (usAddress - usRegCoilStart);

		while (usNCoils > 0)
		{
			iIntBufNum = iIntRegIndex / 8;
			iIntBitNum = iIntRegIndex % 8;
			iExtBufNum = iExtRegIndex / 8;
			iExtBitNum = iExtRegIndex % 8;

			switch (eMode)
			{
			case MB_REG_READ:
				// Read coils
				if (iExtBitNum == 0)
				{
					pucRegBuffer[iExtBufNum] = 0;
				}
				ucTemp = (usRegCoilBuf[iIntBufNum] >> iIntBitNum) & 1;
				pucRegBuffer[iExtBufNum] |= ucTemp << iExtBitNum;
				break;

			case MB_REG_WRITE:
				// Write coils
				ucTemp = usRegCoilBuf[iIntBufNum] & (~(1 << iIntBitNum));
				ucTemp |= ((pucRegBuffer[iExtBufNum] >> iExtBitNum) & 1) << iIntBitNum;
				usRegCoilBuf[iIntBufNum] = ucTemp;
				break;
			}
			iIntRegIndex++;
			iExtRegIndex++;
			usNCoils--;

		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iIntRegIndex;
	int iIntBufNum;
	int iIntBitNum;
	int iExtRegIndex = 0;
	int iExtBufNum;
	int iExtBitNum;
	UCHAR ucTemp;
	if ((usAddress >= REG_DISC_START) && (usAddress + usNDiscrete <= REG_DISC_START + REG_DISC_NREGS))
	{
		iIntRegIndex = (int) (usAddress - usRegDiscStart);

		while (usNDiscrete > 0)
		{
			iIntBufNum = iIntRegIndex / 8;
			iIntBitNum = iIntRegIndex % 8;
			iExtBufNum = iExtRegIndex / 8;
			iExtBitNum = iExtRegIndex % 8;

			// Read discrete inputs
			if (iExtBitNum == 0)
			{
				pucRegBuffer[iExtBufNum] = 0;
			}
			ucTemp = (usRegDiscBuf[iIntBufNum] >> iIntBitNum) & 1;
			pucRegBuffer[iExtBufNum] |= ucTemp << iExtBitNum;

			iIntRegIndex++;
			iExtRegIndex++;
			usNDiscrete--;

		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

eMBErrorCode eMBDiagCB()
{

	return 0;
}

void MODBUS_TimerExpired(void)
{
	pxMBPortCBTimerExpired();
}

extern int interruptFlag;

void MODBUS_UartHandler(void)
{
	if (__HAL_UART_GET_FLAG(&huart6, UART_FLAG_PE))
	{
		__HAL_UART_FLUSH_DRREGISTER(&huart6);

	}
	else if (interruptFlag == 1)
	{
		pxMBFrameCBByteReceived();
	}
	else if (interruptFlag == 2)
		pxMBFrameCBTransmitterEmpty();
}


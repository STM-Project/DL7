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
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "tim.h"

extern COM_SETTINGS ComSettings __attribute__ ((section(".sdram")));

static uint32_t SetTimer35Period(short baudrate)
{
	switch(baudrate)
	{
	case 0:
		return 3209;
		break;
	case 1:
		return 1605;
		break;
	case 2:
		return 803;
		break;
	case 3:
		return 402;
		break;
	case 4:
		return 201;
		break;
	case 5:
		return 101;
		break;
	case 6:
		return 67;
		break;
	case 7:
		return 34;
		break;
	default:
		return 201;
	}
}

/* ----------------------- Initialize Timer -----------------------------*/

BOOL xMBPortTimersInit()
{
	 return MODBUS_TimerInit(SetTimer35Period(ComSettings.ComBaudrate));
}
int countereset = 0;
/* ----------------------- Enable Timer -----------------------------*/
inline void vMBPortTimersEnable()
{
 /* Restart the timer with the period value set in xMBPortTimersInit( ) */
	MODBUS_TimersEnable();
 countereset++;
}

/* ----------------------- Disable timer -----------------------------*/
inline void vMBPortTimersDisable()
{
	MODBUS_TimersDisable();
}

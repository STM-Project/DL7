/*
 * FreeModbus Libary: mbed Port
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
 * File: $Id: porttcp.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
 *
 * modified by Yuji Hosogaya 3-16-2012
 *
 * Originally this file was porttcp.c, then combined with other files
 * in order to make a light version of Modbus TCP.
 */
#include "mbtcp.h"
#include <string.h>
#include "mb.h"

#define MODBUS_THREAD_PRIO  4
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwipopts.h"

void modbusClientThread(void *mbconn);
/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT 502 /* TCP listening port. */
#define MB_TCP_BUF_SIZE     ( 256 + 7 ) /* Must hold a complete Modbus TCP frame. */
#define MB_MAX_TCP_CLIENTS 4

/* ----------------------- Static variables ---------------------------------*/
static UCHAR aucTCPBuf[MB_TCP_BUF_SIZE];
static USHORT usTCPBufPos;

int8_t mbconnNum = 0;

void EventPost_ex(struct netconn *mbconn)
{
	USHORT usAddress, usNRegs;
	UCHAR txbuf[MB_TCP_BUF_SIZE];
	int len = 0;
	UCHAR ucTemp;
	MB_STRUCT *rxmbs = (MB_STRUCT *) aucTCPBuf;
	MB_STRUCT *txmbs = (MB_STRUCT *) txbuf;
	MB_WORD_REQ *wd_req = (MB_WORD_REQ *) GET_DATA_PTR(rxmbs);
	MB_WORD_RES *wd_res = (MB_WORD_RES *) GET_DATA_PTR(txmbs);

	usAddress = TO_USHORT(wd_req->ADDR_H, wd_req->ADDR_L);
	usNRegs = TO_USHORT(wd_req->LEN_H, wd_req->LEN_L);
	switch (GET_FUNC(rxmbs))
	{
	case FC_RD_COILS:
		memcpy((char*) txmbs, (char*) rxmbs, 8);
		wd_res->BYTES = usNRegs * 2;
		len = usNRegs / 8;
		if (usNRegs % 8 != 0)
			len++;
		SET_LEN(txmbs, len + 3)
		;
		wd_res->BYTES = (UCHAR) len;

		if (eMBRegCoilsCB((UCHAR *) &wd_res->DATA, usAddress, usNRegs, MB_REG_READ) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x81);
			aucTCPBuf[8] = 0x02;
		}
		len += 9;
		break;

	case FC_RD_DISC_INPUTS:
		memcpy((char*) txmbs, (char*) rxmbs, 8);
		wd_res->BYTES = usNRegs * 2;
		len = usNRegs / 8;
		if (usNRegs % 8 != 0)
			len++;
		SET_LEN(txmbs, len + 3)
		;
		wd_res->BYTES = (UCHAR) len;

		if (eMBRegDiscreteCB((UCHAR *) &wd_res->DATA, usAddress, usNRegs) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x82);
			aucTCPBuf[8] = 0x02;
		}
		len += 9;
		break;

	case FC_RD_HOLDING_REGS:
		memcpy((char*) txmbs, (char*) rxmbs, 8);
		wd_res->BYTES = usNRegs * 2;
		len = usNRegs * 2;
		SET_LEN(txmbs, len + 3)
		;

		if (eMBRegHoldingCB((UCHAR *) &wd_res->DATA, usAddress, usNRegs, MB_REG_READ) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x83);
			aucTCPBuf[8] = 0x02;
		}
		len += 9;
		break;
	case FC_RD_INPUT_REGS:
		memcpy((char*) txmbs, (char*) rxmbs, 8);
		wd_res->BYTES = usNRegs * 2;
		len = usNRegs * 2;
		SET_LEN(txmbs, len + 3)
		;

		if (eMBRegInputCB((UCHAR *) &wd_res->DATA, usAddress, usNRegs) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x84);
			SET_LEN(txmbs, 3);
			SET_DATA(txmbs, 0x02);
			len += 5;
		}
		else
		{
			len += 9;
		}

		break;

	case FC_WR_SINGLE_COIL:
		ucTemp = (wd_req->LEN_H == 0x00) ? 0 : 1;
		len = 12;
		memcpy((char*) txmbs, (char*) rxmbs, len);
		if (eMBRegCoilsCB(&ucTemp, usAddress, 1, MB_REG_WRITE) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x85);
			aucTCPBuf[8] = 0x02;
		}
		break;

	case FC_WR_SINGLE_REG:
		len = 12;
		memcpy((char*) txmbs, (char*) rxmbs, 12);

		if (eMBRegHoldingCB(&wd_req->LEN_H, usAddress, 1, MB_REG_WRITE) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x86);
			aucTCPBuf[8] = 0x02;
		}
		break;

	case FC_WR_MULTI_COILS:
		len = 12;
		memcpy((char*) txmbs, (char*) rxmbs, len);
		SET_LEN(txmbs, 6)
		;

		if (eMBRegCoilsCB(&wd_req->DATA[1], usAddress, usNRegs, MB_REG_WRITE) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x8F);
			aucTCPBuf[8] = 0x02;
		}
		break;

	case FC_WR_MULTI_REGS:
		len = 12;
		memcpy((char*) txmbs, (char*) rxmbs, len);
		SET_LEN(txmbs, 6)
		;

		if (eMBRegHoldingCB(&wd_req->DATA[1], usAddress, usNRegs, MB_REG_WRITE) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x88);
			aucTCPBuf[8] = 0x02;
		}
		break;
	case FC_RD_WR_MULTI_REGS:
		memcpy((char*) txmbs, (char*) rxmbs, 8);

		if (eMBRegHoldingCB(&wd_req->DATA[5], usAddress, usNRegs, MB_REG_WRITE) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x89);
			aucTCPBuf[8] = 0x01;
		}

		usAddress = TO_USHORT(wd_req->DATA[0],wd_req->DATA[1]) + 1;
		usNRegs = TO_USHORT(wd_req->DATA[2], wd_req->DATA[3]);
		if (eMBRegHoldingCB(&wd_res->DATA, usAddress, usNRegs, MB_REG_READ) != MB_ENOERR)
		{
			SET_FUNC(txmbs, 0x89);
			aucTCPBuf[8] = 0x01;
		}
		len = usNRegs * 2;
		wd_res->BYTES = len;
		SET_LEN(txmbs, len + 3)
		;

		len += 9;
		break;
	case FC_DIAG:
		if (usAddress == 0x0000)
		{
			memcpy((char*) txmbs, (char*) rxmbs, 12);
			eMBDiagCB();
			len += 12;
		}
		break;

	default:

		break;
	}
	if (len > 0 && len < 1000)
	{
		if (mbconn->state == NETCONN_NONE)
			netconn_write(mbconn, (void * ) txbuf, len, NETCONN_COPY);
	}
}

err_t recv_callback_ex(struct netconn *mbconn, void *data, u16_t len)
{
	unsigned short usLength = 0;

	/* Check if status is ok and data is arrived. */
	if (len != 0)
	{
		if ((usTCPBufPos + len) <= MB_TCP_BUF_SIZE)
		{
			memcpy(&aucTCPBuf[usTCPBufPos], data, len);
			usTCPBufPos += len;
		}
		else
			usTCPBufPos = 0;

		if (usTCPBufPos >= MB_TCP_FUNC)
		{
			/* Length is a byte count of Modbus PDU (function code + data) and the
			 * unit identifier. */
			usLength = aucTCPBuf[MB_TCP_LEN] << 8U;
			usLength |= aucTCPBuf[MB_TCP_LEN + 1];

			/* Is the frame already complete. */
			if (usTCPBufPos < ( MB_TCP_UID + usLength))
			{
			}
			else if (usTCPBufPos == ( MB_TCP_UID + usLength))
			{
				EventPost_ex(mbconn);
				usTCPBufPos = 0;
			}
			else
				usTCPBufPos = 0;
		}
	}
	else
	{
		/* No data arrived */
		/* That means the client closes the connection and sent us a packet with FIN flag set to 1. */
		/* We have to cleanup and destroy out TCP connection. */

	}
	mem_free(mbconn);
	return ERR_OK;
}

void modbus_thread(void *arg)
{
	struct netconn *mbconn, *newmbconn;
	err_t err, accept_err;

	/* Create a new mbconnection identifier. */
	mbconn = netconn_new(NETCONN_TCP);

	if (mbconn != NULL)
	{
		/* Bind mbconnection to well known port number 7. */
		err = netconn_bind(mbconn, NULL, EthSettings.IPport);

		if (err == ERR_OK)
		{
			/* Tell mbconnection to go into listening mode. */
			netconn_listen(mbconn);

			while (1)
			{
				/* Grab new mbconnection. */
				accept_err = netconn_accept(mbconn, &newmbconn);
				if (accept_err == ERR_OK)
				{
					newmbconn->send_timeout = 0;
					newmbconn->recv_timeout = 3600000;

					if (mbconnNum < MB_MAX_TCP_CLIENTS)
						xTaskCreate(modbusClientThread, "xThreadMBTCP", 4000, (void*) newmbconn, MODBUS_THREAD_PRIO, NULL);
					else
					{
						netconn_close(newmbconn);
						netconn_delete(newmbconn);
						newmbconn = NULL;
					}
				}
			}
		}
		else
		{
			netconn_delete(newmbconn);
		}
	}
}

void modbusClientThread(void *mbconn)
{
	mbconnNum++;
	struct netbuf *inbuf = NULL;
	uint8_t *data = NULL;
	u16_t len = 0;

	while (netconn_recv(mbconn, &inbuf) == ERR_OK)
	{
		do
		{
			netbuf_data(inbuf, (void *) &data, &len);
			recv_callback_ex(mbconn, data, len);
		} while (netbuf_next(inbuf) >= 0);
		netbuf_delete(inbuf);
		inbuf = NULL;
		vTaskDelay(10);
	}
	netconn_delete(mbconn);
	mbconn = NULL;
	mbconnNum--;
	vTaskDelete(NULL);
}

void modbus_init(void)
{
	sys_thread_new("ModbusTCP", modbus_thread, NULL, DEFAULT_THREAD_STACKSIZE, MODBUS_THREAD_PRIO-1);
}

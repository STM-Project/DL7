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
 * File: $Id: port.h,v 1.1 2006/08/22 21:35:13 wolti Exp $
 *
 * modified by Yuji Hosogaya 3-16-2012
 *
 * Originally this file was porttcp.c, then combined with other files
 * in order to make a light version of Modbus TCP.
 */

#ifndef __MB_TCP__
#define __MB_TCP__

#include "lwip/opt.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "lwip/netif.h"

typedef unsigned char BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef unsigned short USHORT;
typedef short SHORT;

typedef unsigned long ULONG;
typedef long LONG;


/*! \ingroup modbus
 * \brief Errorcodes used by all function in the protocol stack.
 */

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#define MODBUS_DEBUG    0

typedef struct _MB_STRUCT{
    UCHAR TID_H;
    UCHAR TID_L;
    UCHAR PID_H;
    UCHAR PID_L;
    UCHAR LEN_H;
    UCHAR LEN_L;
    UCHAR UID;
    UCHAR FUNC;
    UCHAR DATA;
}MB_STRUCT;

typedef struct _MB_RIR_REQ{
    UCHAR ADDR_H;
    UCHAR ADDR_L;
    UCHAR LEN_H;
    UCHAR LEN_L;
    UCHAR DATA[1000];
}MB_WORD_REQ;

typedef struct _MB_RIR_RES{
    UCHAR BYTES;
    UCHAR DATA;
}MB_WORD_RES;

#define TO_USHORT(H,L)  ((USHORT)(H<<8)|(USHORT)L)
#define GET_TID(p)   (TO_USHORT(p->TID_H,p->TID_L))
#define GET_PID(p)   (TO_USHORT(p->PID_H,p->PID_L))
#define GET_LEN(p)   (TO_USHORT(p->LEN_H,p->LEN_L))
#define GET_UID(p)   (p->UID)
#define GET_FUNC(p)   (p->FUNC)
#define GET_DATA_PTR(p)   (&p->DATA)


#define SET_TID(p,W)    p->TID_H=(W)>>8;p->TID_L=(W)&0xFF;
#define SET_PID(p,W)    p->PID_H=(W)>>8;p->PID_L=(W)&0xFF;
#define SET_LEN(p,W)    p->LEN_H=(W)>>8;p->LEN_L=(W)&0xFF;
#define SET_UID(p,B)    p->UID_H=B;
#define SET_FUNC(p,B)   p->FUNC=B;
#define SET_DATA(p,B)   p->DATA=B;

#define FC_RD_COILS 0x01
#define FC_RD_DISC_INPUTS 0x02
#define FC_RD_HOLDING_REGS 0x03
#define FC_RD_INPUT_REGS 0x04
#define FC_WR_SINGLE_COIL 0x05
#define FC_WR_SINGLE_REG 0x06
#define FC_DIAG 0x08
#define FC_WR_MULTI_COILS 0x0F
#define FC_WR_MULTI_REGS 0x10
#define FC_RD_WR_MULTI_REGS 0x17

void modbus_init(void);

#endif

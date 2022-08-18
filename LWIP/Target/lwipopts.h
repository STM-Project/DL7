/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : Target/lwipopts.h
  * Description        : This file overrides LwIP stack default configuration
  *                      done in opt.h file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LWIPOPTS__H__
#define __LWIPOPTS__H__

#include "main.h"

/*-----------------------------------------------------------------------------*/
/* Current version of LwIP supported by CubeMx: 2.1.2 -*/
/*-----------------------------------------------------------------------------*/

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

#ifdef __cplusplus
 extern "C" {
#endif

/* STM32CubeMX Specific Parameters (not defined in opt.h) ---------------------*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- WITH_RTOS enabled (Since FREERTOS is set) -----*/
#define WITH_RTOS 1
/*----- CHECKSUM_BY_HARDWARE enabled -----*/
#define CHECKSUM_BY_HARDWARE 1
/*-----------------------------------------------------------------------------*/

/* LwIP Stack Parameters (modified compared to initialization value in opt.h) -*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- Default value in ETH configuration GUI in CubeMx: 1524 -----*/
#define ETH_RX_BUFFER_SIZE 1536
/*----- Default Value for LWIP_DNS: 0 ---*/
#define LWIP_DNS 1
/*----- Default Value for MEMP_NUM_UDP_PCB: 4 ---*/
#define MEMP_NUM_UDP_PCB 32
/*----- Default Value for MEMP_NUM_TCP_PCB: 5 ---*/
#define MEMP_NUM_TCP_PCB 32
/*----- Default Value for LWIP_TCPIP_CORE_LOCKING: 0 ---*/
#define LWIP_TCPIP_CORE_LOCKING 1
/*----- Value in opt.h for MEM_ALIGNMENT: 1 -----*/
#define MEM_ALIGNMENT 4
/*----- Default Value for MEM_SIZE: 1600 ---*/
#define MEM_SIZE 16360
/*----- Default Value for MEMP_OVERFLOW_CHECK: 0 ---*/
#define MEMP_OVERFLOW_CHECK 1
/*----- Default Value for MEMP_SANITY_CHECK: 0 ---*/
#define MEMP_SANITY_CHECK 1
/*----- Default Value for MEM_OVERFLOW_CHECK: 0 ---*/
#define MEM_OVERFLOW_CHECK 1
/*----- Default Value for H7 devices: 0x30044000 -----*/
#define LWIP_RAM_HEAP_POINTER 0x30044000
/*----- Default Value for MEMP_NUM_PBUF: 16 ---*/
#define MEMP_NUM_PBUF 32
/*----- Default Value for MEMP_NUM_TCP_SEG: 16 ---*/
#define MEMP_NUM_TCP_SEG 32
/*----- Default Value for MEMP_NUM_SYS_TIMEOUT: 3 ---*/
#define MEMP_NUM_SYS_TIMEOUT 32
/*----- Default Value for MEMP_NUM_NETBUF: 2 ---*/
#define MEMP_NUM_NETBUF 32
/*----- Default Value for MEMP_NUM_NETCONN: 4 ---*/
#define MEMP_NUM_NETCONN 32
/*----- Value supported for H7 devices: 1 -----*/
#define LWIP_SUPPORT_CUSTOM_PBUF 1
/*----- Default Value for PBUF_POOL_SIZE: 16 ---*/
#define PBUF_POOL_SIZE 24
/*----- Default Value for PBUF_POOL_BUFSIZE: 592 ---*/
#define PBUF_POOL_BUFSIZE 1526
/*----- Value in opt.h for LWIP_ETHERNET: LWIP_ARP || PPPOE_SUPPORT -*/
#define LWIP_ETHERNET 1
/*----- Default Value for ETHARP_TABLE_MATCH_NETIF: 1 ---*/
#define ETHARP_TABLE_MATCH_NETIF 0
/*----- Default Value for IP_REASSEMBLY: 1 ---*/
#define IP_REASSEMBLY 0
/*----- Default Value for IP_FRAG: 1 ---*/
#define IP_FRAG 0
/*----- Default Value for IP_REASS_MAXAGE: 15 ---*/
#define IP_REASS_MAXAGE 3
/*----- Value in opt.h for LWIP_DNS_SECURE: (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT) -*/
#define LWIP_DNS_SECURE 7
/*----- Default Value for TCP_MSS: 536 ---*/
#define TCP_MSS 1460
/*----- Default Value for TCP_SND_QUEUELEN: 9 ---*/
#define TCP_SND_QUEUELEN 16
/*----- Value in opt.h for LWIP_NETIF_LINK_CALLBACK: 0 -----*/
#define LWIP_NETIF_LINK_CALLBACK 1
/*----- Value in opt.h for TCPIP_THREAD_STACKSIZE: 0 -----*/
#define TCPIP_THREAD_STACKSIZE 16000
/*----- Value in opt.h for TCPIP_THREAD_PRIO: 1 -----*/
#define TCPIP_THREAD_PRIO osPriorityRealtime
/*----- Value in opt.h for TCPIP_MBOX_SIZE: 0 -----*/
#define TCPIP_MBOX_SIZE 128
/*----- Value in opt.h for SLIPIF_THREAD_STACKSIZE: 0 -----*/
#define SLIPIF_THREAD_STACKSIZE 1024
/*----- Value in opt.h for DEFAULT_THREAD_STACKSIZE: 0 -----*/
#define DEFAULT_THREAD_STACKSIZE 8000
/*----- Value in opt.h for DEFAULT_UDP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_UDP_RECVMBOX_SIZE 128
/*----- Value in opt.h for DEFAULT_TCP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_TCP_RECVMBOX_SIZE 128
/*----- Value in opt.h for DEFAULT_ACCEPTMBOX_SIZE: 0 -----*/
#define DEFAULT_ACCEPTMBOX_SIZE 128
/*----- Default Value for LWIP_TCPIP_TIMEOUT: 0 ---*/
#define LWIP_TCPIP_TIMEOUT 1
/*----- Value in opt.h for LWIP_SOCKET: 1 -----*/
#define LWIP_SOCKET 0
/*----- Default Value for LWIP_SO_SNDTIMEO: 0 ---*/
#define LWIP_SO_SNDTIMEO 1
/*----- Default Value for LWIP_SO_RCVTIMEO: 0 ---*/
#define LWIP_SO_RCVTIMEO 1
/*----- Value in opt.h for RECV_BUFSIZE_DEFAULT: INT_MAX -----*/
#define RECV_BUFSIZE_DEFAULT 2000000000
/*----- Default Value for LWIP_SOCKET_SELECT: 1 ---*/
#define LWIP_SOCKET_SELECT 0
/*----- Default Value for LWIP_SOCKET_POLL: 1 ---*/
#define LWIP_SOCKET_POLL 0
/*----- Value in opt.h for LWIP_STATS: 1 -----*/
#define LWIP_STATS 0
/*----- Value in opt.h for CHECKSUM_GEN_IP: 1 -----*/
#define CHECKSUM_GEN_IP 0
/*----- Value in opt.h for CHECKSUM_GEN_UDP: 1 -----*/
#define CHECKSUM_GEN_UDP 0
/*----- Value in opt.h for CHECKSUM_GEN_TCP: 1 -----*/
#define CHECKSUM_GEN_TCP 0
/*----- Value in opt.h for CHECKSUM_GEN_ICMP6: 1 -----*/
#define CHECKSUM_GEN_ICMP6 0
/*----- Value in opt.h for CHECKSUM_CHECK_IP: 1 -----*/
#define CHECKSUM_CHECK_IP 0
/*----- Value in opt.h for CHECKSUM_CHECK_UDP: 1 -----*/
#define CHECKSUM_CHECK_UDP 0
/*----- Value in opt.h for CHECKSUM_CHECK_TCP: 1 -----*/
#define CHECKSUM_CHECK_TCP 0
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP6: 1 -----*/
#define CHECKSUM_CHECK_ICMP6 0
/*-----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /*__LWIPOPTS__H__ */

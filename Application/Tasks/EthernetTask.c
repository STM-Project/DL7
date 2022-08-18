/*
 * EthernetTask.c
 *
 *  Created on: Nov 16, 2020
 *      Author: TomaszSok
 */

#include "EthernetTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lwip/tcpip.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "mini-printf.h"
#include "version.h"
#include "lwip.h"
#include "httpserver-netconn.h"
#include "parameters.h"
#include "http_requests.h"
#include "lwip/dns.h"
#include "StartUp.h"
#include "smtp_netconn.h"
#include "tcpipclient_netconn.h"
#include "mbtcp.h"

ETH_SETTINGS EthSettings;

extern osSemaphoreId Netif_LinkSemaphore;
struct link_str link_arg;
TaskHandle_t vtaskEthHandle;

static void DNS_Init(void)
{
	ip_addr_t primary;
	ip_addr_t secondary;

	IP4_ADDR(&primary, EthSettings.primaryDNS[0], EthSettings.primaryDNS[1], EthSettings.primaryDNS[2], EthSettings.primaryDNS[3]);
	IP4_ADDR(&secondary,EthSettings.secondaryDNS[0], EthSettings.secondaryDNS[1], EthSettings.secondaryDNS[2], EthSettings.secondaryDNS[3]);

	dns_setserver(0,&primary);
	dns_setserver(1,&secondary);
}

void vtaskEth(void *pvParameters)
{
			while(1)
			{
				if (STARTUP_WaitForBits(0x0040))
				{
					STARTUP_ClaerBits(0x0040);

					vTaskDelay(1000);
					MX_LWIP_Init();
					http_server_netconn_init();
					modbus_init();

					MBTCP_Initialize();

					DNS_Init();
					EMAIL_Init();

					STARTUP_SetBits(0x0080);

					vTaskDelete(NULL);
				}
				else
					continue;
			}
}

void CreateEthTask(void)
{
	xTaskCreate(vtaskEth, "vtaskEth", 1600, NULL, ( unsigned portBASE_TYPE )4, &vtaskEthHandle);
}

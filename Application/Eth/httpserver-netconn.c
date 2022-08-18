/**
 ******************************************************************************
 * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/httpser-netconn.c
 * @author  MCD Application Team
 * @version V1.2.2
 * @date    25-May-2015
 * @brief   Basic http server implementation using LwIP netconn API
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <ctype.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "httpserver-netconn.h"
#include "cmsis_os.h"
#include "EthernetTask.h"
#include "http_requests.h"
#include "string.h"
#include "passwords.h"
#include "mini-printf.h"
#include "tcp.h"
#include "fatfs.h"

#define WEBSERVER_THREAD_PRIO		4
#define LOG_TIME_MS	120000

TickType_t httpLoggingServiceTimer;
TickType_t currentTick;
u32_t req_ip = 0;

uint8_t bufferForIndex[25600] __attribute__ ((section(".sdram")));
uint32_t bytesreadFromIndex;

uint8_t bufferForLog[4096] __attribute__ ((section(".sdram")));
uint32_t bytesreadFromLog;

uint8_t bufferForLogo[16000] __attribute__ ((section(".sdram")));
uint32_t bytesreadFromLogo;


unsigned int LoadFileToRAM(const char *pathToFile, unsigned char *buff)
{
	FIL graphicFile;
	FILINFO fInfo;
	UINT bytesReadFromFile;
	f_stat (pathToFile, &fInfo);
	f_open(&graphicFile, pathToFile, FA_READ);
	f_read(&graphicFile, buff, fInfo.fsize, &bytesReadFromFile);
	f_close(&graphicFile);
	return bytesReadFromFile;
}

void InitWebsiteFiles(void)
{
	bytesreadFromIndex = LoadFileToRAM("0:website/index.htm", bufferForIndex);
	bytesreadFromLogo = LoadFileToRAM("0:website/logo.png", bufferForLogo);
	bytesreadFromLog = LoadFileToRAM("0:website/log.htm", bufferForLog);
}

void pushWebsiteFilesToMemory(void);

/**
 * @brief  http server thread
 * @param arg: pointer on argument(not used here)
 * @retval None
 */
static void http_server_netconn_thread(void *arg)
{
	InitWebsiteFiles();
	httpLoggingServiceTimer = xTaskGetTickCount();
	struct netconn *conn, *newconn;
	err_t accept_err;
	while(1)
	{
		conn = netconn_new(NETCONN_TCP);

		if (conn != NULL)
		{
			netconn_bind(conn, NULL, 80);

			netconn_listen(conn);

			do{
				accept_err = netconn_accept(conn, &newconn);

				if(accept_err == ERR_OK)
				{
					currentTick = xTaskGetTickCount();

					if(currentTick-httpLoggingServiceTimer > LOG_TIME_MS && req_ip)
						req_ip = 0;

					if(newconn->pcb.tcp->remote_ip.addr == req_ip || PASSWORDS_IsUserPasswordDisable())
					{
						if(NULL==sys_thread_new("HTTP_req", http_server_serve_logged, newconn, 1600, 2))
						{
							netconn_close(newconn);
							netconn_delete(newconn);
						}
						httpLoggingServiceTimer = currentTick;
						if(PASSWORDS_IsUserPasswordDisable())
							req_ip = newconn->pcb.tcp->remote_ip.addr;
					}
					else
					{
						if(req_ip == 0)
						{
							if( http_server_serve_not_logged(newconn))
							{
								req_ip = newconn->pcb.tcp->remote_ip.addr;
								httpLoggingServiceTimer = currentTick;
							}
						}
						else
						{
							char respond[350];
							int cx = mini_snprintf(respond, 350, "HTTP/1.1 200 OK\r\n Connection: close\r\n\r\nAccess denied, someone else is logged.\nZugriff verweigert, jemand anderes wird protokolliert.\nAcceso denegado, otra persona se registra.\nAcces refuse, quelqu'un d'autre est connecte.\nPolaczenie odrzucone, ktos inny jest zalogowany.\nAcesso negado, alguem e registrado.");
							netconn_write(newconn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
						}
						netconn_close(newconn);
						netconn_delete(newconn);
						vTaskDelay(150);
					}
				}
			}while(accept_err == ERR_OK);
			netconn_close(conn);
		  netconn_delete(conn);
		}
	}
}
/**
 * @brief  Initialize the HTTP server (start its thread)
 * @param  none
 * @retval None
 */
void http_server_netconn_init()
{
	sys_thread_new("HTTP", http_server_netconn_thread, NULL, 16000, WEBSERVER_THREAD_PRIO);
}

/*
 * Timer reset
 * Use it if process will hang http_server_netconn_thread for more than 2 minutes
 *
 */
void setWebLoginTimer()
{
	currentTick = xTaskGetTickCount();
	httpLoggingServiceTimer = currentTick;
}
/*
 *
 * Use it for reset timer and logoff peer
 */
void logOffPeer(void)
{
	req_ip = 0;
}

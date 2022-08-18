#include <inttypes.h>
#include "lwip/api.h"
#include "lwip/priv/api_msg.h"
#include "lwip/tcp.h"
#include "ff.h"
#include "EthernetTask.h"
#include "httpserver-netconn.h"
#include "archive.h"
#include "GUI.h"
#include "parameters.h"
#include <string.h>
#include "mini-printf.h"
#include "dtos.h"

#define DOWNLOAD_BUFFER_SIZE	65536
#define WEBSITE_BUFFER_SIZE		60000
#define RESPOND_BUFFER_SIZE		330

static char archDownBuffer[DOWNLOAD_BUFFER_SIZE] __attribute__ ((section(".sdram")));
static char archive_page[WEBSITE_BUFFER_SIZE] __attribute__ ((section(".sdram")));
static char respond[RESPOND_BUFFER_SIZE] __attribute__ ((section(".sdram")));
static UINT archDownCounter;
static FIL arch_file;
static u32_t downloadFlag = 0;


void archive_download(struct netconn *conn, const char* filepath)
{
	int cx;
	if (!downloadFlag)
	{
		downloadFlag = 1;
		FRESULT res;
		archDownCounter = 0;
		DWORD filePointer=0;
		ARCHIVE_SaveBuffersToSDCard();
		FILINFO info;
		res = f_stat(filepath, &info);
		if (res == FR_OK)
		{
			cx = mini_snprintf(respond, RESPOND_BUFFER_SIZE,
					"HTTP/1.1 200 OK \r\nDate: Fri, 31 Dec 2154 23:59:59 GMT\r\nContent-Type: file/csv\r\nContent-Length: %u\r\n\r\n",
					info.fsize);
			netconn_write(conn, respond, (size_t )cx, NETCONN_NOCOPY);

			while (1)
			{
				if (conn->state == NETCONN_NONE)
				{
					if (ARCHIVE_TakeSemaphore(50))
					{
						f_open(&arch_file, filepath, FA_READ);
						f_lseek(&arch_file, filePointer);
						res = f_read(&arch_file, archDownBuffer, DOWNLOAD_BUFFER_SIZE, &archDownCounter);
						filePointer = arch_file.fptr;
						f_close(&arch_file);
						ARCHIVE_GiveSemaphore();
						if (res || archDownCounter == 0)
							break;
						if (ERR_OK != netconn_write(conn, archDownBuffer, (size_t ) archDownCounter, NETCONN_MORE|NETCONN_COPY))
							break;
						vTaskDelay(75);
					}
				}
				else
					vTaskDelay(5);
			}
			downloadFlag = 0;
		}
	}
	else
	{
		cx =	mini_snprintf(respond, RESPOND_BUFFER_SIZE,
						"HTTP/1.1 503 Service Unavailable\r\n Connection: close\r\n\r\nSomeone is downloading archive right now.\nJemand ladt jetzt das Archiv herunter\nAlguien esta descargando un archivo ahora\nQuelqu'un est en cours de telechargement des archives en ce moment\nKtos pobiera w tym momencie archiwum\nAlguem esta baixando o arquivo agora");
		netconn_write(conn, respond, (size_t )cx, NETCONN_NOCOPY);
	}
}

void archive_website(struct netconn *conn, const char* filepath, const char* host)
{
	FRESULT res;
	static FILINFO fno;
	DIR dir;
	int n = 0;
	char file_path_buffer[40]={0};
	char file_size[13]={0};

	memset(archive_page, 0, WEBSITE_BUFFER_SIZE);
	res = f_opendir(&dir, filepath); /* Open the directory */
	mini_snprintf(file_path_buffer, 40, "%s/%s/",host, filepath);
	if (res == FR_OK)
	{
		n = mini_snprintf(archive_page, WEBSITE_BUFFER_SIZE, "HTTP/1.1 200 OK \r\n Connection: close\r\n"
				"Content-Type: text/html\r\nCache-Control: no-store\r\n\r\n"
				"<table class=\"table table-hover sortable\">\n");
		for (;;)
		{
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (fno.fname[0] == '.')
				continue; /* Ignore dot entry */
			if (AM_DIR != (fno.fattrib & AM_DIR))
			{
				float2stri(file_size, ((float) fno.fsize) / 1024, 2);
				n += mini_snprintf(archive_page + n, WEBSITE_BUFFER_SIZE,
						"<tr onclick=\"location.href='http://%s%s'\"><td>%s</td><td>%s kB</td><td>%u-%02u-%02u %02u:%02u</td></tr>\n", file_path_buffer,
						fno.fname, fno.fname, file_size, (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31, (fno.ftime >> 11),
						((fno.ftime >> 5) & 63));
			}
		}
		n += mini_snprintf(archive_page + n, WEBSITE_BUFFER_SIZE, "\n</table>");
		netconn_write(conn, archive_page, n, NETCONN_NOCOPY);
		f_closedir(&dir);
	}
}

void archive_GetHost(char *request,char *hostIP)
{
	char *hostptr=NULL;
	int IPlength=0;
	hostptr = strstr(request,"Host: ");
	if(hostptr!=NULL)
	{
		hostptr=hostptr+6;
		IPlength = strcspn(hostptr,"\r");
		strncpy(hostIP,hostptr,IPlength);
	}
	else
		mini_snprintf(hostIP, 16, "%u.%u.%u.%u", EthSettings.IPAddress[0], EthSettings.IPAddress[1],
				EthSettings.IPAddress[2],EthSettings.IPAddress[3]);
}

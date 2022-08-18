#include <ctype.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/mem.h"
#include "string.h"
#include "httpserver-netconn.h"
#include "website_process.h"
#include "cmsis_os.h"
#include "base64.h"
#include "passwords.h"
#include "http_requests.h"
#include "mini-printf.h"
#include "version.h"

#define GET_METHOD_STRING 4
#define SMALL_PART	50
static char respond[110];

void sendSimpleHTTPResponse(int type, struct netconn *conn)
{
	bzero(respond,110);
	int cx;
	if ( type == 200 )
		cx = mini_snprintf((char *)respond, 110, "HTTP/1.0 200 OK\r\n\r\n");
	else if ( type == 401 )
		cx = mini_snprintf((char *)respond, 110, "HTTP/1.0 401 Unauthorized\r\n Connection: Keep-Alive\r\n\r\nUnauthorized access, please refresh website.");
	else if (type == 404 )
		cx = mini_snprintf((char *)respond, 110, "HTTP/1.0 404 Not Found\r\n\r\n");

	netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
}

/**
 * @brief serve tcp connection when logged
 * @param conn: pointer on connection structure
 * @retval None
 */

void http_server_serve_logged(void *pvParameters)
{
	struct netconn *conn;
	conn = pvParameters;

	bzero(respond,110);
	struct netbuf *inbuf = NULL;
	int len;
	char str[2000];

	if(ERR_OK == netconn_recv(conn, &inbuf))
	{
		if(netconn_err(conn) == ERR_OK){
			int cx = 0;
			char* buf;

			u16_t buflen = 0;
			buflen = netbuf_len(inbuf);
			buf = pvPortMalloc(buflen*sizeof(char));
			netbuf_copy(inbuf, buf, buflen);

			if ((strncmp(buf, "GET /INDEX.htm", 14) == 0) || (strncmp(buf, "GET / ", 6) == 0))
			{
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nCache-Control: no-cache\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				uint8_t *data = bufferForIndex;
				len = bytesreadFromIndex;
				netconn_write(conn, data, (size_t )len, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /vab", 8) == 0))
			{
				char* charTabNum = buf + 8;
				short tabNum = *(charTabNum) - '0';
				short cellNum = 0;
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nCache-Control: no-store\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				cx = measurement_fulfill_value(tabNum, cellNum, str);
				if(conn->state == NETCONN_NONE)
					netconn_write(conn, str, (size_t )cx, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /tab", 8) == 0))
			{
				char* charTabNum = buf + 8;
				short tabNum = *(charTabNum) - '0';
				short cellNum = 0;
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nCache-Control: no-store\r\n\r\n");
				netconn_write(conn, (const char* )respond, (size_t )cx, NETCONN_NOFLAG);
				cx = measurement_fulfill(tabNum, cellNum, str);
				if(conn->state == NETCONN_NONE)
					netconn_write(conn, str, (size_t )cx, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /logo.png", 13) == 0) || (strncmp(buf, "GET /LOGOWWW.png", 16) == 0))
			{
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: image/png\r\nCache-Control: public, max-age=86400\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				uint8_t *data = bufferForLogo;
				len = bytesreadFromLogo;
				netconn_write(conn, data, (size_t )len, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /MAIN", 9) == 0))
			{
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nCache-Control: no-store\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				cx = main_fulfill(str);
				if(conn->state == NETCONN_NONE)
					netconn_write(conn, (const char* )(str), (size_t )cx, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /BAR", 8) == 0))
			{
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nCache-Control: no-store\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				cx =bar_fulfill(str);
				if(conn->state == NETCONN_NONE)
					netconn_write(conn, (const char* )(str), (size_t )cx, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /logoff", 11) == 0))
			{
				logOffPeer();
				sendSimpleHTTPResponse(200, conn);
			}
			else if ((strncmp(buf, "GET /archive/", 13) == 0))
			{

				char* pathsize;
				char filepath[20];
				pathsize = strchr(buf + GET_METHOD_STRING, ' ');
				int n = pathsize - buf;
				strncpy(filepath, buf + GET_METHOD_STRING, n - GET_METHOD_STRING);
				filepath[n - GET_METHOD_STRING] = '\0';
				if (strcmp(filepath, "/archive/") != 0)
					archive_download(conn, filepath);
				else
				{
					char hostIP[24]={0};
					archive_GetHost(buf,hostIP);
					archive_website(conn, "archive",hostIP);
				}
			}
			else
			{
				cx = mini_snprintf((char *)respond, 110, "HTTP/1.0 404 Not Found\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
			}
			vPortFree(buf);
		}
		mem_free(conn);
		netbuf_delete(inbuf);
	}
	netconn_close(conn);
	netconn_delete(conn);
	vTaskDelete(NULL);
}
/**
 * @brief serve site when not logged
 * @param conn: pointer on connection structure
 * @retval logged/not logged
 */
u32_t http_server_serve_not_logged(struct netconn *conn)
{
	u32_t returnStatus = 0;
	bzero(respond,110);
	struct netbuf *inbuf = NULL;;
	int len;
	char* buf;
	u16_t buflen;
	int cx = 0;

	if (ERR_OK == netconn_recv(conn, &inbuf))
	{
		if (netconn_err(conn) == ERR_OK)
		{
			buflen = netbuf_len(inbuf);
			buf = pvPortMalloc(buflen*sizeof(char));
			netbuf_copy(inbuf, buf, buflen);
			if ((strncmp(buf, "GET /LOGIN", 10) == 0) )
			{
				char* ret;
				char* givenPassword;
				char* realPassword;
				ret = strstr(buf,"x-usr-psw");
				if(ret == 0)
				{
					realPassword = getUserPassword();

					if( isspace(*realPassword) )
					{
						sendSimpleHTTPResponse(200, conn);
						returnStatus = 1;
					}
					else
						sendSimpleHTTPResponse(401, conn);
				}
				else
				{
					ret+=11;
					char password[30] = {0};
					if(*ret != '\n')
					{
						u32_t passLength=0;
						do
						{
							password[passLength]=*ret;
							ret++;
							passLength++;
						}
						while(password[passLength-1] != '\r');
						password[passLength-1]='\0';

						givenPassword = BASE64Decode(password);
					}
					else
					{
						char nullChar = '\0';
						givenPassword=&nullChar;
					}
					realPassword = getUserPassword();

					if( !strcmp(realPassword, givenPassword) )
					{
						sendSimpleHTTPResponse(200, conn);
						returnStatus = 1;
					}
					else
						sendSimpleHTTPResponse(401, conn);

					givenPassword = NULL;
				}

			}
			else if ((strncmp(buf, "GET /archive/", 13) == 0))
			{
				char* ret = strstr(buf,"User-Agent");
				ret+=12;
				char userAgent[10];
				char programRaportUserAgent[10];
				strncpy(userAgent, ret, 6);
				userAgent[6] = '\0';
				mini_snprintf(programRaportUserAgent,10,"%s-RP",NAME);
				if( !strcmp(userAgent, programRaportUserAgent) )
				{
					char* pathsize;
					char filepath[20];
					pathsize = strchr(buf + GET_METHOD_STRING, ' ');
					int n = pathsize - buf;
					strncpy(filepath, buf + GET_METHOD_STRING, n - GET_METHOD_STRING);
					filepath[n - GET_METHOD_STRING] = '\0';
					if (strcmp(filepath, "/archive/") != 0)
						archive_download(conn, filepath);
					else
					{
						char hostIP[24]={0};
						archive_GetHost(buf,hostIP);
						archive_website(conn, "archive",hostIP);
					}
				}
				else
					sendSimpleHTTPResponse(401, conn);
			}
			else if ((strncmp(buf, "GET /logo.png", 13) == 0) || (strncmp(buf, "GET /LOGOWWW.png", 16) == 0) )
			{
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: image/png\r\nCache-Control: public, max-age=86400\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				uint8_t *data = bufferForLogo;
				len = bytesreadFromLogo;
				if(data != 0)
					netconn_write(conn, (const char* )data, sizeof(char)*len, NETCONN_NOFLAG);
			}
			else if ((strncmp(buf, "GET /tab", 8) == 0) || (strncmp(buf, "GET /vab", 8) == 0)
					 || (strncmp(buf, "GET /MAIN", 9) == 0) || (strncmp(buf, "GET /BAR", 8) == 0))
			{
				sendSimpleHTTPResponse(401, conn);
			}
			else
			{
				cx = mini_snprintf(respond, 110, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nCache-Control: max-age=600\r\n\r\n");
				netconn_write(conn, (const char* )(respond), (size_t )cx, NETCONN_NOFLAG);
				uint8_t *data = bufferForLog;
				len = bytesreadFromLog;
				if(data != 0)
					netconn_write(conn, (const char* )data, sizeof(char)*len, NETCONN_NOFLAG);
			}
			vPortFree(buf);
		}
	}
	netbuf_delete(inbuf);
	inbuf = NULL;
	return returnStatus;
}

/*
 * smtp_netconn.c
 *
 *  Created on: 14.02.2019
 *      Author: TomaszSok
 */

#include "smtp_netconn.h"
#include <ctype.h>
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#include "smtp_base64.h"
#include "crc.h"

#include "parameters.h"
#include "version.h"
#include "NSMAC.h"
#include "mini-printf.h"
#include "dtos.h"
#include "channels.h"

#include "PopUpMessageDLG.h"

#include "archive.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

SDRAM EMAIL_SETTINGS emailSettings;
SDRAM EMAIL_SETTINGS bkEmailSettings;

static const char passwordHashkey[4]={1,4,2,3};

static uint8_t testEmailFlag = 0;

void CopyEmailClientSettings(EMAIL_CLIENT *Dst, EMAIL_CLIENT *Src)
{
	strncpy(Dst->email, Src->email, 64);
	strncpy(Dst->login, Src->login, 64);
	strncpy((char *)Dst->loginBase64, (char *)Src->loginBase64, 96);
	strncpy(Dst->password, Src->password, 32);
	strncpy((char *)Dst->passwordBase64, (char *)Src->passwordBase64, 48);
	Dst->useSSL = Src->useSSL;
	for (int i=0; i<5; ++i)
		strncpy(&Dst->mailList[i][0], &Src->mailList[i][0], 64);

}

void CopyEmailServerSettings(SMTP_SERVER *Dst, SMTP_SERVER *Src)
{
	strncpy(Dst->name, Src->name, 64);
	Dst->port = Src->port;
}

void CopyEmailEventSettings(EMAIL_EVENT *Dst, EMAIL_EVENT *Src)
{
	Dst->mode = Src->mode;
	Dst->hour = Src->hour;
	Dst->day = Src->day;
}

static int SMTP_ChcekResponseCode(const char* response,const char* code)
{
	if(strncmp(response,code,3))
		return 1;
	else
		return 0;
}

void SMTP_SendCyceledEmail(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
	static uint8_t sendMailFlag = 0;

	switch (emailSettings.event.mode)
	{
	case 1:
		if (sTime->Hours == emailSettings.event.hour && sendMailFlag == 0)
		{
			sendMailFlag = 1;
			CreateTotalizerEmailReportTask(&emailSettings);
		}
		else if (sTime->Hours != emailSettings.event.hour && sendMailFlag == 1)
			sendMailFlag = 0;
		break;
	case 2:
		if (sendMailFlag == 0 && sTime->Hours == emailSettings.event.hour && sDate->WeekDay == (emailSettings.event.day+1))
		{
			sendMailFlag = 1;
			CreateTotalizerEmailReportTask(&emailSettings);
		}
		else if (sendMailFlag == 1 && (sTime->Hours != emailSettings.event.hour || sDate->WeekDay != (emailSettings.event.day+1)))
			sendMailFlag = 0;
		break;
	case 3:
		if (sendMailFlag == 0 && sTime->Hours == emailSettings.event.hour && sDate->Date == (emailSettings.event.day+1))
		{
			sendMailFlag = 1;
			CreateTotalizerEmailReportTask(&emailSettings);
		}
		else if (sendMailFlag == 1 &&(sTime->Hours != emailSettings.event.hour || sDate->Date != (emailSettings.event.day+1)))
			sendMailFlag = 0;
		break;
	}
}

void EMAIL_EncodePasswordForSettingsFile(unsigned char *codedPassword, char *password)
{
	size_t base64len = 0;
	if(0 == strlen(password))
		strcpy(password," \0");
	mbedtls_base64_encode(codedPassword, 48, &base64len, (unsigned char *)password, strlen(password));
	int passwordLength = strlen((char*)codedPassword);
	for(int i=0;i<passwordLength;++i)
		*(codedPassword+i) += passwordHashkey[i%4];

}

void EMAIL_DecodePasswordFromSettingsFile( char *password,unsigned char *codedPassword)
{
		size_t base64len = 0;

		int passwordLength = strlen((char*)codedPassword);
		for(int i=0;i<passwordLength;++i)
			*(codedPassword+i) -= passwordHashkey[i%4];

		mbedtls_base64_decode((unsigned char *)password, 32, &base64len, codedPassword, passwordLength);
}


void SMTP_ConvertLoginDataToBase64(EMAIL_CLIENT *client)
{
	size_t base64len = 0;
	mbedtls_base64_encode(client->loginBase64, 48, &base64len, (unsigned char *)client->login, strlen((const char *)client->login));
	mbedtls_base64_encode(client->passwordBase64, 48, &base64len, (unsigned char *)client->password, strlen((const char *)client->login));
}

static struct netconn* SMTP_Connect(SMTP_SERVER *server)
{
	struct netconn *client = NULL;

	client = netconn_new(NETCONN_TCP);
	if(client == NULL)
		return NULL;
	client->recv_timeout = 2000;

	if(server->IP.addr == 0)
		return NULL;

	if(ERR_OK != netconn_connect(client, &server->IP, server->port))
	{
		netconn_delete (client);
		return NULL;
	}
	return client;
}

static err_t SMTP_Disconnect(struct netconn *client)
{
	err_t status = ERR_OK;
	status = netconn_close(client);
	if (ERR_OK == status)
		return netconn_delete(client);
	else
		return status;
}

static uint8_t SMTP_ReciveResponse(struct netconn *conn, char *res, size_t resSize)
{
	struct netbuf *inbuf = NULL;
	uint8_t *data = NULL;
	uint16_t len = 0;
	uint16_t responseLength = 0;
	uint8_t responseError = 0;
	if (ERR_OK == netconn_recv(conn, &inbuf))
	{
		do
		{
			netbuf_data(inbuf, (void *) &data, &len);
			responseLength += len;
		} while (netbuf_next(inbuf) >= 0);
		if(resSize>=responseLength)
			memcpy(res,(char*)data,responseLength);
		else
			memcpy(res,(char*)data,resSize);
		netbuf_delete(inbuf);
		inbuf = NULL;
	}
	else
		responseError = 1;

	return responseError;
}

static err_t SMTP_SendRequest(struct netconn *conn, char *buf, char *cmd, char *data)
{
	if(cmd != NULL)
			strcat(buf,cmd);
		if(data != NULL)
			strcat(buf,data);
		strcat(buf,"\r\n");

	return netconn_write(conn, (uint8_t*)buf, (uint16_t)strlen(buf), NETCONN_NOFLAG);
}

static err_t SMTP_SendMailData(struct netconn *conn, char *buf)
{
	return netconn_write(conn, (uint8_t*)buf, (uint16_t)strlen(buf), NETCONN_COPY);
}

void EMAIL_InitDefuaultParameters(void)
{
	strncpy(emailSettings.client.email," ",64);
	strncpy(emailSettings.client.login," ",64);
	strncpy(emailSettings.client.password," ",32);
	emailSettings.client.useSSL = 0;

	strncpy(emailSettings.server.name,"smtp.server.com",64);
	emailSettings.server.port = 587;

	strncpy(&emailSettings.client.mailList[0][0]," ",64);
	strncpy(&emailSettings.client.mailList[1][0]," ",64);
	strncpy(&emailSettings.client.mailList[2][0]," ",64);
	strncpy(&emailSettings.client.mailList[3][0]," ",64);
	strncpy(&emailSettings.client.mailList[4][0]," ",64);

	emailSettings.event.mode = 0;
	emailSettings.event.hour = 0;
	emailSettings.event.day = 0;

	CopyEmailClientSettings(&bkEmailSettings.client,&emailSettings.client);
	CopyEmailServerSettings(&bkEmailSettings.server,&emailSettings.server);
	CopyEmailEventSettings(&bkEmailSettings.event,&emailSettings.event);
}

void EMAIL_Init(void)
{
	netconn_gethostbyname(emailSettings.server.name, &emailSettings.server.IP);
	SMTP_ConvertLoginDataToBase64(&emailSettings.client);
}

static int IsTotalizerInEmail(TOTALIZER *tot)
{
	if(tot->type!=0 && tot->sendEmail!=0)
		return 1;
	else
		return 0;
}

static void EMAIL_SendTestReport(struct netconn *conn,EMAIL_SETTINGS *email)
{
	char sendBuffer[SMTP_MAIL_BUFFER];
	char recvBuffer[128];
	int n = mini_snprintf(sendBuffer,SMTP_MAIL_BUFFER,"From: %s\r\nSubject: %s TEST, ID: %d, %s\r\n",email->client.email,NAME,GeneralSettings.DeviceID,GeneralSettings.DeviceDescription);
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "To: ");
	for(int i=0;i<5;++i)
	{
		if(emailSettings.client.mailList[i][0]!=' ' && email->client.mailList[i][1]!='\0')
		n += mini_snprintf(sendBuffer + n,MAIL_MESSAGE_BUFFER,"%s,",&email->client.mailList[i][0]);
	}
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "\r\n");

	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "Content-Transfer-Encoding: 8bit\r\n");
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "Content-Type: text/html; charset=\"UTF-8\"");
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "\r\n\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<html><style>table{width: 20%%;} td{ border-bottom:1px solid #ddd; padding:7px; font-family:Arial; text-align:center; white-space:nowrap;}</style>");
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "<table><tr><td>MODEL</td><td>%s</td></tr>",NAME);
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "<tr><td>FIRMWARE</td><td>%s</td></tr>",VERSION);
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "<tr><td>SN</td><td>%d</td></tr>",GetSerialNumber());
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "<tr><td>ID</td><td>%d</td></tr>",GeneralSettings.DeviceID);
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "<tr><td>DESC</td><td>%s</td></tr>",GeneralSettings.DeviceDescription);
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "</table></html>\r\n");
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "\r\n.\r\n");

	SMTP_SendMailData(conn, sendBuffer);
	SMTP_ReciveResponse(conn, recvBuffer,128);

}

static void EMAIL_SendTotalizerReport(struct netconn *conn,EMAIL_SETTINGS *email)
{
	char tempText[24]={0};
	char sendBuffer[SMTP_MAIL_BUFFER];
	char recvBuffer[128];
	int n = mini_snprintf(sendBuffer,SMTP_MAIL_BUFFER,"From: %s\r\n",email->client.email);
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer,SMTP_MAIL_BUFFER,"Subject: %s RP, ID:%d, %s\r\n",NAME,GeneralSettings.DeviceID,GeneralSettings.DeviceDescription);
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "To: ");
	for(int i=0;i<5;++i)
	{
		if(emailSettings.client.mailList[i][0]!=' ' && email->client.mailList[i][1]!='\0')
		n += mini_snprintf(sendBuffer + n,SMTP_MAIL_BUFFER,"%s,",&email->client.mailList[i][0]);
	}
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "Content-Transfer-Encoding: 8bit\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "Content-Type: text/html; charset=\"UTF-8\"\r\n\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<html><style>table{width:50%%;}th,td{border-bottom:1px solid #ddd; padding:7px; font-family:Arial; text-align:center; white-space:nowrap;}</style>");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<table><tr><th>CH. #</th><th>∑1</th><th>∑2</th></tr>\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	for(int i = 0; i<NUMBER_OF_CHANNELS; ++i)
	{
		if(Channels[i].source.type)
		{
			if(IsTotalizerInEmail(&Channels[i].Tot1) && IsTotalizerInEmail(&Channels[i].Tot2))
			{
				n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<tr>");
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%d</td>",i+1);
				dbl2stri(tempText, Channels[i].Tot1.value, Channels[i].Tot1.resolution);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%s %s</td>",tempText,Channels[i].Tot1.unit);
				dbl2stri(tempText, Channels[i].Tot2.value, Channels[i].Tot2.resolution);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%s %s</td>",tempText,Channels[i].Tot2.unit);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "</tr>\r\n");
				SMTP_SendMailData(conn, sendBuffer);
				memset(sendBuffer,0,n);
			}
			else if(IsTotalizerInEmail(&Channels[i].Tot1))
			{
				n = mini_snprintf(sendBuffer, 512, "<tr>");
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%d</td>",i+1);
				dbl2stri(tempText, Channels[i].Tot1.value, Channels[i].Tot1.resolution);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%s %s</td>",tempText,Channels[i].Tot1.unit);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td></td>",tempText,Channels[i].Tot2.unit);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "</tr>\r\n");
				SMTP_SendMailData(conn, sendBuffer);
				memset(sendBuffer,0,n);
			}
			else if(IsTotalizerInEmail(&Channels[i].Tot2))
			{
				n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<tr>");
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%d</td>",i+1);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td></td>",tempText,Channels[i].Tot1.unit);
				dbl2stri(tempText, Channels[i].Tot2.value, Channels[i].Tot2.resolution);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "<td>%s %s</td>",tempText,Channels[i].Tot2.unit);
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER-n, "</tr>\r\n");
				SMTP_SendMailData(conn, sendBuffer);
				memset(sendBuffer,0,n);
			}
		}
	}
	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "</table></html>\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "\r\n.\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	SMTP_ReciveResponse(conn, recvBuffer,128);
}

static void EMAIL_SendAlarmReport(struct netconn *conn,EMAIL_SETTINGS *email , char alarmState[][2])
{
	char sendBuffer[SMTP_MAIL_BUFFER];
	char recvBuffer[128];
	int n = mini_snprintf(sendBuffer,SMTP_MAIL_BUFFER,"From: %s\r\n",email->client.email);
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer,SMTP_MAIL_BUFFER,"Subject: %s ALARM, ID:%d, %s\r\n",NAME,GeneralSettings.DeviceID,GeneralSettings.DeviceDescription);
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "To: ");
	for(int i=0;i<5;++i)
	{
		if(emailSettings.client.mailList[i][0]!=' ' && email->client.mailList[i][1]!='\0')
		n += mini_snprintf(sendBuffer + n,SMTP_MAIL_BUFFER,"%s,",&email->client.mailList[i][0]);
	}
	n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER, "\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "Content-Transfer-Encoding: 8bit\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "Content-Type: text/html; charset=\"UTF-8\"\r\n\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<html><style>table{width:20%%;} th,td{ border-bottom:1px solid #ddd; padding:7px; font-family:Arial; text-align:center; white-space:nowrap;}</style>");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<table><tr><th>CH. #</th><th>ALARM 1</th><th>ALARM 2</th></tr>\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	for(int i = 0; i<NUMBER_OF_CHANNELS; ++i)
	{
		if (alarmState[i][0] == 1 && alarmState[i][1] == 1)
		{
			n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<tr>");
			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>%d</td>", i + 1);

			if(Channels[i].alarm[0].state)
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>ON</td>");
			else
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>OFF</td>");

			if(Channels[i].alarm[1].state)
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>ON</td>");
			else
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>OFF</td>");

			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "</tr>\r\n");
			SMTP_SendMailData(conn, sendBuffer);
		}
		else if (alarmState[i][0] == 1)
		{

			n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<tr>");
			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>%d</td>", i + 1);
			if(Channels[i].alarm[0].state)
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>ON</td>");
			else
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>OFF</td>");

			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td></td>");
			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "</tr>\r\n");
			SMTP_SendMailData(conn, sendBuffer);
		}
		else if (alarmState[i][1] == 1)
		{

			n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "<tr>");
			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>%d</td>", i + 1);
			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td></td>");
			if(Channels[i].alarm[1].state)
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>ON</td>");
			else
				n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "<td>OFF</td>");
			n += mini_snprintf(sendBuffer + n, SMTP_MAIL_BUFFER - n, "</tr>\r\n");
			SMTP_SendMailData(conn, sendBuffer);

		}
	}

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "</table></html>\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	memset(sendBuffer,0,n);

	n = mini_snprintf(sendBuffer, SMTP_MAIL_BUFFER, "\r\n.\r\n");
	SMTP_SendMailData(conn, sendBuffer);
	SMTP_ReciveResponse(conn, recvBuffer,128);
}

static void SMTP_EHLO(struct netconn *conn, char *serverName)
{
	char recvBuffer[256] ={0};
	char sendBuffer[96] = {0};
	SMTP_SendRequest(conn, sendBuffer, "EHLO ", serverName);
	SMTP_ReciveResponse(conn, recvBuffer,256);
}

static void SMTP_ConnectionMessage(struct netconn *conn)
{
	char recvBuffer[128] = {0};
	SMTP_ReciveResponse(conn, recvBuffer,128);
}

static int SMTP_AuthLogin(struct netconn *conn, EMAIL_CLIENT *client)
{
	char recvBuffer[96] ={0};
	char sendBuffer[96] = {0};
	SMTP_SendRequest(conn, sendBuffer, "AUTH LOGIN", NULL);
	SMTP_ReciveResponse(conn, recvBuffer,96);

	if(SMTP_ChcekResponseCode(recvBuffer,"334"))
		return 1;

	memset(sendBuffer, 0, 96);
	SMTP_SendRequest(conn, sendBuffer, NULL, (char*) client->loginBase64);
	SMTP_ReciveResponse(conn, recvBuffer,96);

	if(SMTP_ChcekResponseCode(recvBuffer,"334"))
		return 1;

	memset(sendBuffer, 0, 96);
	SMTP_SendRequest(conn, sendBuffer, NULL, (char*) client->passwordBase64);
	SMTP_ReciveResponse(conn, recvBuffer,96);
	return SMTP_ChcekResponseCode(recvBuffer,"235");
}

static int SMTP_MailFrom(struct netconn *conn, char *mailAddress)
{
	char recvBuffer[96] ={0};
	char sendBuffer[96] = {0};

	strncpy(sendBuffer,"MAIL FROM:<",12);
	strncat(sendBuffer,mailAddress,64);
	strncat(sendBuffer,">\r\n",5);

	SMTP_SendMailData(conn, sendBuffer);
	SMTP_ReciveResponse(conn, recvBuffer,96);
	return SMTP_ChcekResponseCode(recvBuffer,"250");
}

static int SMTP_RecipientTo(struct netconn *conn,EMAIL_CLIENT *client)
{
	char recvBuffer[96] = {0};
	char sendBuffer[96] = {0};
	uint8_t nuberOfRecipients = 0;
	for (int i = 0; i<NUMBER_OF_EMAIL_RECIPIENTS; ++i)
	{
		if(client->mailList[i][0]!=' ' && client->mailList[i][1]!='\0')
		{
			memset(sendBuffer, 0, 96);
			strncpy(sendBuffer,"RCPT TO:<",12);
			strncat(sendBuffer,&client->mailList[i][0],64);
			strncat(sendBuffer,">\r\n",5);
			SMTP_SendMailData(conn, sendBuffer);
			SMTP_ReciveResponse(conn, recvBuffer,96);
			if(SMTP_ChcekResponseCode(recvBuffer,"250"))
				return 1;
			else
				nuberOfRecipients++;
		}
	}
	if(0==nuberOfRecipients)
		return 1;
	else
		return 0;
}

static void SMTP_DATA(struct netconn *conn)
{
	char sendBuffer[8] = {0};
	char recvBuffer[96] = {0};
	SMTP_SendRequest(conn, sendBuffer,"DATA", NULL);
	SMTP_ReciveResponse(conn, recvBuffer,96);
}

static void SMTP_QUIT(struct netconn *conn)
{
	char sendBuffer[8] = {0};
	char recvBuffer[96] = {0};
	SMTP_SendRequest(conn, sendBuffer,"QUIT", NULL);
	SMTP_ReciveResponse(conn, recvBuffer,96);
}

void vtaskAlarmEmail(void *pvParameters)
{
	char messageBuffer[NUMBER_OF_CHANNELS][2]={0};
	memcpy((void*)messageBuffer,pvParameters,NUMBER_OF_CHANNELS*2);

	EMAIL_SETTINGS email = {0};
	CopyEmailClientSettings(&email.client, &emailSettings.client);
	CopyEmailServerSettings(&email.server, &emailSettings.server);

	uint8_t connectionError = 0;
	struct netconn *client = NULL;

	netconn_gethostbyname(email.server.name, &email.server.IP);

	while(1)
	{
		client = NULL;
		client = SMTP_Connect(&email.server);
		vTaskDelay(5);
		if (client != NULL)
		{
			SMTP_ConnectionMessage(client);
			SMTP_EHLO(client, email.server.name);

			if(SMTP_AuthLogin(client, &email.client))
			{
				ARCHIVE_SendEvent("EMAIL:ERROR");
				SMTP_Disconnect(client);
				vTaskDelete(NULL);
			}

			if(SMTP_MailFrom(client, email.client.email))
			{
				ARCHIVE_SendEvent("EMAIL:ERROR");
				SMTP_Disconnect(client);
				vTaskDelete(NULL);
			}
			if(SMTP_RecipientTo(client,&email.client))
			{
				ARCHIVE_SendEvent("EMAIL:ERROR");
				SMTP_Disconnect(client);
				vTaskDelete(NULL);
			}
			SMTP_DATA(client);
			EMAIL_SendAlarmReport(client, &email, messageBuffer);

			SMTP_QUIT(client);

			ARCHIVE_SendEvent("EMAIL:OK");
			SMTP_Disconnect(client);
			vTaskDelete(NULL);
		}
		SMTP_Disconnect(client);
		if(++connectionError>=5)
		{
			ARCHIVE_SendEvent("EMAIL:ERROR");
			vTaskDelete(NULL);
		}
		vTaskDelay(500);
	}
}

void vtaskTotalizerEmail(void *pvParameters)
{
	EMAIL_SETTINGS email = {0};
	EMAIL_SETTINGS *tempEmail = (EMAIL_SETTINGS*)pvParameters;
	CopyEmailClientSettings(&email.client, &tempEmail->client);
	CopyEmailServerSettings(&email.server, &tempEmail->server);

	uint8_t connectionError = 0;
	struct netconn *client = NULL;
	netconn_gethostbyname(email.server.name, &email.server.IP);

	while(1)
	{
		client = NULL;
		client = SMTP_Connect(&email.server);
		vTaskDelay(5);
		if (client != NULL)
		{
			SMTP_ConnectionMessage(client);
			SMTP_EHLO(client, email.server.name);
			if(SMTP_AuthLogin(client, &email.client))
			{
				ARCHIVE_SendEvent("EMAIL:ERROR");
				SMTP_Disconnect(client);
				vTaskDelete(NULL);
			}

			if(SMTP_MailFrom(client, email.client.email))
			{
				ARCHIVE_SendEvent("EMAIL:ERROR");
				SMTP_Disconnect(client);
				vTaskDelete(NULL);
			}

			if(SMTP_RecipientTo(client,&email.client))
			{
				ARCHIVE_SendEvent("EMAIL:ERROR");
				SMTP_Disconnect(client);
				vTaskDelete(NULL);
			}

			SMTP_DATA(client);
			EMAIL_SendTotalizerReport(client,&email);

			SMTP_QUIT(client);

			ARCHIVE_SendEvent("EMAIL:OK");
			SMTP_Disconnect(client);
			vTaskDelete(NULL);
		}
		SMTP_Disconnect(client);
		if(++connectionError>=5)
		{
			ARCHIVE_SendEvent("EMAIL:ERROR");
			vTaskDelete(NULL);
		}
		vTaskDelay(500);
	}
}

void vtaskTestSMTPSettings(void *pvParameters)
{
	EMAIL_SETTINGS email = {0};
	EMAIL_SETTINGS *tempEmail = (EMAIL_SETTINGS*)pvParameters;
	CopyEmailClientSettings(&email.client, &tempEmail->client);
	CopyEmailServerSettings(&email.server, &tempEmail->server);

	uint8_t connectionError = 0;
	netconn_gethostbyname(email.server.name, &email.server.IP);

	struct netconn *client = NULL;

	while(1)
	{
		client = NULL;
		client = SMTP_Connect(&email.server);
		vTaskDelay(5);
		if (client != NULL)
		{
			SMTP_ConnectionMessage(client);

			SMTP_EHLO(client, email.server.name);
			if(SMTP_AuthLogin(client, &email.client))
			{
				SMTP_Disconnect(client);
				CreateMessage(GUI_LANG_GetText(215), GUI_YELLOW, GUI_BLACK);
				testEmailFlag = 0;
				vTaskDelete(NULL);
			}

			if(SMTP_MailFrom(client, email.client.email))
			{
				SMTP_Disconnect(client);
				CreateMessage(GUI_LANG_GetText(216), GUI_YELLOW, GUI_BLACK);
				testEmailFlag = 0;
				vTaskDelete(NULL);
			}
			if(SMTP_RecipientTo(client,&email.client))
			{
				SMTP_Disconnect(client);
				CreateMessage(GUI_LANG_GetText(217), GUI_YELLOW, GUI_BLACK);
				testEmailFlag = 0;
				vTaskDelete(NULL);
			}

			SMTP_DATA(client);
			EMAIL_SendTestReport(client,&email);

			SMTP_QUIT(client);
			SMTP_Disconnect(client);

			CreateMessage(GUI_LANG_GetText(218), GUI_YELLOW, GUI_BLACK);
			testEmailFlag = 0;
			vTaskDelete(NULL);
		}
		SMTP_Disconnect(client);
		if(++connectionError>=5)
		{
			CreateMessage(GUI_LANG_GetText(214), GUI_YELLOW, GUI_BLACK);
			testEmailFlag = 0;
			vTaskDelete(NULL);
		}
		vTaskDelay(500);
	}
}

void CreateAlarmEmailTask(void *message)
{
	xTaskCreate(vtaskAlarmEmail, "email_ALARM", 2048, (void*) message, (unsigned portBASE_TYPE ) 3, NULL);
}

void CreateTotalizerEmailReportTask(EMAIL_SETTINGS *parameters)
{
	xTaskCreate(vtaskTotalizerEmail, "email_TOT", 2048, (void*) parameters, (unsigned portBASE_TYPE ) 3, NULL);
}

void CreateTestEMAILTask(EMAIL_SETTINGS *parameters)
{
	if(!testEmailFlag)
	{
		testEmailFlag = 1;
		xTaskCreate(vtaskTestSMTPSettings, "email_TEST", 2048, (void*) parameters, (unsigned portBASE_TYPE ) 3, NULL);
	}
}


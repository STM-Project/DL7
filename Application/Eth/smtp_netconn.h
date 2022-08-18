/*
 * smtp_netconn.h
 *
 *  Created on: 14.02.2019
 *      Author: TomaszSok
 */

#ifndef ETH_SMTP_NETCONN_H_
#define ETH_SMTP_NETCONN_H_

#include "rtc.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#define SMTP_TEST 				0
#define SMTP_TOTALIZERS 	1
#define SMTP_ALARM 				2

#define SMTP_MAIL_BUFFER	512
#define MAIL_MESSAGE_BUFFER	6144
#define NUMBER_OF_EMAIL_RECIPIENTS 5

typedef struct
{
	char name[64];
	ip_addr_t IP;
	uint16_t port;
}SMTP_SERVER;

typedef struct
{
	char email[64];
	char login[64];
	char password[32];
	char mailList[NUMBER_OF_EMAIL_RECIPIENTS][64];

	uint8_t loginBase64[96];
	uint8_t passwordBase64[48];

	uint8_t useSSL;

}EMAIL_CLIENT;

typedef struct
{
	uint8_t mode;
	uint8_t hour;
	uint8_t day;
}EMAIL_EVENT;

typedef struct
{
	EMAIL_CLIENT client;
	SMTP_SERVER server;
	EMAIL_EVENT event;
}EMAIL_SETTINGS;

extern EMAIL_SETTINGS emailSettings;
extern EMAIL_SETTINGS bkEmailSettings;

void CopyEmailClientSettings(EMAIL_CLIENT *Dst, EMAIL_CLIENT *Src);
void CopyEmailServerSettings(SMTP_SERVER *Dst, SMTP_SERVER *Src);
void CopyEmailEventSettings(EMAIL_EVENT *Dst, EMAIL_EVENT *Src);

void EMAIL_EncodePasswordForSettingsFile(unsigned char *codedPassword, char *password);
void EMAIL_DecodePasswordFromSettingsFile( char *password,unsigned char *codedPassword);

void SMTP_SendCyceledEmail(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
void SMTP_ConvertLoginDataToBase64(EMAIL_CLIENT *client);

void EMAIL_InitDefuaultParameters(void);

void EMAIL_Init(void);

void SMTP_CreateAlarmMessage(char *buf, char *alarm, int channelNo);

void CreateAlarmEmailTask(void *message);
void CreateTotalizerEmailReportTask(EMAIL_SETTINGS *parameters);
void CreateTestEMAILTask(EMAIL_SETTINGS *parameters);


#endif /* ETH_SMTP_NETCONN_H_ */

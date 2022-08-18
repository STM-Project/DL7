/*
 * alarms.c
 *
 *  Created on: 7 sie 2015
 *      Author: Tomaszs
 */

#include <alarms.h>
#include <channels.h>
#include <ExpansionCards.h>
#include <ExpansionCards_typedef.h>
#include <watchdog.h>
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "ChannelsSynchronization.h"
#include "channels_typedef.h"
#include "archive.h"
#include "StartUp.h"
#include "skins.h"
#include "ExpansionCards.h"
#include "mini-printf.h"

#include "smtp_netconn.h"

#define ALARM_ON 			( 1 << 0 )
#define ALARM_OFF			( 1 << 1 )
#define ALARM_NEW			( 1 << 2 )
#define ACK_ALARMS			( 1 << 3 )

typedef struct
{
	uint8_t state;
	uint8_t active;
	uint8_t stateChanged;
	uint8_t changeFrequncy;
	uint8_t sendEmail;
}ALARM_STATE;

static uint8_t CheckAlarm(const float *ChannelValue, ALARM *alarm);
static void ManageAlarm(uint8_t ChannelNumber, ALARM *alarm, uint8_t AlarmNumber, ALARM_STATE *alarmStatus);
static void UpdateOutputs(void);
static uint8_t AckAllAlarms(EventBits_t event);
static uint32_t SetChannelColor(const ALARM *alarm1, const ALARM *alarm2);

xTaskHandle vtaskAlarmsHandle;
EventGroupHandle_t ALARMGroup;

static uint8_t tempOutputState[7][6] __attribute__ ((section(".sdram")));
static char AlarmMessage[20] __attribute__ ((section(".sdram")));



void vtaskAlarms(void *pvParameters)
{
	ALARM_STATE alarm = {0,0,0,0,0};
	uint8_t alarmEmailData[NUMBER_OF_CHANNELS][2]={0};

	uint8_t ChannelNumber = 0, IONumber = 0, BoardNumber = 0;

	EventBits_t event;

	ALARMGroup = xEventGroupCreate();

	while (1)
	{
		if (STARTUP_WaitForBits(0x0100))
		{
			STARTUP_ClaerBits(0x0100);
			STARTUP_SetBits(0x0200);

			while (1)
			{
				if (TakeChannelsMutex(200))
				{
					for (BoardNumber = 0; BoardNumber < 7; BoardNumber++)
						for (IONumber = 0; IONumber < 6; IONumber++)
							tempOutputState[BoardNumber][IONumber] = 0;

					event = xEventGroupWaitBits(ALARMGroup, 0x8, pdTRUE, 0x0, 5);
					AckAllAlarms(event);

					alarm.active = 0;
					alarm.state = 0;
					alarm.stateChanged = 0;
					alarm.changeFrequncy = 0;
					alarm.sendEmail = 0;

					for (ChannelNumber = 0; ChannelNumber < 100; ChannelNumber++)
					{
						if (Channels[ChannelNumber].source.type > 0)
						{
							alarm.stateChanged = CheckAlarm(&Channels[ChannelNumber].value, &Channels[ChannelNumber].alarm[0]);
							ManageAlarm(ChannelNumber, &Channels[ChannelNumber].alarm[0], 1, &alarm);

							if(alarm.stateChanged && Channels[ChannelNumber].alarm[0].sendEmail)
							{
								alarm.sendEmail = 1;
								alarmEmailData[ChannelNumber][0] = 1;
							}
							else
								alarmEmailData[ChannelNumber][0] = 0;

							alarm.stateChanged = CheckAlarm(&Channels[ChannelNumber].value, &Channels[ChannelNumber].alarm[1]);
							ManageAlarm(ChannelNumber, &Channels[ChannelNumber].alarm[1], 2, &alarm);

							if(alarm.stateChanged && Channels[ChannelNumber].alarm[1].sendEmail)
							{
								alarm.sendEmail = 1;
								alarmEmailData[ChannelNumber][1] = 1;
							}
							else
								alarmEmailData[ChannelNumber][1] = 0;

							Channels[ChannelNumber].Color = SetChannelColor(&Channels[ChannelNumber].alarm[0], &Channels[ChannelNumber].alarm[1]);
						}
					}

					if (alarm.active == 0)
						xEventGroupSetBits(ALARMGroup, ALARM_OFF);
					else
						xEventGroupSetBits(ALARMGroup, ALARM_NEW);

					if (alarm.state == 1)
						xEventGroupSetBits(ALARMGroup, ALARM_ON);

					if (alarm.changeFrequncy ^ ARCHIVE_GetFrequencyState())
						ARCHIVE_SetFrequencyState(alarm.changeFrequncy);

					if(alarm.sendEmail == 1)
						CreateAlarmEmailTask(&alarmEmailData[0][0]);

					UpdateOutputs();
					GiveChannelsMutex();
				}

				WDFlags[3] = ALIVE;
				vTaskDelay(500);
			}
		}
		else
			continue;
	}
}

void CreateAlarmsTask(void)
{
	xTaskCreate(vtaskAlarms, "vtaskAlarms", 1000, NULL, ( unsigned portBASE_TYPE )3, &vtaskAlarmsHandle);
}

static uint8_t CheckAlarm(const float *ChannelValue, ALARM *alarm)
{
	switch (alarm->type)
	{
	case 0: //alarm wyłączony
		break;
	case 1: //dolny
		if (alarm->state == 0 && *ChannelValue < alarm->level) //zmiana z niekatywnego na aktywny
		{
			alarm->AckAlarm = alarm->mode;
			alarm->state = 1;
			return 1;
		}

		else if (alarm->state == 1 && *ChannelValue >= alarm->level + alarm->hysteresis && alarm->AckAlarm == 1) //zmiana z aktywnego na nieaktywny
		{
			alarm->state = 0;
			return 1;
		}
		break;

	case 2: //górny
		if (alarm->state == 0 && *ChannelValue > alarm->level)
		{
			alarm->AckAlarm = alarm->mode;
			alarm->state = 1;
			return 1;
		}
		else if (alarm->state == 1 && *ChannelValue <= alarm->level - alarm->hysteresis && alarm->AckAlarm == 1)
		{
			alarm->state = 0;
			return 1;
		}
		break;
	}
	return 0;
}

static void ManageAlarm(uint8_t ChannelNumber, ALARM *alarm, uint8_t AlarmNumber, ALARM_STATE *alarmStatus)
{
	if (alarm->AckAlarm == 0 && alarm->mode == 0)
		alarmStatus->active = 1;

	if (alarm->AckAlarm == 1 && alarm->mode == 0 && alarm->state == 1)
		alarmStatus->state = 1;

	if (alarm->state)
	{
		if (alarmStatus->stateChanged)
		{
			if (alarm->logEvent)
			{
				mini_snprintf(AlarmMessage, 20, "AL:CH%03d AL%d ON", ChannelNumber + 1, AlarmNumber);
				ARCHIVE_SendEvent(AlarmMessage);
			}
		}
		if (alarm->output.type)
			tempOutputState[alarm->output.board][alarm->output.number] = 1;

		if (alarm->ChangeArchivizationFrequency == 1)
			alarmStatus->changeFrequncy = 1;
	}
	else
	{
		if (alarmStatus->stateChanged)
		{
			if (alarm->logEvent)
			{
				mini_snprintf(AlarmMessage, 20, "AL:CH%03d AL%d OFF", ChannelNumber + 1, AlarmNumber);
				ARCHIVE_SendEvent(AlarmMessage);
			}
		}
	}
}

static void UpdateOutputs(void)
{
	for (int BoardNumber = 0; BoardNumber < 7; BoardNumber++)
	{
		if (ExpansionCards[BoardNumber].type == SPI_CARD_OUT6RL)
		{
			for (int IONumber = 0; IONumber < 6; IONumber++)
				ExpansionCards[BoardNumber].channel[IONumber].value = tempOutputState[BoardNumber][IONumber];
		}
	}
}

static uint8_t AckAllAlarms(EventBits_t event)
{
	if ((event & ACK_ALARMS) != 0)
	{
		for (int ChannelNumber = 0; ChannelNumber < 100; ChannelNumber++)
		{
			Channels[ChannelNumber].alarm[0].AckAlarm = 1;
			Channels[ChannelNumber].alarm[1].AckAlarm = 1;
		}
		ARCHIVE_SendEvent("AL:ACK");
		return 1;
	}
	else
		return 0;
}

static uint32_t SetChannelColor(const ALARM *alarm1, const ALARM *alarm2)
{
	if (!alarm1->state & !alarm2->state)
		return SKINS_GetTextColor();
	else if (alarm1->state > alarm2->state)
		return ALARMS_GetColor(alarm1->color);
	else if (alarm1->state < alarm2->state)
		return ALARMS_GetColor(alarm2->color);
	else if (alarm1->state == alarm2->state)
	{
		if (alarm1->color >= alarm2->color)
			return ALARMS_GetColor(alarm1->color);
		else
			return ALARMS_GetColor(alarm2->color);
	}
	else
		return SKINS_GetTextColor();
}

uint32_t ALARMS_GetColor(uint8_t AlarmColor)
{
	switch (AlarmColor)
	{
	case 1:
		return 0x0000c000;
	case 2:
		return GUI_YELLOW;
	case 3:
		return GUI_RED;
	default:
		return SKINS_GetTextColor();
	}
}

void ALARMS_AckAll(void)
{
	xEventGroupSetBits(ALARMGroup, ACK_ALARMS);
}

uint8_t ALARMS_GetState(void)
{
	EventBits_t event = 0xFF;
	uint8_t alarmsState;
	event = xEventGroupWaitBits(ALARMGroup, 0x7, pdTRUE, pdFALSE, 5);
	if ((event & ALARM_NEW))
		alarmsState = 2;
	else if ((event & ALARM_ON) != 0)
		alarmsState = 1;
	else if ((event & ALARM_OFF) != 0)
		alarmsState = 0;
	else
		alarmsState = 0xFF;
	return alarmsState;
}

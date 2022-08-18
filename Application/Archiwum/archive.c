#include "archive.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "task.h"
#include "channels.h"
#include "ff.h"
#include "NSMAC.h"
#include "watchdog.h"
#include "version.h"
#include "parameters.h"
#include "rtc.h"
#include "dtos.h"
#include "crc.h"
#include "UserMessage.h"
#include "StartUp.h"
#include "fram.h"
#include "shift_reg.h"
#include "ChannelsSynchronization.h"
#include "mini-printf.h"
#include "PopUpMessageDLG.h"

#define BUFFER_HEADER_SIZE 24576
#define DATA_ARCHIVE_BUFFER_LENGHT	20480
#define DATA_ARCHIVE_MAX_DATA_IN_BUFFER	18000
#define TOTALZIER_ARCHIVE_BUFFER_LENGHT 18000

#define ARCH_OK			0
#define ARCH_ERROR	1

typedef enum
{
 STATE_POLLING,
 STATE_COLLECT,
 STATE_FORM,
 STATE_PUT,
 STATE_SYNC
} vArchEvent;

const int archDataFrequency[15] =
{ 2, 5, 10, 15, 30, 60, 300, 600, 900, 1800, 3600, 7200, 14400, 43200, 86400 };

const int archTotalizersFrequency[10] =
{ 60, 300, 600, 900, 1800, 3600, 7200, 14400, 43200, 86400 };

const int archMaximumRecordsInBuffer[5] =
{30,12,6,4,2};

static char archBufferArchive[DATA_ARCHIVE_BUFFER_LENGHT] __attribute__((aligned(4))) = {0};
static char archBufferTotalizer[TOTALZIER_ARCHIVE_BUFFER_LENGHT] __attribute__((aligned(4))) = {0};
static char archBufferHeader[BUFFER_HEADER_SIZE] __attribute__ ((section(".sdram")));

static unsigned int buffArch = 0;
static unsigned int numberOfRecordsInbuffArch = 0;
static uint8_t FileID = 0;
static int archPollingState = 0;
static uint8_t ArchivizationFrequencyState = 0;

static char archDataCRC1[5] = {0};
static char archTotalizerCRC1[5] = {0};
static char archEventCRC1[5] = {0};
static char archDataMeasCRC2[5] = {0};
static char archTotalizerMeasCRC2[5] = {0};
static char archEventMeasCRC2[5] = {0};

static uint8_t archFirstRecordOnPowerUp = 0;

static char archDataFilePath[32] = {0};
static char archTotalizerFilePath[32] = {0};
static char archEventFilePath[32] = {0};

RTC_TimeTypeDef RTCTimeArch, RTCLastTimeData, RTCLastTimeCount, RTCTimeEvent , RTCPowerFailTime;
RTC_DateTypeDef RTCDateArch, RTCPowerFailDate;

xQueueHandle xDataQueue, xDataResolution, xDataStatusQueue;
xQueueHandle xTotalizerQueue, xTotalizerResolution;
xQueueHandle xChannelsQueue, xMessageQueue;

xTaskHandle vtaskArchiveHandle;
EventGroupHandle_t xArchiveEventGroup;
SemaphoreHandle_t xSwitchSemaphore;
SemaphoreHandle_t xArchiveWrite;

static int GetDataArchiveFreqency(uint8_t freqChoose)
{
	if (freqChoose < 15)
		return archDataFrequency[freqChoose];
	else
		return archDataFrequency[0];
}

static int GetTotalizersArchiveFrequency(uint8_t freqChoose)
{
	if (freqChoose < 10)
		return archTotalizersFrequency[freqChoose];
	else
		return archTotalizersFrequency[0];
}

static int GetMaximumRecordsInBuffer(uint8_t freqChoose)
{
	if (freqChoose < 5)
		return archMaximumRecordsInBuffer[freqChoose];
	else
		return archMaximumRecordsInBuffer[0];
}

static uint8_t archGetNewFileID(void)
{
	uint8_t NewArchID = FRAM_Read(CURRENT_ARCH_ADDR);
	if (NewArchID >= 99)
		NewArchID = 0;
	else
		NewArchID++;
	return NewArchID;
}

static void archSaveNewFileID(uint8_t NewFileID)
{
	FileID = NewFileID;
	FRAM_Write(CURRENT_ARCH_ADDR, NewFileID);
}

static void archErrorStop(void)
{
	FRAM_Write(ARCHIVE_STATE_ADDR, 0);
	archPollingState = 0;
	CreateMessage(GUI_LANG_GetText(266),GUI_RED,GUI_BLACK);
	USERMESSAGE_Archive(archPollingState);
}

static void SetDataArchiveFilePath(void)
{
	mini_snprintf(archDataFilePath,32, "0:/archive/%dAD%02d.csv", GeneralSettings.DeviceID, FileID);
}

static void SetTotalizerArchiveFilePath(void)
{
	mini_snprintf(archTotalizerFilePath,32, "0:/archive/%dAT%02d.csv", GeneralSettings.DeviceID, FileID);
}

static void SetEventArchiveFilePath(void)
{
	mini_snprintf(archEventFilePath,32, "0:/archive/%dAE%02d.csv", GeneralSettings.DeviceID, FileID);
}

static int archivedDataAvailable(void)
{
	for (int i = 0; i < 100; i++)
	{
		if (Channels[i].ToArchive == 1)
			return 1;
	}
	return 0;
}

static int archivedTotalizerAvailable(void)
{
	for (int i = 0; i < 100; i++)
	{
		if (Channels[i].Tot1.ToArchive == 1)
			return 1;
		else if (Channels[i].Tot2.ToArchive == 1)
			return 1;
	}
	return 0;
}

static int checkFrequency(int frequencyValue, RTC_TimeTypeDef RTCTime)
{
	int checkValue=-1;
	if (frequencyValue >= 1 && frequencyValue <= 60)
	{
		checkValue = RTCTime.Seconds % frequencyValue;
		if (checkValue == 0)
			return 1;
		else
			return 0;
	}
	else if (frequencyValue >= 300 && frequencyValue <= 3600 && RTCTime.Seconds == 0)
	{
		checkValue = RTCTime.Minutes % (frequencyValue / 60);
		if (checkValue == 0)
			return 1;
		else
			return 0;
	}
	else if (frequencyValue >= 7200 && frequencyValue <= 86400 && RTCTime.Seconds == 0 && RTCTime.Minutes == 0)
	{
		checkValue = RTCTime.Hours % (frequencyValue / 3600);
		if (checkValue == 0)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

static uint8_t archCheckTotalizersPeriod(void)
{
	 int archMeasPeriod = GetTotalizersArchiveFrequency((uint8_t) ArchiveSettings.TotalizerArchivizationFrequency);

	if (checkFrequency(archMeasPeriod, RTCTimeArch) == 1)
		return 1;
	else
		return 0;
}

static uint8_t archCheckDataPeriod(void)
{
	int archMeasPeriod = -1;
	if (ArchivizationFrequencyState == 0)
		archMeasPeriod = GetDataArchiveFreqency((uint8_t) ArchiveSettings.ArchivizationFrequency1);
	else if (ArchivizationFrequencyState == 1)
		archMeasPeriod = GetDataArchiveFreqency((uint8_t) ArchiveSettings.ArchivizationFrequency2);

	if (checkFrequency(archMeasPeriod, RTCTimeArch) == 1)
		return 1;
	else
		return 0;
}

static uint8_t FREERTOS_StoreDataArchiveToSD(void)
{
	FIL FilData={0};
	uint8_t ret = ARCH_ERROR;
	FRESULT res = FR_OK;
	FSIZE_t tempfPtr = 0;
	int WrittenDataLenght = 0;
	int bufferLenght = strlen(archBufferArchive);

	if(ARCHIVE_TakeSemaphore(500))
	{
		res = f_open(&FilData, archDataFilePath, FA_WRITE | FA_OPEN_APPEND);
		if (FR_OK == res)
		{
			res = f_lseek(&FilData, f_size(&FilData));
			if (FR_OK == res)
			{
				tempfPtr = FilData.fptr;

				WrittenDataLenght = f_puts(archBufferArchive, &FilData);

				if (EOF == WrittenDataLenght)
				{
					if(FR_OK == f_close(&FilData))
					{
						if(FR_OK == f_open(&FilData, archDataFilePath, FA_WRITE | FA_OPEN_APPEND))
						{
							if(FR_OK == f_lseek(&FilData, tempfPtr))
							{
								if(FR_OK != f_truncate(&FilData))
								{
									archErrorStop();
									ARCHIVE_GiveSemaphore();
									return ARCH_ERROR;
								}
							}
							else
							{
								archErrorStop();
								ARCHIVE_GiveSemaphore();
								return ARCH_ERROR;
							}
						}
						else
						{
							archErrorStop();
							ARCHIVE_GiveSemaphore();
							return ARCH_ERROR;
						}
					}
					else
					{
						archErrorStop();
						ARCHIVE_GiveSemaphore();
						return ARCH_ERROR;
					}
				}
				else
				{
					f_sync(&FilData);
					tempfPtr = FilData.fptr;
					memset(archBufferArchive, 0, WrittenDataLenght);
					buffArch = 0;
					numberOfRecordsInbuffArch = 0;
					ret = ARCH_OK;
				}
			}
			else
			{
				if(DATA_ARCHIVE_MAX_DATA_IN_BUFFER < bufferLenght)
					archErrorStop();
			}

			int reTryCounter = 0;
			do
			{
					res = f_close(&FilData);
					if (FR_OK != res)
						vTaskDelay(50);
			} while (++reTryCounter < 5 && FR_OK != res);

			if (5 <= reTryCounter && FR_OK != res)
			{
				archErrorStop();
			}
		}
		else
		{
			if(DATA_ARCHIVE_MAX_DATA_IN_BUFFER < bufferLenght)
				archErrorStop();
		}
		ARCHIVE_GiveSemaphore();
	}
	return ret;
}

static void FREERTOS_StoreTotalizerArchiveToSD(void)
{
	FIL FilCount = {0};
	FRESULT res = FR_OK;
	int WrittenTotalizerDataLenght = 0;
	int bufferLenght = strlen(archBufferTotalizer);
	if (ARCHIVE_TakeSemaphore(500))
	{
		res = f_open(&FilCount, archTotalizerFilePath, FA_WRITE | FA_OPEN_EXISTING);
		if (FR_OK == res)
		{
			res = f_lseek(&FilCount, f_size(&FilCount));
			if(FR_OK == res)
			{
				WrittenTotalizerDataLenght = f_puts(archBufferTotalizer, &FilCount);
				f_sync(&FilCount);

				if (EOF == WrittenTotalizerDataLenght)
				{
					if(TOTALZIER_ARCHIVE_BUFFER_LENGHT < bufferLenght)
						archErrorStop();
				}
				else
				{
					memset(archBufferTotalizer, 0, WrittenTotalizerDataLenght);
				}
			}

			int reTryCounter = 0;
			do
			{
				res = f_close(&FilCount);
				if(FR_OK != res)
					vTaskDelay(50);
			}while(++reTryCounter < 5 && FR_OK != res);

			if(5 <= reTryCounter && FR_OK != res)
			{
				archErrorStop();
			}
		}
		else
		{
			if(16000 < bufferLenght)
				archErrorStop();
		}
		ARCHIVE_GiveSemaphore();
	}
}

/**
 * Data archive polling.
 *
 * Steps of polling data measurements:
 * - STATE_POLLING: Waiting for semaphore to gather measurements
 * - STATE_COLLECT: Gathering measurements from queue to buffer
 * - STATE_FORM: Forming string using measurements in buffer
 * - STATE_PUT: Pass formatted string to buffer collecting data before saving it to file on SDCard, and save to SD if nessecery
 *
 * @param none
 * @return none
 */
static void archDataPolling(void)
{
	float archDataVal[100]={0};
	uint8_t archDataStatus[100]={0};
	uint8_t archDataResolution[100]={0};
	char FilArchive[2200]={0};
	int archivePeriod=0;
	int maximumBufferedRcords=0;

	vArchEvent eventArch = STATE_POLLING;

	char CiphCRC[5]={0};

	int queueDataSize = 0;

	if ( TakeChannelsMutex(200))
		eventArch = STATE_COLLECT;

	if (eventArch == STATE_COLLECT)
	{
		if(archCheckDataPeriod())
		{
			while (uxQueueMessagesWaiting(xDataQueue) > 0)
			{
				xQueueReceive(xDataQueue, &archDataVal[queueDataSize], 0);
				xQueueReceive(xDataStatusQueue, &archDataStatus[queueDataSize], 0);
				xQueueReceive(xDataResolution, &archDataResolution[queueDataSize], 0);
				queueDataSize++;
			}
			if (queueDataSize > 0)
				eventArch = STATE_FORM;
		}
		GiveChannelsMutex();
	}

	if (eventArch == STATE_FORM)
	{
		if (((RTCLastTimeData.Hours != RTCTimeArch.Hours) || (RTCLastTimeData.Minutes != RTCTimeArch.Minutes)
				|| (RTCLastTimeData.Seconds != RTCTimeArch.Seconds)))
		{
			RTCLastTimeData = RTCTimeArch;

			mini_snprintf(FilArchive,2200, "%02d-%02d-%02d,%02d:%02d:%02d,%d,", RTCDateArch.Year, RTCDateArch.Month, RTCDateArch.Date, RTCTimeArch.Hours,
					RTCTimeArch.Minutes, RTCTimeArch.Seconds, DST_GetFlagState());

			char archDataValString[21]={0};

			if(1 == archFirstRecordOnPowerUp)
			{
				archFirstRecordOnPowerUp = 0;
				for (int i = 0; i < queueDataSize; i++)
					strcat(FilArchive, "--W--,");
			}
			else
			{
				for (int i = 0; i < queueDataSize; i++)
				{
					switch(archDataStatus[i])
					{
					case 0:
						strcat(FilArchive, "---");
						break;
					case 1:
						float2stri(archDataValString,archDataVal[i],archDataResolution[i]);
						strcat(FilArchive, archDataValString);
						break;
					case 0x0A:
						strcat(FilArchive, "--W--");
						break;
					case 0x02:
						strcat(FilArchive, "--||--");
						break;
					case 0x03:
						strcat(FilArchive, "--E--");
						break;
					case 0x04:
						strcat(FilArchive, "--R--");
						break;
					default:
						strcat(FilArchive, "--ERR--");
						break;
					}
					strcat(FilArchive,",");
				}
			}
			WORD tempCRC = 0xFFFF;
			generateCRC(FilArchive,strlen(FilArchive),&tempCRC);
			generateCRC(archDataMeasCRC2,4,&tempCRC);
			hashCRC(CiphCRC,&tempCRC);
			strcat(FilArchive, CiphCRC);
			strcat(FilArchive, "\n");
			strcpy(archDataMeasCRC2, CiphCRC);
			eventArch = STATE_PUT;
		}
	}

	if (eventArch == STATE_PUT)
	{
		if (ARCHIVE_GetFrequencyState() == 0)
		{
			archivePeriod = GetDataArchiveFreqency((uint8_t) ArchiveSettings.ArchivizationFrequency1);
			maximumBufferedRcords = GetMaximumRecordsInBuffer ((uint8_t) ArchiveSettings.ArchivizationFrequency1);
		}
		else
		{
			archivePeriod = GetDataArchiveFreqency((uint8_t) ArchiveSettings.ArchivizationFrequency2);
			maximumBufferedRcords = GetMaximumRecordsInBuffer ((uint8_t) ArchiveSettings.ArchivizationFrequency2);
		}

		if (archivePeriod < 60)
		{
			int recordLength = strlen(FilArchive);
			if (numberOfRecordsInbuffArch > maximumBufferedRcords || buffArch + recordLength >= 16000)
				FREERTOS_StoreDataArchiveToSD();
			strcat(archBufferArchive,FilArchive);
			buffArch += recordLength;
			numberOfRecordsInbuffArch++;
		}
		else
		{
			strcat(archBufferArchive, FilArchive);
			FREERTOS_StoreDataArchiveToSD();
		}
	}
	memset(FilArchive, 0, strlen(FilArchive));
}

/**
 * Totalizers archive polling.
 *
 * Steps of polling data measurements:
 * - STATE_POLLING: Waiting for semaphore to gather measurements
 * - STATE_COLLECT: Gathering measurements from queue to buffer
 * - STATE_FORM: Forming string using measurements in buffer
 * - STATE_PUT: Pass formatted string to buffer collecting data before saving it to file on SDCard and Saving data to file
 *
 * @param none
 * @return none
 */
static void archTotalizerPolling(void)
{
	double archTotalizerVal[200]={0};
	char totalizerArchiveResolution[200]={0};
	char recordBuffer[4400]={0};
	char CiphCRC[5]={0};
	int queueTotalizerSize = 0;

	vArchEvent eventArch = STATE_POLLING;

	if ( TakeChannelsMutex(200))
		eventArch = STATE_COLLECT;

	if (eventArch == STATE_COLLECT)
	{
		if (archCheckTotalizersPeriod())
		{
			while (uxQueueMessagesWaiting(xTotalizerQueue) > 0)
			{
				xQueueReceive(xTotalizerQueue, &archTotalizerVal[queueTotalizerSize], 0);
				xQueueReceive(xTotalizerResolution, &totalizerArchiveResolution[queueTotalizerSize], 0);
				queueTotalizerSize++;
			}
			if (queueTotalizerSize > 0)
				eventArch = STATE_FORM;
		}
		GiveChannelsMutex();
	}

	if (eventArch == STATE_FORM)
	{
		if ((RTCLastTimeCount.Hours != RTCTimeArch.Hours) || (RTCLastTimeCount.Minutes != RTCTimeArch.Minutes)
						|| (RTCLastTimeCount.Seconds != RTCTimeArch.Seconds))
		{
			RTCLastTimeCount = RTCTimeArch;

			mini_snprintf(recordBuffer,22, "%02d-%02d-%02d,%02d:%02d:%02d,%d,", RTCDateArch.Year, RTCDateArch.Month, RTCDateArch.Date, RTCTimeArch.Hours,
					RTCTimeArch.Minutes, RTCTimeArch.Seconds, DST_GetFlagState());

			char archCountValString [21]={0};

			for (int i = 0; i < queueTotalizerSize; i++)
			{
				if (isnan(archTotalizerVal[i]))
					strcat(recordBuffer,"-----,");
				else
				{
					{
						dbl2stri(archCountValString,archTotalizerVal[i],totalizerArchiveResolution[i]);
						strcat(recordBuffer,archCountValString);
						strcat(recordBuffer,",");
					}
				}
			}

			WORD tempCRC = 0xFFFF;
			generateCRC(recordBuffer,strlen(recordBuffer),&tempCRC);
			generateCRC(archTotalizerMeasCRC2,4,&tempCRC);
			hashCRC(CiphCRC,&tempCRC);

			strcat(recordBuffer,CiphCRC);
			strcat(recordBuffer,"\n");
			strcpy(archTotalizerMeasCRC2, CiphCRC);
			strcat(archBufferTotalizer,recordBuffer);
			FREERTOS_StoreTotalizerArchiveToSD();
		}
	}
}

/**
 * Event archive polling.
 *
 * Function waits for events in queue, then forming string using time of receiving event.
 * Exceptional situation is when device is turning off: time is based on the time of brownout interrupt.
 * @param none
 * @return none
 */
static void archEventPolling(void)
{
	char* eventArchMessage;
	char archBufferEvent[150]={0};
	char FilDate[21]={0};
	char CiphCRC[5]={0};
	FIL FilEvent={0};
	int queueSize = uxQueueMessagesWaiting(xMessageQueue);

	for (int i = 0; i < queueSize; i++)
	{
		uxQueueMessagesWaiting(xMessageQueue);
		if (xQueueReceive(xMessageQueue, &(eventArchMessage), portMAX_DELAY))
		{
			if (FRAM_Read(ARCH_RTC_HOUR) != 0)
			{
				RTCPowerFailTime.Hours = FRAM_Read(ARCH_RTC_HOUR);
				RTCPowerFailTime.Minutes = FRAM_Read(ARCH_RTC_MINUTES);
				RTCPowerFailTime.Seconds = FRAM_Read(ARCH_RTC_SECONDS);
				RTCPowerFailDate.Date = FRAM_Read(ARCH_RTC_DATE);
				RTCPowerFailDate.Month = FRAM_Read(ARCH_RTC_MONTH);
				RTCPowerFailDate.Year = FRAM_Read(ARCH_RTC_YEAR);
				RTCPowerFailTime.DayLightSaving = FRAM_Read(ARCH_RTC_DST);
				mini_snprintf(FilDate,21, "%02d-%02d-%02d,%02d:%02d:%02d,%d,", RTCPowerFailDate.Year, RTCPowerFailDate.Month, RTCPowerFailDate.Date,
						RTCPowerFailTime.Hours, RTCPowerFailTime.Minutes, RTCPowerFailTime.Seconds, (int)RTCPowerFailTime.DayLightSaving);
				uint8_t framZeroes[6] = { 0 };
				FRAM_WriteMultiple(ARCH_RTC_HOUR, framZeroes, 6);
			}
			else
			{
				RTC_GetTimeAndDate(&RTCTimeEvent, &RTCDateArch);
				mini_snprintf(FilDate,21, "%02d-%02d-%02d,%02d:%02d:%02d,%d,", RTCDateArch.Year, RTCDateArch.Month, RTCDateArch.Date, RTCTimeEvent.Hours,
						RTCTimeEvent.Minutes, RTCTimeEvent.Seconds, DST_GetFlagState());
			}
			strcpy(archBufferEvent,FilDate);
			strcat(archBufferEvent,eventArchMessage);
			strcat(archBufferEvent,",");

			WORD tempCRC = 0xFFFF;
			generateCRC(archBufferEvent,strlen(archBufferEvent),&tempCRC);
			generateCRC(archEventMeasCRC2,4,&tempCRC);
			hashCRC(CiphCRC,&tempCRC);

			strcat(archBufferEvent,CiphCRC);
			strcat(archBufferEvent,"\n");
			strcpy(archEventMeasCRC2, CiphCRC);
			uxQueueMessagesWaiting(xMessageQueue);
			if(ARCHIVE_TakeSemaphore(500))
			{
				f_open(&FilEvent, archEventFilePath, FA_OPEN_EXISTING | FA_WRITE);
				f_lseek(&FilEvent, f_size(&FilEvent));
				f_printf(&FilEvent, archBufferEvent);
				f_close(&FilEvent);
				ARCHIVE_GiveSemaphore();
			}
			vPortFree(eventArchMessage);
		}
	}
}

/**
 * Creating header for data's archive file
 *
 * @param bytesNum pointer to number of bytes in header
 * @param archDataString pointer to string with header
 * @return none
 */
static void archInitData(char *archDataString)
{
	char archHeadString[613] =
	{ 0 };
	char CiphCRC[5];
	int HeadStringLength = 0;
	int DataStringLength = 0;
	int NumberChannelsToArchive = 0;
	buffArch = 0;
	char minBuffer[31]={0};
	char maxBuffer[31]={0};


	HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,613, "\nDATE,TIME,DST,");

	for (int i = 0; i < 100; i++)
	{
		if (Channels[i].ToArchive == 1)
		{
			HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,613, "CH %d,", i + 1);
			NumberChannelsToArchive++;
		}
	}

	HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,613, "CRC2\n");

	NumberChannelsToArchive += 3;
	DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "%s,%s,%d,%d,%d,DATA,", NAME, VERSION, GetSerialNumber(),
			GeneralSettings.DeviceID, NumberChannelsToArchive);
	int CRClocation = DataStringLength;

	DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "\n\nCHANNEL,DESCRIPTION,UNIT,INPUT TYPE,INPUT NO,TREND MIN,TREND MAX\n");

	for (int i = 0; i < 100; i++)
	{
		if (Channels[i].ToArchive == 1)
		{

			DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "%d,%s,%s,", i + 1, Channels[i].description, Channels[i].unit);
			switch (Channels[i].source.type)
			{
			case 1:
				DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "ME,%c%d,", Channels[i].source.board + 'A',
						Channels[i].source.number + 1);
				break;
			case 2:
				DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "CO,--,");
				break;
			case 3:
				DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "RE,%d,", Channels[i].source.number + 1);
				break;
			case 4:
				DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "DE,--,");
				break;
			default:
				DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "--,--,");
				break;
			}

			float2striTrimmed(minBuffer, Channels[i].graph_min_value, 6);
			float2striTrimmed(maxBuffer, Channels[i].graph_max_value, 6);

			DataStringLength += mini_snprintf(archDataString + DataStringLength,BUFFER_HEADER_SIZE, "%s,%s\n", minBuffer, maxBuffer);
		}
	}

	calculateCRC(archDataString, strlen(archDataString), CiphCRC);
	strcpy(archDataCRC1, CiphCRC);
	strcpy(archDataMeasCRC2, CiphCRC);

	memmove(archDataString + CRClocation + 4, archDataString + CRClocation, DataStringLength);
	memcpy(archDataString + CRClocation, (char *) CiphCRC, 4);
	strcat(archDataString, archHeadString);

}

/**
 * Creating header for totalizers archive file
 *
 * @param bytesNum pointer to number of bytes in header
 * @param archCountString pointer to string with header
 * @return none
 */
static void archInitTotalizer(char *archCountString)
{
	char archHeadString[1605] =	{ 0 };
	char CiphCRC[5]={0};
	int HeadStringLength = 0;
	int DataStringLength = 0;
	int numberOfHeaderLines = 0;

	for (int i = 0; i < 100; i++)
	{
		if ((Channels[i].Tot1.ToArchive != 0) || (Channels[i].Tot2.ToArchive != 0))
			numberOfHeaderLines++;
	}

	numberOfHeaderLines += 3;
	DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, "%s,%s,%d,%d,%d,TOT,", NAME, VERSION, GetSerialNumber(), GeneralSettings.DeviceID,
			numberOfHeaderLines);

	int CRClocation = DataStringLength;
	DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength,
			"\n\nCHANNEL,DESCRIPTION,TOTALIZER 1 TYPE,TOTALIZER 1 UNIT,TOTALIZER 2 TYPE,TOTALIZER 2 UNIT\n");

	HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,1605-HeadStringLength, "\nDATE,TIME,DST");

	for (int i = 0; i < 100; i++)
	{
		if ((Channels[i].Tot1.ToArchive != 0) || (Channels[i].Tot2.ToArchive != 0))
		{
			DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, "%d,%s", i + 1, Channels[i].description);

			if ((Channels[i].Tot1.ToArchive != 0))
			{
				DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, ",%d,%s", Channels[i].Tot1.type, Channels[i].Tot1.unit);
				HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,1605-HeadStringLength, ",CH%d:T1", i + 1);
			}
			else
				DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, ", , ");

			if ((Channels[i].Tot2.ToArchive != 0))
			{
				DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, ",%d,%s", Channels[i].Tot2.type, Channels[i].Tot2.unit);
				HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,1605-HeadStringLength, ",CH%d:T2", i + 1);
			}
			else
				DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, ", , ");

			DataStringLength += mini_snprintf(archCountString + DataStringLength,BUFFER_HEADER_SIZE-DataStringLength, "\n");
		}
	}

	HeadStringLength += mini_snprintf(archHeadString + HeadStringLength,1605-HeadStringLength, ",CRC2\n");

	calculateCRC(archCountString, strlen(archCountString), CiphCRC);
	strcpy(archTotalizerCRC1, CiphCRC);
	strcpy(archTotalizerMeasCRC2, CiphCRC);

	memmove(archCountString + CRClocation + 4, archCountString + CRClocation, DataStringLength);
	memcpy(archCountString + CRClocation, (char *) CiphCRC, 4);
	strcat(archCountString, archHeadString);
}

/**
 * Creating header for event archive file
 *
 * @param bytesNum pointer to number of bytes in header
 * @param archEventString pointer to string with header
 * @return none
 */
static void archInitEvent(char *archEventString)
{
	char CiphCRC[5]={0};
	int bytesNum = 0;
	bytesNum += mini_snprintf(archEventString + bytesNum,200-bytesNum,  "%s,%s,%d,%d,1,EVENT,", NAME, VERSION, GetSerialNumber(), GeneralSettings.DeviceID);
	calculateCRC(archEventString, strlen(archEventString), CiphCRC);
	bytesNum += mini_snprintf(archEventString + bytesNum, 200-bytesNum, (char *) CiphCRC);
	strcpy(archEventCRC1, CiphCRC);
	strcpy(archEventMeasCRC2, CiphCRC);
	bytesNum += mini_snprintf(archEventString + bytesNum, 200-bytesNum, "\n\nDATE, TIME, DST, EVENT CODE, CRC2\n");
}

static void archCreateNewDataArchive(void)
{
	FIL FilData={0};

	if (archivedDataAvailable())
	{
		SetDataArchiveFilePath();
		if (f_open(&FilData, archDataFilePath, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
		{
			archInitData(archBufferHeader);
			f_puts(archBufferHeader, &FilData);
			f_close(&FilData);
		}
	}
}

static void archCreateNewTotalizerArchive(void)
{
	FIL FilCount={0};
	if (archivedTotalizerAvailable())
	{
		SetTotalizerArchiveFilePath();
		if (f_open(&FilCount, archTotalizerFilePath, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
		{
			archInitTotalizer(archBufferHeader);
			f_puts(archBufferHeader, &FilCount);
			f_close(&FilCount);
		}
	}
}

static void archCreateNewEventArchive(void)
{
	char FilInitArchive[200]={0};
	FIL FilEvent={0};

	SetEventArchiveFilePath();
	if (f_open(&FilEvent, archEventFilePath, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
	{
		archInitEvent(FilInitArchive);
		f_printf(&FilEvent, FilInitArchive);
		f_close(&FilEvent);
	}
}

static void archCreateNewArchive(void)
{
	BLUE_LED_ON;
	uint8_t newFileID = 0;

	xQueueReset(xDataQueue);
	xQueueReset(xDataStatusQueue);
	xQueueReset(xDataResolution);
	xQueueReset(xTotalizerQueue);

	newFileID = archGetNewFileID();

	archSaveNewFileID(newFileID);

	archCreateNewDataArchive();
	archCreateNewTotalizerArchive();
	archCreateNewEventArchive();

	USERMESSAGE_RefreshArchiveID();

	BLUE_LED_OFF;
}

static void archStartDataArchive(void)
{
	char archFilRead[50]={0};
	FIL FilData={0};
	UINT bytesReadFromFile = 0;

	if (archivedDataAvailable())
	{
		SetDataArchiveFilePath();
		if (f_open(&FilData, archDataFilePath, FA_OPEN_EXISTING | FA_WRITE | FA_READ) == FR_OK)
		{
			f_gets(archFilRead, 50, &FilData);
			int initLen = strlen(archFilRead);
			strncpy(archDataCRC1, archFilRead + initLen - 5, 4);

			f_lseek(&FilData, f_size(&FilData) - 6);
			f_read(&FilData, archDataMeasCRC2, 4, &bytesReadFromFile);
			if (strcmp(archDataMeasCRC2, "CRC2") == 0)
				strcpy(archDataMeasCRC2, archDataCRC1);
			f_close(&FilData);
		}
		else
			archCreateNewDataArchive();
	}
}

static void archStartTotalizerArchive(void)
{
	char archFilRead[50]= {0};
	FIL FilCount={0};
	UINT bytesReadFromFile = 0;

	if (archivedTotalizerAvailable())
	{
		SetTotalizerArchiveFilePath();
		if (f_open(&FilCount, archTotalizerFilePath, FA_OPEN_EXISTING | FA_WRITE | FA_READ) == FR_OK)
		{
			f_gets(archFilRead, 50, &FilCount);
			int initLen = strlen(archFilRead);
			strncpy(archTotalizerCRC1, archFilRead + initLen - 5, 4);

			f_lseek(&FilCount, f_size(&FilCount) - 6);
			f_read(&FilCount, archTotalizerMeasCRC2, 4, &bytesReadFromFile);
			if (strcmp(archTotalizerMeasCRC2, "CRC2") == 0)
				strcpy(archTotalizerMeasCRC2, archTotalizerCRC1);
			f_close(&FilCount);
		}
		else
			archCreateNewTotalizerArchive();
	}
}

static void archStartEventArchive(void)
{
	char archFilRead[50]={0};
	FIL FilEvent= {0};
	UINT bytesReadFromFile = 0;

	SetEventArchiveFilePath();

	if (f_open(&FilEvent, archEventFilePath, FA_OPEN_EXISTING | FA_WRITE | FA_READ) == FR_OK)
	{
		f_gets(archFilRead, 50, &FilEvent);
		int initLen = strlen(archFilRead);
		strncpy(archEventCRC1, archFilRead + initLen - 5, 4);

		f_lseek(&FilEvent, f_size(&FilEvent) - 6);
		f_read(&FilEvent, archEventMeasCRC2, 4, &bytesReadFromFile);
		if (strcmp(archEventMeasCRC2, "CRC2") == 0)
			strcpy(archEventMeasCRC2, archEventCRC1);
		f_close(&FilEvent);
	}
	else
		archCreateNewEventArchive();

}

static void archStartArchive(void)
{
	FileID = FRAM_Read(CURRENT_ARCH_ADDR);
	if (FileID > 99)
		archSaveNewFileID(0);

	archStartDataArchive();
	archStartTotalizerArchive();
	archStartEventArchive();
}

static uint8_t SaveArchiveOnSDCard(void)
{
	if ( xSemaphoreTake( xSwitchSemaphore, (TickType_t) 100 ) == pdTRUE)
	{
		FREERTOS_StoreDataArchiveToSD();
		xSemaphoreGive(xSwitchSemaphore);
		return 1;
	}
	else
		return 0;
}

static void updateArchiveTimeAndDate(void)
{
	RTC_GetTimeAndDate(&RTCTimeArch, &RTCDateArch);
}

void ARCHIVE_SendEvent(char* eventDesc)
{
	char* eventMessage = pvPortMalloc(100);
	if (NULL == eventMessage)
	{

	}
	else
	{
		strncpy(eventMessage,eventDesc,100);
		if(pdPASS != xQueueSend(xMessageQueue, &eventMessage, 20))
			vPortFree(eventMessage);
	}
}

void ARCHIVE_SafeStop(void)
{
	SaveArchiveOnSDCard();
	if (TakeChannelsMutex(100))
	{
		FRAM_Write(ARCHIVE_STATE_ADDR, 0);
		archPollingState = 0;
		GiveChannelsMutex();
	}
}

void ARCHIVE_SafeStart(void)
{
	FRAM_Write(ARCHIVE_STATE_ADDR, 1);
	archPollingState = 1;
}

void ARCHIVE_CreateNewArchives(void)
{
	xEventGroupSetBits(xArchiveEventGroup, 0x02);
}

uint8_t ARCHIVE_GetFrequencyState(void)
{
	return ArchivizationFrequencyState;
}

void ARCHIVE_SetFrequencyState(uint8_t NewState)
{
	ArchivizationFrequencyState = NewState;
}

int ARCHIVE_GetState(void)
{
	return archPollingState;
}

void ARCHIVE_SaveBuffersToSDCard(void)
{
	if (ARCHIVE_GetState())
		SaveArchiveOnSDCard();
}

uint32_t ARCHIVE_TakeSemaphore(uint32_t timeout)
{
	if(xSemaphoreTake( xArchiveWrite, timeout) == pdTRUE)
		return 1;
	else
		return 0;
}

void ARCHIVE_GiveSemaphore(void)
{
	xSemaphoreGive(xArchiveWrite);
}

uint8_t ARCHIVE_IsCurrent(const char *archive)
{
	if(strcmp(archDataFilePath,archive))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t ARCHIVE_IsFileCurrentArchive(char * fileName)
{
	if(0 == strcmp(fileName,archDataFilePath))
		return 1;
	else if(0 == strcmp(fileName,archTotalizerFilePath))
		return 1;
	else if(0 == strcmp(fileName,archEventFilePath))
		return 1;
	else
		return 0;
}

void vtaskArchive(void *pvParameters)
{
	vTaskDelay(500);
	uint8_t ChannelsNumber={0};

	xSwitchSemaphore = xSemaphoreCreateMutex();
	xArchiveWrite = xSemaphoreCreateMutex();

	xArchiveEventGroup = xEventGroupCreate();

	xMessageQueue = xQueueCreate(20, sizeof(char *));

	xDataQueue = xQueueCreate(100, sizeof(float));
	xDataStatusQueue = xQueueCreate(100, sizeof(uint8_t));
	xDataResolution = xQueueCreate(100, sizeof(char));

	xTotalizerQueue = xQueueCreate(200, sizeof(uint64_t));
	xTotalizerResolution = xQueueCreate(200, sizeof(char));

	archPollingState = FRAM_Read(ARCHIVE_STATE_ADDR);
	if (archPollingState != 0 && archPollingState != 1)
		archPollingState = 0;

	if(1 == archPollingState && ArchiveSettings.ArchivizationFrequency1 < 5)
		archFirstRecordOnPowerUp = 1;
	else
		archFirstRecordOnPowerUp = 0;

	memset(archBufferArchive, 0, sizeof(archBufferArchive));
	memset(archBufferTotalizer, 0, sizeof(archBufferTotalizer));

	FILINFO filInfo;
	if(FR_NO_FILE == f_stat("0:archive", &filInfo))
	{
		f_mkdir("0:archive");
		archCreateNewArchive();
	}

	while (1)
	{
		if (STARTUP_WaitForBits(0x0010))
		{
			STARTUP_ClaerBits(0x0010);
			STARTUP_SetBits(0x0020);

			xQueueReceive(xChannelsQueue, &ChannelsNumber, 0);

			if (FRAM_Read(NEW_ARCHIVE_FLAG_ADDR) == 1)
			{
				archCreateNewArchive();
				FRAM_Write(NEW_ARCHIVE_FLAG_ADDR, 0);
			}

			archStartArchive();

			ARCHIVE_SendEvent("SYS:STOP");
			ARCHIVE_SendEvent("SYS:START");

			if (ChannelsNumber != 0)
			{
				while (1)
				{
					if (xEventGroupWaitBits(xArchiveEventGroup, 0x01 | 0x02, pdTRUE, pdFALSE, 0) != 0)
					{
						xQueueReset(xDataQueue);
						xQueueReset(xDataStatusQueue);
						xQueueReset(xDataResolution);
						xQueueReset(xTotalizerQueue);
						archCreateNewArchive();
					}

					if ( xSemaphoreTake( xSwitchSemaphore, ( TickType_t ) 0 ) == pdTRUE)
					{
						if (archPollingState != 0)
						{
							updateArchiveTimeAndDate();
							archDataPolling();
							archTotalizerPolling();
						}
						xSemaphoreGive(xSwitchSemaphore);
					}

					archEventPolling();
					WDFlags[5] = ALIVE;
					vTaskDelay(250);
				}
			}
			else
			{
				while (1)
				{
					archEventPolling();
					WDFlags[5] = ALIVE;
					vTaskDelay(250);
				}
			}
		}
		else
			continue;
	}
}

void CreateArchiveTask(void)
{
	xTaskCreate(vtaskArchive, "vtaskArchive", 10000, NULL, ( unsigned portBASE_TYPE )6, &vtaskArchiveHandle);
}

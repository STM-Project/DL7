#include "channels.h"
#include "ExpansionCards.h"
#include "NSMAC.h"
#include "parameters.h"
#include "passwords.h"
#include "crc.h"
#include "dtos.h"
#include <string.h>
#include "WindowManagement.h"
#include "UserChar.h"
#include "skins.h"
#include "PopUpMessageDLG.h"
#include "backlight.h"
#include "version.h"
#include "screenSaver.h"
#include "archive.h"
#include "fram.h"
#include "shift_reg.h"
#include "rtc.h"
#include "mini-printf.h"
#include "ComputeChannel.h"
#include "tcpipclient_netconn.h"
#include "smtp_netconn.h"

#include "fatfs.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

#define BUFFER_SIZE 	3356
#define CRC_SIZE			4

static uint8_t oldCharVer = 0;
static uint8_t oldCompVer = 0;
static uint8_t oldRS485andLang = 0;

SDRAM GENERAL_SETTINGS GeneralSettings;
SDRAM GENERAL_SETTINGS bkGeneralSettings;

SDRAM ARCHIVE_SETTINGS ArchiveSettings;
SDRAM ARCHIVE_SETTINGS bkArchiveSettings;

SDRAM USER_TABLES_SETTINGS UserTablesSettings;
SDRAM USER_TABLES_SETTINGS bkUserTablesSettings;

SDRAM ETH_SETTINGS bkEthSettings;
SDRAM COM_SETTINGS bkComSettings;

SDRAM USER_TREND UserTrendsSettings[6];
SDRAM USER_TREND bkUserTrendsSettings[6];

SDRAM char ParametersReadWriteBuffer[BUFFER_SIZE];
SDRAM char ChannelSourceDescryption[2];

SDRAM static BYTE bufferLang[32768];
SDRAM FIL LangFile;
SDRAM UINT bytesLang;

SDRAM uint8_t ListOfActiveChannels[100];

SDRAM uint8_t NewSettings;
SDRAM uint8_t RestartDevice;
SDRAM uint8_t NewArchive;
SDRAM uint8_t NewUserChar;

SDRAM USER_CHAR UserCharacteristcs[10];
SDRAM USER_CHAR bkUserCharacteristics[10];

static FIL ParametersFile;

void FillListOfActiveChannels(void)
{
	for (int i = 0; i < 100; i++)
	{
		if (Channels[i].source.type == 0)
			ListOfActiveChannels[i] = 0;
		else
			ListOfActiveChannels[i] = 1;
	}
}

uint8_t CheckChannelType(int ChannelIndex)
{
	return ListOfActiveChannels[ChannelIndex];
}

void CopyArchiveSettings(ARCHIVE_SETTINGS *Dst, ARCHIVE_SETTINGS *Src)
{
	Dst->ArchivizationFrequency1 = Src->ArchivizationFrequency1;
	Dst->ArchivizationFrequency2 = Src->ArchivizationFrequency2;
	Dst->TotalizerArchivizationFrequency = Src->TotalizerArchivizationFrequency;
	Dst->ArchiveMaxSize = Src->ArchiveMaxSize;
	Dst->canUserStopArchive = Src->canUserStopArchive;
}

void CopyGeneralSettings(GENERAL_SETTINGS *Dst, GENERAL_SETTINGS *Src)
{
	Dst->DeviceLanguage = Src->DeviceLanguage;
	strncpy(Dst->DeviceDescription, Src->DeviceDescription, 81);
	Dst->DeviceID = Src->DeviceID;

	Dst->Brightness = Src->Brightness;
	Dst->ScreenSaverBrightness = Src->ScreenSaverBrightness;
	Dst->ScreenSaverTime = Src->ScreenSaverTime;

	Dst->BeeperMode = Src->BeeperMode;
	Dst->AlarmBeeperMode = Src->AlarmBeeperMode;
	Dst->DSTMode = Src->DSTMode;

	RestartScreenSaver(Dst->Brightness, Dst->ScreenSaverTime, Dst->ScreenSaverBrightness);
}

void CopyEthSettings(ETH_SETTINGS *Dst, ETH_SETTINGS *Src)
{
	for (int i = 0; i < 4; i++)
	{
		Dst->IPAddress[i] = Src->IPAddress[i];
		Dst->IPGateway[i] = Src->IPGateway[i];
		Dst->NetMask[i] = Src->NetMask[i];
		Dst->primaryDNS[i] = Src->primaryDNS[i];
		Dst->secondaryDNS[i] = Src->secondaryDNS[i];
	}
	Dst->IPport = Src->IPport;
}

void CopyComSettings(COM_SETTINGS *Dst, COM_SETTINGS *Src)
{
	Dst->ComBaudrate = Src->ComBaudrate;
	Dst->ComParity = Src->ComParity;
	Dst->ComStopBits = Src->ComStopBits;
	Dst->MBAddress = Src->MBAddress;
}

void CopyUserTablesSettings(USER_TABLES_SETTINGS *Dst, USER_TABLES_SETTINGS *Src)
{
	for (int i = 0; i < 6; i++)
	{
		strncpy(Dst->TablesTitles[i], Src->TablesTitles[i], 41);
		for (int j = 0; j < 15; j++)
			Dst->TablesData[15 * i + j] = Src->TablesData[15 * i + j];
	}
}

void CopyUserTrendsSettings(USER_TREND *Dst, USER_TREND *Src)
{
	strncpy(Dst->description, Src->description, 48);
	Dst->max = Src->max;
	Dst->min = Src->min;
	Dst->isActive = 0;
	for (int j = 0; j < 6; ++j)
	{
		Dst->channelID[j] = Src->channelID[j];
		if (-1 < Dst->channelID[j])
		{
			Dst->pChannel[j] = &Channels[Dst->channelID[j]];
			Dst->isActive = 1;
		}
		else
			Dst->pChannel[j] = NULL;
	}
}

void CopyUserCharSettings(USER_CHAR *Dst, USER_CHAR *Src)
{
	for (int i=0; i<10; i++)
	{
		strncpy((Dst+i)->name, (Src+i)->name, 61);
		(Dst+i)->numberOfPoints = (Src+i)->numberOfPoints;
		for (int j = 0; j< (Src+i)->numberOfPoints; j++)
		{
			(Dst+i)->pointsX[j] = (Src+i)->pointsX[j];
			(Dst+i)->pointsY[j] = (Src+i)->pointsY[j];
		}
	}
}

int SetFileParametersCRC(const TCHAR* path)
{
	FILINFO fno;
	FRESULT fresult;
	UINT bytesr = 0;
	char newCRC[5] =
	{ '0', '0', '0', '0', '\0' };
	uint16_t pParametersCRCWord = 0xFFFF;

	fresult = f_open(&ParametersFile, path, FA_READ | FA_WRITE);
	f_stat(path,&fno);

	if (fresult != FR_OK)
		return 1;
	else if (fno.fsize == 0)
	{
		f_close(&ParametersFile);
		return 2;
	}
	else
	{
		while (ParametersFile.fptr != fno.fsize)
		{
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
			f_read(&ParametersFile, ParametersReadWriteBuffer, BUFFER_SIZE, &bytesr);
			generateCRC(ParametersReadWriteBuffer, bytesr, &pParametersCRCWord);
		}

		hashCRC(newCRC, &pParametersCRCWord);
		f_write(&ParametersFile, newCRC, 4, &bytesr);
		memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
		f_close(&ParametersFile);
		return 0;
	}
}

int ChcekParametersFile(const TCHAR* path)
{
	FILINFO fno;
	FRESULT fresult;
	UINT bytesr = 0;
	UINT allbytesread = 0;
	WORD pParametersCRCWord = 0xFFFF;

	char computedParametersCRC[5] =
	{ '0', '0', '0', '0', '\0' };
	char readParametersCRC[5] =
	{ '0', '0', '0', '0', '\0' };

	memset(ParametersReadWriteBuffer, 0, BUFFER_SIZE);

	fresult = f_open(&ParametersFile, path, FA_READ | FA_WRITE);
	f_stat(path,&fno);

	if (fresult != FR_OK)
		return 1;
	else if (fno.fsize == 0 || fno.fsize >= 10485760)
	{
		f_close(&ParametersFile);
		return 2;
	}
	else
	{
		UINT fileSizeWithoutCRC = fno.fsize-CRC_SIZE;
		while (ParametersFile.fptr<fileSizeWithoutCRC)
		{
			memset(ParametersReadWriteBuffer, 0, bytesr);
			f_read(&ParametersFile, ParametersReadWriteBuffer, BUFFER_SIZE, &bytesr);
			allbytesread += bytesr;
			if (ParametersFile.fptr<fileSizeWithoutCRC)
				generateCRC(ParametersReadWriteBuffer, bytesr, &pParametersCRCWord);
			else
			{
				bytesr -= (ParametersFile.fptr - fileSizeWithoutCRC);
				generateCRC(ParametersReadWriteBuffer, bytesr, &pParametersCRCWord);
			}
		}
		hashCRC(computedParametersCRC, &pParametersCRCWord);

		f_lseek(&ParametersFile, fileSizeWithoutCRC);
		f_read(&ParametersFile, readParametersCRC, 5, &bytesr);
		f_close(&ParametersFile);

		if (strcmp(computedParametersCRC, readParametersCRC))
			return 2;
		else
			return 0;
	}
}

void ReadExpansionCardsParameters(EXPANSION_CARD *Card, char *readpointer)
{
	Card->settings.IOcard.generalSet = 0;

	switch (Card->type)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
	case SPI_CARD_IN6V:
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		for (int j = 0; j < 6; j++)
		{
			Card->settings.IOcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].adjusment = atof(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].compensationConstantValue = 0.0;
			Card->settings.IOcard.channels[j].compChannel = -1;
		}
		break;
	case SPI_CARD_IN4SG:
		Card->settings.IOcard.generalSet = atoi(strtok_r(NULL, ",", &readpointer));
		for (int j = 0; j < 6; j++)
		{
			Card->settings.IOcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].adjusment = atof(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].compensationConstantValue = 0.0;
			Card->settings.IOcard.channels[j].compChannel = -1;
		}
		break;
	case SPI_CARD_OUT6RL:
		for (int j = 0; j < 6; j++)
		{
			Card->settings.IOcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].adjusment = 0.0;
			Card->settings.IOcard.channels[j].compensationConstantValue = 0.0;
			Card->settings.IOcard.channels[j].compChannel = -1;
		}
		break;
	case SPI_CARD_IN6TC:
		for (int j = 0; j < 6; j++)
		{
			Card->settings.IOcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].adjusment = atof(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].compChannel = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].compensationConstantValue = atof(strtok_r(NULL, ",", &readpointer));
		}
		Card->settings.IOcard.channels[6].adjusment = 0.0;
		Card->settings.IOcard.channels[6].compChannel = 0.0;
		Card->settings.IOcard.channels[6].compensationConstantValue = -1;
		Card->settings.IOcard.channels[7].adjusment = 0.0;
		Card->settings.IOcard.channels[7].compChannel = 0.0;
		Card->settings.IOcard.channels[7].compensationConstantValue = -1;
		break;
	case SPI_CARD_IN3:
		for (int j = 0; j < 3; j++)
		{
			Card->settings.IOcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].adjusment = atof(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].compChannel = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].compensationConstantValue = atof(strtok_r(NULL, ",", &readpointer));
		}
		Card->settings.IOcard.channels[3].adjusment = 0.0;
		Card->settings.IOcard.channels[3].compChannel = 0.0;
		Card->settings.IOcard.channels[3].compensationConstantValue = -1;
		Card->settings.IOcard.channels[4].adjusment = 0.0;
		Card->settings.IOcard.channels[4].compChannel = 0.0;
		Card->settings.IOcard.channels[4].compensationConstantValue = -1;
		break;
	case SPI_CARD_IN6D:
		Card->settings.IOcard.generalSet = atoi(strtok_r(NULL, ",", &readpointer));
		for (int j = 0; j < 6; j++)
		{
			Card->settings.IOcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.IOcard.channels[j].adjusment = 0.0;
			Card->settings.IOcard.channels[j].compChannel = 0.0;
			Card->settings.IOcard.channels[j].compensationConstantValue = -1;
		}
		break;
	case SPI_CARD_2RS485:
	case SPI_CARD_2RS485I24:
		for (int j = 0; j < 2; j++)
		{
			Card->settings.RScard.port[j].baudrate = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.RScard.port[j].parity = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.RScard.port[j].stopBits = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.RScard.port[j].timeout = atoi(strtok_r(NULL, ",", &readpointer));
		}

		for (int j = 0; j < 25; j++)
		{
			Card->settings.RScard.channels[j].port = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.RScard.channels[j].deviceAdr = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.RScard.channels[j].registerAdr = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.RScard.channels[j].registerType = atoi(strtok_r(NULL, ",", &readpointer));
		}
		break;
	case SPI_CARD_OUT3:
		for (int j = 0; j < 3; j++)
		{
			Card->settings.OUTcard.channels[j].type = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.OUTcard.channels[j].source = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.OUTcard.channels[j].lowScaleValue = atof(strtok_r(NULL, ",", &readpointer));
			Card->settings.OUTcard.channels[j].highScaleValue = atof(strtok_r(NULL, ",", &readpointer));
			Card->settings.OUTcard.channels[j].failureMode = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.OUTcard.channels[j].failureValue = atof(strtok_r(NULL, ",", &readpointer));
		}
		break;
	case SPI_CARD_1HRT:
		Card->settings.HARTcard.isSecondary = atoi(strtok_r(NULL, ",", &readpointer));
		Card->settings.HARTcard.isResistor = atoi(strtok_r(NULL, ",", &readpointer));
		Card->settings.HARTcard.preambleLenght = atoi(strtok_r(NULL, ",", &readpointer));

		for (int j = 0; j < MAX_HART_SENSOR_FOR_CARD; j++)
		{
			Card->settings.HARTcard.sensors[j].activity = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.HARTcard.sensors[j].address = strtoull(strtok_r(NULL, ",", &readpointer), NULL, 16);

		}
		for (int j = 0; j < 25; j++)
		{
			Card->settings.HARTcard.readChannel[j].sensorIdx = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.HARTcard.readChannel[j].valueType = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.HARTcard.readChannel[j].commandType = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.HARTcard.readChannel[j].valueAddr = atoi(strtok_r(NULL, ",", &readpointer));
			Card->settings.HARTcard.readChannel[j].statusMode = atoi(strtok_r(NULL, ",", &readpointer));
		}
		break;
	default:
		break;
	}
}

static void AddFloatParameterToString(char * string, float parameter)
{
	char buf[20];
	strcat(string, ",");
	float2stri(buf, parameter, 4);
	strcat(string, buf);
}

static void AddFloatParameterToStringWithResolution(char * string, float parameter, uint8_t resolution)
{
	char buf[20];
	strcat(string, ",");
	float2stri(buf, parameter, resolution);
	strcat(string, buf);
}

static void AddIntParameterToString(char * string, int parameter)
{
	char buf[10];
	strcat(string, ",");
	itoa(parameter, buf, 10);
	strcat(string, buf);
}

static void HART_ConvertAddressToString(uint64_t addr, char *buf)
{
	uint8_t bufHex[10];

	bufHex[0]=(addr>>(4*8))&0xff;
	bufHex[1]=(addr>>(3*8))&0xff;
	bufHex[2]=(addr>>(2*8))&0xff;
	bufHex[3]=(addr>>(1*8))&0xff;
	bufHex[4]=(addr>>(0*8))&0xff;
	mini_snprintf(buf,11,"%02x%02x%02x%02x%02x", bufHex[0], bufHex[1], bufHex[2], bufHex[3], bufHex[4]);
}

static void AddHARTAddressToString(char * string, uint64_t address)
{
	char buf[20];
	strcat(string, ",");
	HART_ConvertAddressToString(address,buf);
	strcat(string, buf);
}

static void WriteExpansionCardParameters(uint8_t cardType, const EXPANSION_CARD_SETTINGS *settings, char *buf)
{
	switch (cardType)
	{
	case SPI_CARD_IN6I24:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN6I24);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN6I:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN6I);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN6RTD:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN6RTD);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN3RTD:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN3RTD);
		for (int i = 0; i < 3; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
		}
		strcat(buf, ",0,0.0,0,0.0,0,0.0\n");
		break;
	case SPI_CARD_OUT6RL:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_OUT6RL);
		for (int i = 0; i < 6; ++i)
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN6TC:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN6TC);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
			AddIntParameterToString(buf, settings->IOcard.channels[i].compChannel);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].compensationConstantValue);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN4SG:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN4SG);
		AddIntParameterToString(buf, settings->IOcard.generalSet);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN6V:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN6V);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_IN3:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN3);
		for (int i = 0; i < 3; ++i)
		{
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].adjusment);
			AddIntParameterToString(buf, settings->IOcard.channels[i].compChannel);
			AddFloatParameterToString(buf, settings->IOcard.channels[i].compensationConstantValue);
		}
		strcat(buf, ",0,0.0,0,0.0,0,0.0,0,0.0,0,0.0,0,0.0\n");
		break;
	case SPI_CARD_IN6D:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_IN6D);
		AddIntParameterToString(buf, settings->IOcard.generalSet);
		for (int i = 0; i < 6; ++i)
			AddIntParameterToString(buf, settings->IOcard.channels[i].type);
		strcat(buf, "\n");
		break;
	case SPI_CARD_2RS485:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_2RS485);
		for (int i = 0; i < 2; ++i)
		{
			AddIntParameterToString(buf, settings->RScard.port[i].baudrate);
			AddIntParameterToString(buf, settings->RScard.port[i].parity);
			AddIntParameterToString(buf, settings->RScard.port[i].stopBits);
			AddIntParameterToString(buf, settings->RScard.port[i].timeout);
		}
		for (int i = 0; i < 25; ++i)
		{
			AddIntParameterToString(buf, settings->RScard.channels[i].port);
			AddIntParameterToString(buf, settings->RScard.channels[i].deviceAdr);
			AddIntParameterToString(buf, settings->RScard.channels[i].registerAdr);
			AddIntParameterToString(buf, settings->RScard.channels[i].registerType);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_2RS485I24:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_2RS485I24);
		for (int i = 0; i < 2; ++i)
		{
			AddIntParameterToString(buf, settings->RScard.port[i].baudrate);
			AddIntParameterToString(buf, settings->RScard.port[i].parity);
			AddIntParameterToString(buf, settings->RScard.port[i].stopBits);
			AddIntParameterToString(buf, settings->RScard.port[i].timeout);
		}
		for (int i = 0; i < 25; ++i)
		{
			AddIntParameterToString(buf, settings->RScard.channels[i].port);
			AddIntParameterToString(buf, settings->RScard.channels[i].deviceAdr);
			AddIntParameterToString(buf, settings->RScard.channels[i].registerAdr);
			AddIntParameterToString(buf, settings->RScard.channels[i].registerType);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_OUT3:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_OUT3);
		for (int i = 0; i < 3; ++i)
		{
			AddIntParameterToString(buf, settings->OUTcard.channels[i].type);
			AddIntParameterToString(buf, settings->OUTcard.channels[i].source);
			AddFloatParameterToString(buf, settings->OUTcard.channels[i].lowScaleValue);
			AddFloatParameterToString(buf, settings->OUTcard.channels[i].highScaleValue);
			AddIntParameterToString(buf, settings->OUTcard.channels[i].failureMode);
			AddFloatParameterToString(buf, settings->OUTcard.channels[i].failureValue);
		}
		strcat(buf, "\n");
		break;
	case SPI_CARD_1HRT:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_1HRT);

			AddIntParameterToString(buf, settings->HARTcard.isSecondary);
			AddIntParameterToString(buf, settings->HARTcard.isResistor);
			AddIntParameterToString(buf, settings->HARTcard.preambleLenght);

			for (int i = 0; i < MAX_HART_SENSOR_FOR_CARD; i++)
			{
				AddIntParameterToString(buf, settings->HARTcard.sensors[i].activity);
				AddHARTAddressToString(buf, settings->HARTcard.sensors[i].address);

			}
			for (int j = 0; j < 25; j++)
			{
				AddIntParameterToString(buf, settings->HARTcard.readChannel[j].sensorIdx);
				AddIntParameterToString(buf, settings->HARTcard.readChannel[j].valueType);
				AddIntParameterToString(buf, settings->HARTcard.readChannel[j].commandType);
				AddIntParameterToString(buf, settings->HARTcard.readChannel[j].valueAddr);
				AddIntParameterToString(buf, settings->HARTcard.readChannel[j].statusMode);
			}

		strcat(buf, "\n");
		break;
	case SPI_CARD_PSBATT:
		mini_snprintf(buf, BUFFER_SIZE, "%d", SPI_CARD_PSBATT);
		strcat(buf, "\n");
		break;
	default:
		strcpy(buf, "0\n");
		break;
	}
}

static void SetDefaultFRAMSettings(void)
{
	uint64_t FRAMResetBuffer = 0x0000000000000000;
	uint32_t ResetBuffer = 0xFFFFFFFF;

	RTC_TimeTypeDef Temp_Time;
	RTC_DateTypeDef Temp_Date;
	RTC_GetTimeAndDate(&Temp_Time, &Temp_Date);

	FRAM_Write(WINDOWS_STATE_ADDR, INFO);
	FRAM_Write(WINDOW_INSTANCE_ADDR, 0);
	FRAM_Write(AUTO_CHANGE_ADDR, 0);
	FRAM_Write(USER_TABLE_SCREEN_ADDR, 0);
	FRAM_Write(USER_TREND_SCREEN_ADDR, 0);
	FRAM_Write(ARCHIVE_STATE_ADDR, 0);
	FRAM_Write(CURRENT_ARCH_ADDR, 0);
	FRAM_Write(SKIN_ADDR, 0);
	FRAM_Write(NO_OF_ACTIVE_CHANNELS_ADDR, 0);

	FRAM_Write(NEW_PARAMETERS_ADDR, 1);
	FRAM_Write(NEW_ARCHIVE_FLAG_ADDR, 1);

	for (int i = 0; i < 100; i++)
	{
		FRAM_Write((START_RESET_DATE_ADDR + 5 * i), Temp_Date.Date);
		FRAM_Write((START_RESET_DATE_ADDR + 1 + 5 * i), Temp_Date.Month);
		FRAM_Write((START_RESET_DATE_ADDR + 2 + 5 * i), Temp_Date.Year);
		FRAM_Write((START_RESET_DATE_ADDR + 3 + 5 * i), Temp_Time.Hours);
		FRAM_Write((START_RESET_DATE_ADDR + 4 + 5 * i), Temp_Time.Minutes);
		FRAM_WriteMultiple(START_COUNTER_ADDR + 16 * i, (uint8_t*) &FRAMResetBuffer, 8);
		FRAM_WriteMultiple(START_COUNTER_ADDR + 16 * i + 8, (uint8_t*) &FRAMResetBuffer, 8);
	}

	for (int i = 0; i < 100; i++)
		FRAM_WriteMultiple(START_MAXIMUM_ADDR + 4 * i, (uint8_t*) &ResetBuffer, 4);

	for (int i = 0; i < 100; i++)
		FRAM_WriteMultiple(START_MINIMUM_ADDR + 4 * i, (uint8_t*) &ResetBuffer, 4);
}

static void WriteParametersHeaderToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "%s,%s,%08d\n", NAME, VERSION, GetSerialNumber());
	f_puts(buffer, parametersFile);
}

static void WriteGeneralSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "%d,%s,%d,%d,%d,%d,%d,%d,%d\n", bkGeneralSettings.DeviceID,
			bkGeneralSettings.DeviceDescription, bkGeneralSettings.DeviceLanguage, bkGeneralSettings.Brightness,
			bkGeneralSettings.ScreenSaverBrightness, bkGeneralSettings.ScreenSaverTime, bkGeneralSettings.BeeperMode,
			bkGeneralSettings.AlarmBeeperMode, bkGeneralSettings.DSTMode);
	f_puts(buffer, parametersFile);
}

static void WriteArchiveSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "%d,%d,%d,%d,%d\n", bkArchiveSettings.ArchivizationFrequency1,
			bkArchiveSettings.ArchivizationFrequency2, bkArchiveSettings.TotalizerArchivizationFrequency,
			bkArchiveSettings.ArchiveMaxSize, bkArchiveSettings.canUserStopArchive);
	f_puts(buffer, parametersFile);
}

static void WriteEthSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "%d.%d.%d.%d,"
			"%d,"
			"%d.%d.%d.%d,"
			"%d.%d.%d.%d,"
			"%d.%d.%d.%d,"
			"%d.%d.%d.%d\n", bkEthSettings.IPAddress[0],bkEthSettings.IPAddress[1], bkEthSettings.IPAddress[2], bkEthSettings.IPAddress[3],
			bkEthSettings.IPport,
			bkEthSettings.NetMask[0],bkEthSettings.NetMask[1], bkEthSettings.NetMask[2], bkEthSettings.NetMask[3],
			bkEthSettings.IPGateway[0],bkEthSettings.IPGateway[1], bkEthSettings.IPGateway[2], bkEthSettings.IPGateway[3],
			bkEthSettings.primaryDNS[0],bkEthSettings.primaryDNS[1], bkEthSettings.primaryDNS[2], bkEthSettings.primaryDNS[3],
			bkEthSettings.secondaryDNS[0],bkEthSettings.secondaryDNS[1], bkEthSettings.secondaryDNS[2], bkEthSettings.secondaryDNS[3]);
	f_puts(buffer, parametersFile);

}

static void WriteComSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "%d,%d,%d,%d,500\n", bkComSettings.ComBaudrate, bkComSettings.ComParity,
			bkComSettings.ComStopBits, bkComSettings.MBAddress);
	f_puts(buffer, parametersFile);
}

static void WriteUserTablesSettingsToFile(FIL* parametersFile, char *buffer)
{
	char temp_value[10];
	for (int i = 0; i < 6; i++)
	{
		strcpy(buffer, bkUserTablesSettings.TablesTitles[i]);
		for (int j = 0; j < 15; j++)
		{
			strcat(buffer, ",");
			mini_snprintf(temp_value, 10, "%d", bkUserTablesSettings.TablesData[15 * i + j]);
			strcat(buffer, temp_value);
		}
		strcat(buffer, "\n");
		f_puts(buffer, parametersFile);
	}
}

static void WriteUserTrendsSettingsToFile(FIL* parametersFile, char *buffer)
{
	char tempValue[21];
	for (int i = 0; i < 6; i++)
	{
		strcpy(buffer, bkUserTrendsSettings[i].description);
		strcat(buffer, ",");
		float2stri(tempValue, bkUserTrendsSettings[i].max, 4);
		strcat(buffer, tempValue);
		strcat(buffer, ",");
		float2stri(tempValue, bkUserTrendsSettings[i].min, 4);
		strcat(buffer, tempValue);

		for (int j = 0; j < 6; j++)
		{
			strcat(buffer, ",");
			mini_snprintf(tempValue, 10, "%d", bkUserTrendsSettings[i].channelID[j]);
			strcat(buffer, tempValue);
		}
		strcat(buffer, "\n");
		f_puts(buffer, parametersFile);
	}
}

static void WriteEmptyLineToFile(FIL* parametersFile)
{
	f_puts("\n", parametersFile);
}

static void WriteExpansionCardsSettingsToFile(FIL* parametersFile, char *buffer)
{
	for (int i = 0; i < 7; i++)
	{
		WriteExpansionCardParameters(bkExpansionCards[i].type, &bkExpansionCards[i].settings, buffer);
		f_puts(ParametersReadWriteBuffer, parametersFile);
	}
}

static void WriteSingleChannelParameters(const CHANNEL *channel, int ChannelNo, uint8_t *NumberOfActiveChannels, char *buf)
{

	switch (channel->source.type)
	{
	case 1: //kanał pomiarowy
		mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE, "%d,%c%d,%d", ChannelNo + 1, channel->source.board + 'A',
				channel->source.number + 1, channel->CharacteristicType);
		(*NumberOfActiveChannels)++;
		break;
	case 2: //kanał obliczeniowy
		mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE, "%d,CO,%d", ChannelNo + 1, channel->CharacteristicType);
		(*NumberOfActiveChannels)++;
		break;
	case 3: //demo
		mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE, "%d,R%d,%d", ChannelNo + 1,channel->source.number + 1, channel->CharacteristicType);
		(*NumberOfActiveChannels)++;
		break;
	case 4: //demo
		mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE, "%d,DE,%d", ChannelNo + 1, channel->CharacteristicType);
		(*NumberOfActiveChannels)++;
		break;
	default: //kanał wyłaczony albo błedna wartosc
		mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE, "%d,--,%d", ChannelNo + 1, channel->CharacteristicType);
		break;
	}
	AddFloatParameterToStringWithResolution(ParametersReadWriteBuffer, channel->LowSignalValue,6);
	AddFloatParameterToStringWithResolution(ParametersReadWriteBuffer, channel->LowScaleValue,6);
	AddFloatParameterToStringWithResolution(ParametersReadWriteBuffer, channel->HighSignalValue,6);
	AddFloatParameterToStringWithResolution(ParametersReadWriteBuffer, channel->HighScaleValue,6);
	strcat(ParametersReadWriteBuffer, ",");
	strcat(ParametersReadWriteBuffer, channel->description);
	strcat(ParametersReadWriteBuffer, ",");
	strcat(ParametersReadWriteBuffer, channel->unit);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->resolution);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->graph_max_value);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->graph_min_value);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->ToArchive);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->failureMode);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->failureValue);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->filterType);

	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot1.type);
	strcat(ParametersReadWriteBuffer, ",");
	strcat(ParametersReadWriteBuffer, channel->Tot1.unit);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot1.resolution);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot1.period);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot1.multipler);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot1.ToArchive);

	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot2.type);
	strcat(ParametersReadWriteBuffer, ",");
	strcat(ParametersReadWriteBuffer, channel->Tot2.unit);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot2.resolution);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot2.period);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot2.multipler);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->Tot2.ToArchive);

	char buffer[5];
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[0].type);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[0].mode);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->alarm[0].level);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->alarm[0].hysteresis);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[0].color);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[0].output.type);

	mini_snprintf(buffer, 4, ",%c%d", channel->alarm[0].output.board + 'A', channel->alarm[0].output.number + 1);
	strcat(ParametersReadWriteBuffer, buffer);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[0].logEvent);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[0].ChangeArchivizationFrequency);

	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[1].type);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[1].mode);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->alarm[1].level);
	AddFloatParameterToString(ParametersReadWriteBuffer, channel->alarm[1].hysteresis);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[1].color);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[1].output.type);

	mini_snprintf(buffer, 4, ",%c%d", channel->alarm[1].output.board + 'A', channel->alarm[1].output.number + 1);
	strcat(ParametersReadWriteBuffer, buffer);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[1].logEvent);
	AddIntParameterToString(ParametersReadWriteBuffer, channel->alarm[1].ChangeArchivizationFrequency);

	AddIntParameterToString(ParametersReadWriteBuffer,  channel->UserCharacteristicType);
	strcat(ParametersReadWriteBuffer, ",");
	strcat(ParametersReadWriteBuffer, channel->formula.userFormula);

	AddIntParameterToString(ParametersReadWriteBuffer,  channel->Tot1.sendEmail);
	AddIntParameterToString(ParametersReadWriteBuffer,  channel->Tot2.sendEmail);
	AddIntParameterToString(ParametersReadWriteBuffer,  channel->alarm[0].sendEmail);
	AddIntParameterToString(ParametersReadWriteBuffer,  channel->alarm[1].sendEmail);

	strcat(ParametersReadWriteBuffer, "\n");

}

static void WriteChannelsHeaderToFile(FIL* parametersFile, char *buffer)
{
	strcpy(buffer,
			"CHANNEL NO,INPUT NO,CHARACTERISTIC TYPE,LOW SIGNAL VALUE,LOW SCALE VALUE,HIGH SIGNAL VALUE,HIGH SCALE VALUE,"
					"DESCRIPTION,UNITS,RESOLUTION,TREND MAX,TREND MIN,ARCHIVE,FAILURE MODE,FAILURE VALUE,FILTER,"
					"TOTALIZER 1 TYPE,TOTALIZER 1 UNIT,TOTALIZER 1 RESOLUTION,TOTALIZER 1 PERIOD,TOTALIZER 1 MULTIPLER,TOTALIZER 1 ARCHIVE,"
					"TOTALIZER 2 TYPE,TOTALIZER 2 UNIT,TOTALIZER 2 RESOLUTION,TOTALIZER 2 PERIOD,TOTALIZER 2 MULTIPLER,TOTALIZER 2 ARCHIVE,"
					"ALARM 1 TYPE,ALARM 1 MODE,ALARM 1 LEVEL,ALARM 1 HIST,ALARM 1 COLOR,ALARM 1 OUTPUT, ALARM 1 OUTPUT NO,ALARM 1 LOG, ALARM 1 ARCH.,"
					"ALARM 2 TYPE,ALARM 2 MODE,ALARM 2 LEVEL,ALARM 2 HIST,ALARM 2 COLOR,ALARM 2 OUTPUT, ALARM 2 OUTPUT NO,ALARM 2 LOG, ALARM 2 ARCH.,"
					"USER CHARCTERISTIC,USER FORMULA,TOTALIZER 1 EMAIL,TOTALIZER 2 EMAIL,ALARM 1 EMAIL, ALARM 2 EMAIL\n");
	f_puts(buffer, parametersFile);
}

static void WriteChannelsSettingsToFile(FIL* parametersFile, char *buffer,uint8_t *numberOfActiveChannels)
{
	for (int i = 0; i < 100; ++i)
	{
		WriteSingleChannelParameters(&bkChannels[i], i, numberOfActiveChannels, buffer);
		f_puts(buffer, parametersFile);
	}
}

static void WriteUserCharSettingToFile(FIL* parametersFile, char *buffer)
{
	for(int i=0; i<10; i++) {
		mini_snprintf(buffer, BUFFER_SIZE, "%d,%s,%d", i, bkUserCharacteristics[i].name, bkUserCharacteristics[i].numberOfPoints);
		for (int j=0; j<bkUserCharacteristics[i].numberOfPoints; j++) {
			AddFloatParameterToStringWithResolution(buffer, bkUserCharacteristics[i].pointsX[j], 5);
			AddFloatParameterToStringWithResolution(buffer, bkUserCharacteristics[i].pointsY[j], 5);
		}
		strcat(buffer, "\n");
		f_puts(buffer, parametersFile);
	}
}

static void WriteModbusTCPServersSettingToFile(FIL* parametersFile, char *buffer)
{
	int n=mini_snprintf(buffer, BUFFER_SIZE, "%d,%d.%d.%d.%d,%d,%d,%d",bkMBserver[0].mode,
			bkMBserver[0].ip[0],bkMBserver[0].ip[1],bkMBserver[0].ip[2],bkMBserver[0].ip[3],bkMBserver[0].port,
			bkMBserver[0].responseTimeout,bkMBserver[0].frequency);

	for(int i=1; i<MAX_MODBUS_TCP_CONNECTIONS; i++)
	{
		n+=mini_snprintf(buffer+n, BUFFER_SIZE, ",%d,%d.%d.%d.%d,%d,%d,%d",bkMBserver[i].mode,
				bkMBserver[i].ip[0],bkMBserver[i].ip[1],bkMBserver[i].ip[2],bkMBserver[i].ip[3],bkMBserver[i].port,
				bkMBserver[i].responseTimeout,bkMBserver[i].frequency);
	}
	strcat(buffer, "\n");
	f_puts(buffer, parametersFile);
}

static void WriteModbusTCPRegistersSettingToFile(FIL* parametersFile, char *buffer)
{
	int n=mini_snprintf(buffer, BUFFER_SIZE, "%d,%d,%d,%d",bkModbusTCPregisters[0].connectionID,
			bkModbusTCPregisters[0].deviceAddress,bkModbusTCPregisters[0].number,bkModbusTCPregisters[0].registerType);

	for(int i=1; i<MAX_MODBUS_TCP_REGISTERS; i++)
	{
		n+=mini_snprintf(buffer+n, BUFFER_SIZE, ",%d,%d,%d,%d",bkModbusTCPregisters[i].connectionID,
				bkModbusTCPregisters[i].deviceAddress,bkModbusTCPregisters[i].number,bkModbusTCPregisters[i].registerType);
	}
	strcat(buffer, "\n");
	f_puts(buffer, parametersFile);
}

static void WriteEmailSettingsToFile(FIL* parametersFile, char *buffer)
{
	unsigned char codedEmailPassword[48] = {0};
	EMAIL_EncodePasswordForSettingsFile(codedEmailPassword, bkEmailSettings.client.password);
	mini_snprintf(buffer, BUFFER_SIZE, "%s,%s,%s,%d,%s,%d,%d,%d,%d", bkEmailSettings.client.email,bkEmailSettings.client.login,(char*)codedEmailPassword,
			bkEmailSettings.client.useSSL, bkEmailSettings.server.name,bkEmailSettings.server.port,
			bkEmailSettings.event.mode, bkEmailSettings.event.day, bkEmailSettings.event.hour);
	for (int i=0;i<NUMBER_OF_EMAIL_RECIPIENTS;++i)
	{
		strcat(buffer, ",");
		strcat(buffer, &bkEmailSettings.client.mailList[i][0]);
	}
	strcat(buffer, "\n");
	f_puts(buffer, parametersFile);
}

void saveParameters(void)
{
	uint8_t NumberOfActiveChannels = 0;
	char parameterFilePath[18];
	mini_snprintf(parameterFilePath,18,"0:config/%s.par",NAME);
	if (FR_OK == f_open(&ParametersFile, parameterFilePath, FA_CREATE_ALWAYS | FA_WRITE))
	{
		WriteParametersHeaderToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteGeneralSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteArchiveSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteEthSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteComSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteUserTablesSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteUserTrendsSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteEmptyLineToFile(&ParametersFile);
		WriteExpansionCardsSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteEmptyLineToFile(&ParametersFile);
		WriteChannelsHeaderToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteChannelsSettingsToFile(&ParametersFile, ParametersReadWriteBuffer, &NumberOfActiveChannels);
		WriteEmptyLineToFile(&ParametersFile);
		WriteUserCharSettingToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteEmptyLineToFile(&ParametersFile);
		WriteModbusTCPServersSettingToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteModbusTCPRegistersSettingToFile(&ParametersFile, ParametersReadWriteBuffer);
		WriteEmptyLineToFile(&ParametersFile);
		WriteEmailSettingsToFile(&ParametersFile, ParametersReadWriteBuffer);
		f_close(&ParametersFile);

		SetFileParametersCRC(parameterFilePath);

		FRAM_Write(NO_OF_ACTIVE_CHANNELS_ADDR, NumberOfActiveChannels);
		HAL_Delay(500);
		NumberOfActiveChannels = 0;
		FRAM_Write(NEW_PARAMETERS_ADDR, 1);
	}
}

static void WriteDefaultGeneralSettingsToFile(FIL* parametersFile, char *buffer, uint8_t deviceLanguage)
{
	mini_snprintf(buffer, BUFFER_SIZE, "1, ,%d,100,1,5,1,1,1\n",deviceLanguage);
	f_puts(buffer, parametersFile);
}

static void WriteDefaultArchiveSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "0,0,0,0,1\n");
	f_puts(buffer, parametersFile);
}

static void WriteDefaultEthSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "192.168.0.10,502,255.255.255.0,192.168.0.1,8.8.8.8,8.8.4.4\n");
	f_puts(buffer, parametersFile);
}

static void WriteDefaultComSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, "3,2,1,1,0\n");
	f_puts(buffer, parametersFile);
}

static void WriteDefaultUserTablesSettingsToFile(FIL* parametersFile, char *buffer, const char *tableTitle)
{
	for (int i = 1; i < 7; ++i)
	{
		mini_snprintf(buffer, BUFFER_SIZE, "%s %d,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1\n", tableTitle, i);
		f_puts(buffer, parametersFile);
	}
}

static void WriteDefaultUserTrendsSettingsToFile(FIL* parametersFile, char *buffer, const char *trendTitle)
{
	for (int i = 1; i < 7; ++i)
	{
		mini_snprintf(buffer, BUFFER_SIZE, "%s %d,100.0000,0.0000,-1,-1,-1,-1,-1,-1\n", trendTitle, i);
		f_puts(buffer, parametersFile);
	}
}

static void WriteDefaultExpansionCardParameters(FIL* parametersFile, char *buffer, uint8_t cardType)
{
	switch (cardType)
	{
	case SPI_CARD_IN6I24:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN6I24);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer,0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN6I:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN6I);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN6RTD:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN6RTD);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN3RTD:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN3RTD);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_OUT6RL:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_OUT6RL);
		for (int i = 0; i < 6; ++i)
			AddIntParameterToString(buffer, 0);
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN6TC:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN6TC);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN6V:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN6V);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN4SG:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN4SG);
		AddIntParameterToString(buffer, 0);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN3:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN3);
		for (int i = 0; i < 6; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_IN6D:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_IN6D);
		AddIntParameterToString(buffer, 0);
		for (int i = 0; i < 6; ++i)
			AddIntParameterToString(buffer, 0);
		strcat(buffer, "\n");
		break;
	case SPI_CARD_2RS485:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_2RS485);
		for (int i = 0; i < 2; ++i)
		{
			AddIntParameterToString(buffer, 5);
			AddIntParameterToString(buffer, 2);
			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 2000);
		}
		for (int i = 0; i < 25; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 1);
			AddIntParameterToString(buffer, 300000);
			AddIntParameterToString(buffer, 0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_2RS485I24:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_2RS485I24);
		for (int i = 0; i < 2; ++i)
		{
			AddIntParameterToString(buffer, 5);
			AddIntParameterToString(buffer, 2);
			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 2000);
		}
		for (int i = 0; i < 25; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 1);
			AddIntParameterToString(buffer, 300000);
			AddIntParameterToString(buffer, 0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_OUT3:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_OUT3);
		for (int i = 0; i < 3; ++i)
		{
			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
			AddFloatParameterToString(buffer, 0.0);
			AddIntParameterToString(buffer, 0);
			AddFloatParameterToString(buffer, 0.0);
		}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_1HRT:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_1HRT);

			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 0);
			AddIntParameterToString(buffer, 5);

			for (int i = 0; i < MAX_HART_SENSOR_FOR_CARD; i++)
			{
				AddIntParameterToString(buffer, 0);
				AddIntParameterToString(buffer, 0);
				AddHARTAddressToString(buffer, 0);
			}
			for (int i = 0; i < 25; i++)
			{
				AddIntParameterToString(buffer, 0);
				AddIntParameterToString(buffer, 0);
				AddIntParameterToString(buffer, 0);
				AddIntParameterToString(buffer, 0);
				AddIntParameterToString(buffer, 0);
			}
		strcat(buffer, "\n");
		break;
	case SPI_CARD_PSBATT:
		mini_snprintf(buffer, BUFFER_SIZE, "%d", SPI_CARD_PSBATT);
		strcat(buffer, "\n");
		break;
	default:
		strcpy(buffer, "0\n");
		break;
	}

	f_puts(buffer, parametersFile);
}

static void WriteDefaultChannelsSettingsToFile(FIL* parametersFile, char *buffer, const char* channelDescryption)
{
	for (int i = 1; i <= 100; ++i)
	{
		mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE,
				"%d,--,0,0.0000,0.0000,100.0000,100.0000,%s %d, ,0,100.0000,0.0000,0,0,0.0000,0,0, ,0,0,1,0,0, ,0,0,1,0,0,0,0.0000,0.0000,0,0,A1,0,0,0,0,0.0000,0.0000,0,0,A1,0,0,0,0,0,0,0,0\n",
				i, channelDescryption, i);
		f_puts(buffer, parametersFile);
	}
}

static void WriteDefaultUserCharSettingToFile(FIL* parametersFile, char *buffer,const char *charDescryption)
{
	for (int i = 1; i <= 10; i++)
	{
		mini_snprintf(buffer, BUFFER_SIZE, "%d,%s %d,2,0.00000,0.00000,1.00000,1.00000\n", i - 1,charDescryption, i);
		f_puts(buffer, parametersFile);
	}
}

static void WriteDefaultModbusTCPServersSettingToFile(FIL* parametersFile, char *buffer)
{
	int n = mini_snprintf(buffer, BUFFER_SIZE, "0,192.168.0.10,502,5000,2");
	for (int i = 1; i < MAX_MODBUS_TCP_CONNECTIONS; ++i)
		n += mini_snprintf(buffer + n, BUFFER_SIZE - n, ",0,192.168.0.10,502,5000,2");
	strncat(buffer, "\n", 2);
	f_puts(buffer, parametersFile);
}

static void WriteDefaultModbusTCPRegistersSettingToFile(FIL* parametersFile, char *buffer)
{
	int n = mini_snprintf(buffer, BUFFER_SIZE, "0,1,300000,0");
	for (int i = 1; i < MAX_MODBUS_TCP_REGISTERS; i++)
		n += mini_snprintf(buffer + n, BUFFER_SIZE - n, ",0,1,300000,0");
	strncat(buffer, "\n", 2);
	f_puts(buffer, parametersFile);
}

static void WriteDefaultEmailSettingsToFile(FIL* parametersFile, char *buffer)
{
	mini_snprintf(buffer, BUFFER_SIZE, " , ,JE?@,0,smtp.server.com,587,0,0,0, , , , , \n");
	f_puts(buffer, parametersFile);
}

static void RestoreDefaultParameters(void)
{
	char parameterFilePath[18];
	mini_snprintf(parameterFilePath, 18, "0:config/%s.par", NAME);
	if (FR_OK == f_open(&ParametersFile, parameterFilePath, FA_CREATE_ALWAYS | FA_WRITE))
	{
		memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
		WriteParametersHeaderToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultGeneralSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, bkGeneralSettings.DeviceLanguage);
		WriteDefaultArchiveSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultEthSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultComSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultUserTablesSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, GUI_LANG_GetText(63));
		WriteDefaultUserTrendsSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, GUI_LANG_GetText(64));
		WriteEmptyLineToFile(&ParametersFile);
		for (int i = 0; i < NUMBER_OF_CARD_SLOTS; ++i)
			WriteDefaultExpansionCardParameters(&ParametersFile,ParametersReadWriteBuffer, ExpansionCards[i].type);
		WriteEmptyLineToFile(&ParametersFile);
		WriteChannelsHeaderToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultChannelsSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, GUI_LANG_GetText(6));
		WriteEmptyLineToFile(&ParametersFile);
		WriteDefaultUserCharSettingToFile(&ParametersFile,ParametersReadWriteBuffer,GUI_LANG_GetText(32));
		WriteEmptyLineToFile(&ParametersFile);
		WriteDefaultModbusTCPServersSettingToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultModbusTCPRegistersSettingToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteEmptyLineToFile(&ParametersFile);
		WriteDefaultEmailSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);

		f_close(&ParametersFile);
		SetFileParametersCRC(parameterFilePath);

		FRAM_Write(NO_OF_ACTIVE_CHANNELS_ADDR, 0);
		HAL_Delay(500);
		FRAM_Write(NEW_PARAMETERS_ADDR, 1);
	}
}

void RestoreDefaultSettings(void)
{
	RestoreDefaultParameters();
	SetDefaultPasswords(PASSWORDS_GetCurrentLevel());
	SetDefaultFRAMSettings();
}

static void GetGeneralSettingsFromBuffer(char *buffer, GENERAL_SETTINGS *Dst)
{
	char *readpointer = NULL;
	Dst->DeviceID = atoi(strtok_r(buffer, ",", &readpointer));
	strncpy(Dst->DeviceDescription, strtok_r(NULL, ",", &readpointer), 81);
	Dst->DeviceLanguage = atoi(strtok_r(NULL, ",", &readpointer));
	if(oldRS485andLang && Dst->DeviceLanguage>=4)
		Dst->DeviceLanguage +=1;
	Dst->Brightness = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->ScreenSaverBrightness = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->ScreenSaverTime = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->BeeperMode = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->AlarmBeeperMode = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->DSTMode = atoi(strtok_r(NULL, ",", &readpointer));
}

static void GetArchiveSettingsFromBuffer(char *buffer, ARCHIVE_SETTINGS *Dst)
{
	char *readpointer = NULL;
	Dst->ArchivizationFrequency1 = atoi(strtok_r(buffer, ",", &readpointer));
	Dst->ArchivizationFrequency2 = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->TotalizerArchivizationFrequency = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->ArchiveMaxSize = atoi(strtok_r(NULL, ",", &readpointer));
	char *tempBuf = strtok_r(NULL, ",\n", &readpointer);
	if (tempBuf == NULL)
		Dst->canUserStopArchive = 1;
	else
		Dst->canUserStopArchive = atoi(tempBuf);
}

static void GetEthSettingsFromBuffer(char *buffer, ETH_SETTINGS *Dst)
{
	char *readpointer = NULL;
	Dst->IPAddress[0] = atoi(strtok_r(buffer, ".", &readpointer));
	Dst->IPAddress[1] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->IPAddress[2] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->IPAddress[3] = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->IPport = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->NetMask[0] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->NetMask[1] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->NetMask[2] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->NetMask[3] = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->IPGateway[0] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->IPGateway[1] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->IPGateway[2] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->IPGateway[3] = atoi(strtok_r(NULL, ",", &readpointer));

	char *tempBuf = strtok_r(NULL, ".", &readpointer);
	if (tempBuf == NULL)
	{
		Dst->primaryDNS[0] = 8;
		Dst->primaryDNS[1] = 8;
		Dst->primaryDNS[2] = 8;
		Dst->primaryDNS[3] = 8;

		Dst->secondaryDNS[0] = 8;
		Dst->secondaryDNS[1] = 8;
		Dst->secondaryDNS[2] = 4;
		Dst->secondaryDNS[3] = 4;
	}
	else
	{
		Dst->primaryDNS[0] = atoi(tempBuf);
		Dst->primaryDNS[1] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->primaryDNS[2] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->primaryDNS[3] = atoi(strtok_r(NULL, ",", &readpointer));

		Dst->secondaryDNS[0] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->secondaryDNS[1] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->secondaryDNS[2] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->secondaryDNS[3] = atoi(strtok_r(NULL, ",", &readpointer));
	}
}

static void GetComSettingsFromBuffer(char *buffer, COM_SETTINGS *Dst)
{
	char *readpointer = NULL;
	Dst->ComBaudrate = atoi(strtok_r(buffer, ",", &readpointer));
	if(oldRS485andLang)
	{
		if(Dst->ComBaudrate == 0)
			Dst->ComBaudrate = 3;
		else
			Dst->ComBaudrate -= 1;
	}

	Dst->ComParity = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->ComStopBits = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->MBAddress = atoi(strtok_r(NULL, ",", &readpointer));
}

static void GetChannelSettingsFromBuffer(char *buffer, CHANNEL *Dst)
{
	char *readpointer = NULL;
	char ChannelSourceDescryption[5] = " ";

	char formulaBuffer[FORMULA_RPN_SIZE];
	char messageBuffer[100];

	uint8_t selection = atoi(strtok_r(buffer, ",", &readpointer));

	strncpy(ChannelSourceDescryption, strtok_r(NULL, ",", &readpointer), 5);
	if (ChannelSourceDescryption[0] == '-' && ChannelSourceDescryption[1] == '-')
	{
		Dst->source.type = 0;
		Dst->source.board = 0;
		Dst->source.number = 0;
	}
	else if (ChannelSourceDescryption[0] == 'C' && ChannelSourceDescryption[1] == 'O')
	{
		Dst->source.type = 2;
		Dst->source.board = 0;
		Dst->source.number = 0;
	}
	else if (ChannelSourceDescryption[0] == 'D' && ChannelSourceDescryption[1] == 'E')
	{
		Dst->source.type = 4;
		Dst->source.board = 0;
		Dst->source.number = 0;
	}

	else if (ChannelSourceDescryption[0] == 'R') // remote
	{
		Dst->source.type = 3;
		Dst->source.board = 0;
		Dst->source.number = atoi(&ChannelSourceDescryption[1])-1;
	}
	else if (ChannelSourceDescryption[0] >= 'A' && ChannelSourceDescryption[0] <= 'G')
	{
		Dst->source.type = 1;
		Dst->source.board = ChannelSourceDescryption[0] - 'A';
		Dst->source.number = atoi(&ChannelSourceDescryption[1])-1;
	}
	else
	{
		Dst->source.type = 0;
		Dst->source.board = 0;
		Dst->source.number = 0;
	}

	if(oldCompVer && Dst->source.type >= 2)
		Dst->source.type++;

	Dst->CharacteristicType = atoi(strtok_r(NULL, ",", &readpointer));
	if(oldCharVer && Dst->CharacteristicType >= 1)
		Dst->CharacteristicType++;

	Dst->LowSignalValue = atof(strtok_r(NULL, ",", &readpointer));
	Dst->LowScaleValue = atof(strtok_r(NULL, ",", &readpointer));
	Dst->HighSignalValue = atof(strtok_r(NULL, ",", &readpointer));
	Dst->HighScaleValue = atof(strtok_r(NULL, ",", &readpointer));

	strncpy(Dst->description, strtok_r(NULL, ",", &readpointer), 40);

	strncpy(Dst->unit, strtok_r(NULL, ",", &readpointer), UNIT_SIZE);
	Dst->resolution = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->graph_max_value = atof(strtok_r(NULL, ",", &readpointer));
	Dst->graph_min_value = atof(strtok_r(NULL, ",", &readpointer));
	Dst->ToArchive = atoi(strtok_r(NULL, ",", &readpointer));

	Dst->failureMode = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->failureValue = atof(strtok_r(NULL, ",", &readpointer));
	Dst->filterType = atoi(strtok_r(NULL, ",", &readpointer));

	Dst->Tot1.type = atoi(strtok_r(NULL, ",", &readpointer));
	if (0 == Dst->Tot1.type)
		Dst->Tot1.value = 0;
	strncpy(Dst->Tot1.unit, strtok_r(NULL, ",", &readpointer), UNIT_SIZE);
	Dst->Tot1.resolution = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->Tot1.period = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->Tot1.multipler = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->Tot1.ToArchive = atoi(strtok_r(NULL, ",", &readpointer));

	Dst->Tot2.type = atoi(strtok_r(NULL, ",", &readpointer));
	if (0 == Dst->Tot2.type)
		Dst->Tot2.value = 0;
	strncpy(Dst->Tot2.unit, strtok_r(NULL, ",", &readpointer), UNIT_SIZE);
	Dst->Tot2.resolution = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->Tot2.period = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->Tot2.multipler = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->Tot2.ToArchive = atoi(strtok_r(NULL, ",", &readpointer));

	Dst->alarm[0].type = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[0].mode = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[0].level = atof(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[0].hysteresis = atof(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[0].color = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[0].output.type = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	strncpy(ChannelSourceDescryption, strtok_r(NULL, ",", &readpointer), 2);
	Dst->alarm[0].output.board = ChannelSourceDescryption[0] - 'A';
	Dst->alarm[0].output.number = ChannelSourceDescryption[1] - '1';
	Dst->alarm[0].logEvent = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[0].ChangeArchivizationFrequency = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));

	Dst->alarm[1].type = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[1].mode = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[1].level = atof(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[1].hysteresis = atof(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[1].color = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[1].output.type = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	strncpy(ChannelSourceDescryption, strtok_r(NULL, ",", &readpointer), 2);
	Dst->alarm[1].output.board = ChannelSourceDescryption[0] - 'A';
	Dst->alarm[1].output.number = ChannelSourceDescryption[1] - '1';
	Dst->alarm[1].logEvent = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));
	Dst->alarm[1].ChangeArchivizationFrequency = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));

	Dst->UserCharacteristicType = (uint8_t) atoi(strtok_r(NULL, ",", &readpointer));

	memset(&Dst->formula.RPNFormula, 0, FORMULA_RPN_SIZE);
	memset(&Dst->formula.userFormula, 0, FORMULA_USER_SIZE);

	if (oldCompVer) {
		strcpy(Dst->formula.userFormula, "0");
	}
	else {
		strcpy(Dst->formula.userFormula, strtok_r(NULL,",\n", &readpointer));
		if(Dst->source.type==2) {
			convertSpecialCase(formulaBuffer, Dst->formula.userFormula);
			if(StriToRPN(Dst->formula.RPNFormula, formulaBuffer)) {
				Dst->source.type = 0;
				mini_snprintf(messageBuffer, 100, "%s %d.\n%s.",GUI_LANG_GetText(195), selection,GUI_LANG_GetText(196));
				CreateMessage(messageBuffer, GUI_YELLOW, GUI_BLACK);
			}
		}
	}

	char *tempBuf = strtok_r(NULL, ",", &readpointer);
	if (tempBuf == NULL)
	{
		Dst->Tot1.sendEmail = 0;
		Dst->Tot2.sendEmail = 0;

		Dst->alarm[0].sendEmail = 0;
		Dst->alarm[1].sendEmail = 0;
	}
	else
	{
		Dst->Tot1.sendEmail = atoi(tempBuf);
		Dst->Tot2.sendEmail = atoi(strtok_r(NULL, ",", &readpointer));

		Dst->alarm[0].sendEmail = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->alarm[1].sendEmail = atoi(strtok_r(NULL, ",", &readpointer));
	}
}

static void GetUserCharSettingsFromBuffer(char *buffer, USER_CHAR *Dst)
{
	char *readpointer = NULL;

	strtok_r(buffer, ",", &readpointer);
	strncpy(Dst->name, strtok_r(NULL, ",", &readpointer), 61);
	Dst->numberOfPoints = atoi(strtok_r(NULL, ",", &readpointer));

	for(int j=0; j<100; j++)
	{
		Dst->pointsX[j] = atoff(strtok_r(NULL, ",", &readpointer));
		Dst->pointsY[j] = atoff(strtok_r(NULL, ",", &readpointer));
	}
}

static void GetTrendSettingsFromBuffer(char *buffer, USER_TREND *Dst)
{
	char *readpointer = NULL;
	strncpy(Dst->description, strtok_r(buffer, ",", &readpointer), 41);
	Dst->max = atof(strtok_r(NULL, ",", &readpointer));
	Dst->min = atof(strtok_r(NULL, ",", &readpointer));
	for (int j = 0; j < 6; ++j)
		Dst->channelID[j] = atoi(strtok_r(NULL, ",", &readpointer));
}

static void GetTableSettingsFromBuffer(char *buffer, USER_TABLES_SETTINGS *Dst, int TableNo)
{
	char *readpointer = NULL;
	strncpy(Dst->TablesTitles[TableNo], strtok_r(buffer, ",", &readpointer), 41);
	for (int j = 0; j < 15; j++)
		Dst->TablesData[15 * TableNo + j] = atoi(strtok_r(NULL, ",", &readpointer));
}

static void GetModbusServersSettingsFromBuffer(char *buffer, MODBUS_TCP_SERVER *Dst)
{
	char *readpointer = NULL;

	Dst->connectionID = 1;
	Dst->mode = atoi(strtok_r(buffer, ",", &readpointer));
	Dst->ip[0] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->ip[1] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->ip[2] = atoi(strtok_r(NULL, ".", &readpointer));
	Dst->ip[3] = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->port = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->responseTimeout = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->frequency = atoi(strtok_r(NULL, ",", &readpointer));
	Dst++;

	for(int i=1;i<MAX_MODBUS_TCP_CONNECTIONS;i++,Dst++)
	{
		Dst->connectionID = i+1;
		Dst->mode = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->ip[0] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->ip[1] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->ip[2] = atoi(strtok_r(NULL, ".", &readpointer));
		Dst->ip[3] = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->port = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->responseTimeout = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->frequency = atoi(strtok_r(NULL, ",", &readpointer));
	}
}

static void GetModbusRegistersSettingsFromBuffer(char *buffer, MODBUS_TCP_REGISTER *Dst)
{
	char *readpointer = NULL;

	Dst->connectionID = atoi(strtok_r(buffer, ",", &readpointer));
	Dst->deviceAddress = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->number = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->registerType = atoi(strtok_r(NULL, ",", &readpointer));
	Dst++;

	for(int i=1;i<MAX_MODBUS_TCP_REGISTERS;i++,Dst++)
	{
		Dst->connectionID = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->deviceAddress = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->number = atoi(strtok_r(NULL, ",", &readpointer));
		Dst->registerType = atoi(strtok_r(NULL, ",", &readpointer));
	}
}

static void GetEmailSettingsFromBuffer(char *buffer, EMAIL_SETTINGS *Dst)
{
	unsigned char codedEmailPassword[48]={0};
	char *readpointer = NULL;

	memset(Dst->client.password,0,32);

	strncpy(Dst->client.email, strtok_r(buffer, ",", &readpointer), 64);
	strncpy(Dst->client.login, strtok_r(NULL, ",", &readpointer), 64);
	strncpy((char *)codedEmailPassword, strtok_r(NULL, ",", &readpointer), 48);
	EMAIL_DecodePasswordFromSettingsFile(Dst->client.password, codedEmailPassword);
	Dst->client.useSSL = atoi(strtok_r(NULL, ",", &readpointer));

	strncpy(Dst->server.name, strtok_r(NULL, ",", &readpointer), 64);
	Dst->server.IP.addr = 0;
	Dst->server.port = atoi(strtok_r(NULL, ",", &readpointer));

	Dst->event.mode = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->event.day = atoi(strtok_r(NULL, ",", &readpointer));
	Dst->event.hour = atoi(strtok_r(NULL, ",", &readpointer));

	for(int i=0;i<NUMBER_OF_EMAIL_RECIPIENTS-1;++i)
		strncpy(&Dst->client.mailList[i][0], strtok_r(NULL, ",", &readpointer), 64);
	strncpy(&Dst->client.mailList[4][0], strtok_r(NULL, "\n", &readpointer), 64);
}

static void SetDefaultParameters(void)
{
	char parameterFilePath[18];
	mini_snprintf(parameterFilePath, 18, "0:config/%s.par", NAME);
	if (FR_OK == f_open(&ParametersFile, parameterFilePath, FA_CREATE_ALWAYS | FA_WRITE))
	{
		memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
		WriteParametersHeaderToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultGeneralSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, 0);
		WriteDefaultArchiveSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultEthSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultComSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultUserTablesSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, "Table");
		WriteDefaultUserTrendsSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, "Graph");
		WriteEmptyLineToFile(&ParametersFile);
		for (int i = 0; i < NUMBER_OF_CARD_SLOTS; ++i)
			WriteDefaultExpansionCardParameters(&ParametersFile,ParametersReadWriteBuffer, 0);
		WriteEmptyLineToFile(&ParametersFile);
		WriteChannelsHeaderToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultChannelsSettingsToFile(&ParametersFile,ParametersReadWriteBuffer, "Channel");
		WriteEmptyLineToFile(&ParametersFile);
		WriteDefaultUserCharSettingToFile(&ParametersFile,ParametersReadWriteBuffer,"Characteristic");
		WriteEmptyLineToFile(&ParametersFile);
		WriteDefaultModbusTCPServersSettingToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteDefaultModbusTCPRegistersSettingToFile(&ParametersFile,ParametersReadWriteBuffer);
		WriteEmptyLineToFile(&ParametersFile);
		WriteDefaultEmailSettingsToFile(&ParametersFile,ParametersReadWriteBuffer);

		f_close(&ParametersFile);
		SetFileParametersCRC(parameterFilePath);

		FRAM_Write(WINDOWS_STATE_ADDR, INFO);
		FRAM_Write(WINDOW_INSTANCE_ADDR, 0);
		FRAM_Write(AUTO_CHANGE_ADDR, 0);
		FRAM_Write(USER_TABLE_SCREEN_ADDR, 0);
		FRAM_Write(USER_TREND_SCREEN_ADDR, 0);
		FRAM_Write(ARCHIVE_STATE_ADDR, 0);
		FRAM_Write(CURRENT_ARCH_ADDR, 0);
		FRAM_Write(SKIN_ADDR, 0);
		FRAM_Write(NO_OF_ACTIVE_CHANNELS_ADDR, 0);
	}
}

int loadParameters(const TCHAR* path)
{
	char *readpointer;
	FRESULT fresult;
	xChannelsQueue = xQueueCreate(1, sizeof(uint8_t));
	uint8_t ActiveChannelsNumber = 0;
	int parametersFileStatus = 99;

	char version[12];

	oldCharVer = 0;
	oldCompVer = 0;
	uint8_t oldModbusTCPclient = 0;
	uint8_t oldEmailClient = 0;


	parametersFileStatus = ChcekParametersFile(path);

	if (parametersFileStatus != 0)
		return parametersFileStatus;
	else
	{
		fresult = f_open(&ParametersFile, path, FA_READ);
		if (fresult != FR_OK)
			return 1;
		else
		{
			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			if (strncmp(ParametersReadWriteBuffer, NAME, 3) != 0)
			{
				f_close(&ParametersFile);
				return 2;
			}

			strtok_r(ParametersReadWriteBuffer, ",", &readpointer);
			strcpy(version, strtok_r(NULL, ",", &readpointer));
			int major = atoi(strtok_r(version, ".", &readpointer));
			int minor = atoi(strtok_r(NULL, ".", &readpointer));
			if (major<2 && minor<2)
				oldCharVer = 1;
			if (major<2 && minor<3)
				oldCompVer = 1;
			if (major<2 && minor<4)
				oldModbusTCPclient = 1;
			if (major<2 && minor<5)
				oldRS485andLang = 1;
			if (major<2 && minor<6)
				oldEmailClient = 1;

			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			GetGeneralSettingsFromBuffer(ParametersReadWriteBuffer, &GeneralSettings);
			CopyGeneralSettings(&bkGeneralSettings, &GeneralSettings);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			GetArchiveSettingsFromBuffer(ParametersReadWriteBuffer, &ArchiveSettings);
			CopyArchiveSettings(&bkArchiveSettings, &ArchiveSettings);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);

			GetEthSettingsFromBuffer(ParametersReadWriteBuffer, &EthSettings);
			CopyEthSettings(&bkEthSettings, &EthSettings);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			GetComSettingsFromBuffer(ParametersReadWriteBuffer, &ComSettings);
			CopyComSettings(&bkComSettings, &ComSettings);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			for (int i = 0; i < 6; i++)
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				GetTableSettingsFromBuffer(ParametersReadWriteBuffer, &UserTablesSettings, i);
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
			}
			CopyUserTablesSettings(&bkUserTablesSettings, &UserTablesSettings);

			for (int i = 0; i < 6; i++)
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				GetTrendSettingsFromBuffer(ParametersReadWriteBuffer, &UserTrendsSettings[i]);
				UserTrendsSettings[i].isActive = 0;
				for(int j = 0; j < 6; ++j)
				{
					if(-1 < UserTrendsSettings[i].channelID[j])
					{
						UserTrendsSettings[i].pChannel[j] = &Channels[UserTrendsSettings[i].channelID[j]];
						UserTrendsSettings[i].isActive = 1;
					}
					else
						UserTrendsSettings[i].pChannel[j] = NULL;
				}

				CopyUserTrendsSettings(&bkUserTrendsSettings[i], &UserTrendsSettings[i]);
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
			}

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			for (int i = 0; i < 7; i++)
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				ExpansionCards[i].type = atoi(strtok_r(ParametersReadWriteBuffer, ",", &readpointer));
				ReadExpansionCardsParameters(&ExpansionCards[i], readpointer);
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
			}

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
			memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

			for (int i = 0; i < 100; i++)
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				GetChannelSettingsFromBuffer(ParametersReadWriteBuffer, &Channels[i]);
				if (Channels[i].source.type != 0)
					ActiveChannelsNumber++;
				bkChannels[i] = Channels[i];
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
			}

			if(oldCharVer)
			{
				for (int i=0; i <10; i++)
				{
					mini_snprintf(ParametersReadWriteBuffer, BUFFER_SIZE, "User Char %d\0", i+1);
					strncpy(UserCharacteristcs[i].name, ParametersReadWriteBuffer, 61);
					UserCharacteristcs[i].numberOfPoints = 2;
					UserCharacteristcs[i].pointsX[0] = 0.00000;
					UserCharacteristcs[i].pointsY[0] = 0.00000;
					UserCharacteristcs[i].pointsX[1] = 1.00000;
					UserCharacteristcs[i].pointsY[1] = 1.00000;
					memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
					bkUserCharacteristics[i] = UserCharacteristcs[i];
				}
			}
			else
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
				for (int i = 0; i<10;  i++)
				{
					f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
					GetUserCharSettingsFromBuffer(ParametersReadWriteBuffer, &UserCharacteristcs[i]);
					bkUserCharacteristics[i] = UserCharacteristcs[i];
					memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
				}
			}

			if(oldModbusTCPclient)
			{
				MBTCP_InitDefaultServers();
				MBTCP_InitDefaultRegisters();
			}
			else
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);

				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				GetModbusServersSettingsFromBuffer(ParametersReadWriteBuffer, MBserver);
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				GetModbusRegistersSettingsFromBuffer(ParametersReadWriteBuffer, ModbusTCPregisters);
			}

			if(oldEmailClient)
			{
				EMAIL_InitDefuaultParameters();
			}
			else
			{
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				memset(&ParametersReadWriteBuffer[0], 0, BUFFER_SIZE);
				f_gets(ParametersReadWriteBuffer, BUFFER_SIZE, &ParametersFile);
				GetEmailSettingsFromBuffer(ParametersReadWriteBuffer, &emailSettings);

				CopyEmailClientSettings(&bkEmailSettings.client,&emailSettings.client);
				CopyEmailServerSettings(&bkEmailSettings.server,&emailSettings.server);
				CopyEmailEventSettings(&bkEmailSettings.event,&emailSettings.event);
			}

			f_close(&ParametersFile);

			FillListOfActiveChannels();

			xQueueSend(xChannelsQueue, (void * ) &ActiveChannelsNumber, 0);
			return 0;
		}
	}
}

static void LoadLanguageFile(void)
{
	FILINFO fno;
	f_open(&LangFile, "0:lang/lang.csv", FA_READ);
	f_stat("0:lang/lang.csv",&fno);
	f_read(&LangFile, bufferLang, fno.fsize, &bytesLang);
	f_close(&LangFile);
	GUI_LANG_LoadCSV(bufferLang, bytesLang);
}

void SetLanguage(uint8_t newLang)
{
	GUI_LANG_SetLang(newLang);
}

void InitLanguage(void)
{
	LoadLanguageFile();
	SetLanguage(GeneralSettings.DeviceLanguage);
	SKINS_UpdateCalendar();
}

void HandleInternalDiskErrorState(void)
{
	SKINS_SetDarkSkin();
	CreateMessage("INTERNAL DISK ERROR", GUI_RED, GUI_BLACK);
	GUI_Exec();
	BACKLIGHT_Set(100);
	BLUE_LED_OFF;
	RED_LED_ON;
	while (1)
	{
	}
}

void LoadParametersFile(void)
{
	FILINFO fno;

	char parameterFilePath[18];
	char bkParameterFilePath[20];
	mini_snprintf(parameterFilePath,18,"0:config/%s.par",NAME);
	mini_snprintf(bkParameterFilePath,20,"0:config/bk%s.par",NAME);

	int res = loadParameters(parameterFilePath);

	switch (res)
	{
	case 0:
		if (FRAM_Read(NEW_PARAMETERS_ADDR) == 1)
		{
			FATFS_CopyFile(parameterFilePath, bkParameterFilePath);
			FRAM_Write(NEW_PARAMETERS_ADDR, 0);
		}
		break;
	case 1:
		res = loadParameters(bkParameterFilePath);
		if (res)
		{
			res = f_stat("0:config", &fno);
			if (res != FR_OK)
				res = f_mkdir("0:config");

			if (res == FR_OK)
			{
				SetDefaultParameters();
				loadParameters(parameterFilePath);
				if (res == 0)
				{
					FATFS_CopyFile(parameterFilePath, bkParameterFilePath);
					CreateMessage("DEFAULT PARAMETERS", GUI_YELLOW, GUI_BLACK);
				}
				else
					HAL_NVIC_SystemReset();
			}
			else
				HandleInternalDiskErrorState();
		}
		else
		{
			CreateMessage("BACKUP FILE LOADED. PARAMETER FILE DOES NOT EXIST", GUI_YELLOW, GUI_BLACK);
			FATFS_CopyFile(bkParameterFilePath, parameterFilePath);
		}

		break;
	case 2:
		res = loadParameters(bkParameterFilePath);
		if (res)
		{
			SetDefaultParameters();
			res = loadParameters(parameterFilePath);
			if (res == FR_OK)
			{
				FATFS_CopyFile(parameterFilePath, bkParameterFilePath);
				CreateMessage("DEFAULT PARAMETERS", GUI_YELLOW, GUI_BLACK);
			}
			else
				HandleInternalDiskErrorState();
		}
		else
		{
			CreateMessage("BACKUP FILE LOADED. PARAMETER FILE DOES NOT EXIST", GUI_YELLOW, GUI_BLACK);
			FATFS_CopyFile(bkParameterFilePath, parameterFilePath);
		}
		break;
	}
}

void InitializeModules(void)
{
	ScreenSaverInit(GeneralSettings.Brightness, GeneralSettings.ScreenSaverTime, GeneralSettings.ScreenSaverBrightness);
}

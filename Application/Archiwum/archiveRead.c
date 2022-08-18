/*
 * archiveRead.c
 *
 *  Created on: 22.04.2021
 *      Author: TomaszSok
 */

#include "archiveRead.h"
#include "fram.h"
#include "archive.h"
#include "ff.h"
#include "FreeRTOS.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

#define NUMBER_OF_CHANNELS 100
#define BUFFER_SIZE 	2232

ARCHIVE_FILE *ArchiveFile = NULL;
SDRAM ARCHIVE_CHANNEL_HEADER *channelHeader[100];
SDRAM GUI_POINT archiveGraphPoints[6][NUMBER_OF_ARCHIVE_POINTS];
int numberOfRecords = 0;
short trendOffsetY[6]={0};
int8_t selectedChannelsID[6]={0};

static char *readBuffer = NULL;

static SDRAM char* tempValues[100];

static uint8_t ARCHIVE_READ_DecodeStatus(char* valueString)
{
	if((*valueString == '-') && *(valueString+1) == '-')
	{
		switch(*(valueString+2))
		{
		case '-':
			return 0;
		case 'W':
			return 0x0A;
		case '|':
			return 0x02;
		case 'E':
			if('-' == *(valueString+3))
				return 0x03;
			else
				return 0xFF;
			break;
		case 'R':
			return 0x04;
		default:
			return 0xFF;
		}
	}
	else
		return 1;
}

static int ARCHIVE_READ_TakeSemaphore(void)
{
	if(ArchiveFile->isCurrentArchive)
	{
		if(ARCHIVE_TakeSemaphore(500))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 1;
	}
}

static void ARCHIVE_READ_GiveSemaphore(void)
{
	if(ArchiveFile->isCurrentArchive)
	{
		ARCHIVE_GiveSemaphore();
	}
}

void ARCHIVE_READ_ResetSelectedChannels(void)
{
	for(int i=0;i < 6; ++i)
	{
			selectedChannelsID[i] = 0;
	}
	FRAM_WriteMultiple(READ_ARCHIVE_SELECTION, (uint8_t*) selectedChannelsID, 6);

}

void ARCHIVE_READ_Initialize(void)
{
	FRAM_ReadMultiple(READ_ARCHIVE_SELECTION, (uint8_t*) selectedChannelsID, 6);
	for(int i=0;i < 6; ++i)
	{
		if(NUMBER_OF_CHANNELS < selectedChannelsID[i] ||  0 > selectedChannelsID[i] )
			selectedChannelsID[i] = 0;
	}

	if( ArchiveFile == NULL)
	{
		ArchiveFile = pvPortMalloc(sizeof(ARCHIVE_FILE));
	}
	else
	{
		vPortFree(ArchiveFile);
		ArchiveFile = NULL;
		ArchiveFile = pvPortMalloc(sizeof(ARCHIVE_FILE));
	}

	if( readBuffer == NULL)
	{
		readBuffer = pvPortMalloc(BUFFER_SIZE);
	}
	else
	{
		vPortFree(readBuffer);
		readBuffer = NULL;
		readBuffer = pvPortMalloc(BUFFER_SIZE);
	}

	for(int j = 0;j<6;++j)
	{
		for(int i = 0; i < NUMBER_OF_ARCHIVE_POINTS; ++i)
			archiveGraphPoints[j][i].x = i;
	}

	for(int i=0;i<100;++i)
	{
		channelHeader[i] = pvPortMalloc(sizeof(ARCHIVE_CHANNEL_HEADER));
	}
}

void ARCHIVE_READ_Deinitalize(void)
{
	vPortFree(ArchiveFile);
	ArchiveFile = NULL;

	vPortFree(readBuffer);
	readBuffer = NULL;

	for(int i=0;i<100;++i)
		vPortFree(channelHeader[i]);
}

static int ARCHIVE_READ_CheckVersion(char * version)
{
	char *readpointer = NULL;
	int major = atoi(strtok_r(version, ".", &readpointer));
	int minor = atoi(strtok_r(NULL, ".", &readpointer));
	int patch = atoi(strtok_r(NULL, ".", &readpointer));

	if(1 == major)
	{
		if(7 == minor)
		{
			if(3 > patch)
			{
				return 0;
			}
		}
		else if(7 > minor)
		{
			return 0;
		}
	}
	else if (1 > major)
	{
		return 0;
	}
	return 1;
}

int ARCHIVE_READ_ReadHeader(char * filePath)
{
	FIL file;
	FILINFO fileInfo;
	char *readpointer = NULL;
	char version[12] = {0};
	uint8_t prevIsCurrentArchive = 0;
	memset(readBuffer,0,BUFFER_SIZE);
	if(ArchiveFile != NULL)
	{
		prevIsCurrentArchive = ArchiveFile->isCurrentArchive;
		ArchiveFile->isCurrentArchive = ARCHIVE_IsCurrent(filePath);
		if(ARCHIVE_READ_TakeSemaphore())
		{
			f_stat(filePath, &fileInfo);
			f_open(&file, filePath, FA_READ|FA_OPEN_EXISTING);
			f_gets(readBuffer, BUFFER_SIZE, &file);
			strtok_r(readBuffer, ",\n", &readpointer);
			strncpy(version, strtok_r(NULL, ",\n", &readpointer),12);
			if(!ARCHIVE_READ_CheckVersion(version))
			{
				f_close(&file);
				ArchiveFile->isCurrentArchive = prevIsCurrentArchive;
				ARCHIVE_READ_GiveSemaphore();
				return 2;
			}
			strcpy(ArchiveFile->filePath,filePath);

			strtok_r(NULL, ",\n", &readpointer);
			strtok_r(NULL, ",\n", &readpointer);
			ArchiveFile->numberOfChannels = atoi(strtok_r(NULL, ",\n", &readpointer))-3;
			memset(readBuffer,0,strlen(readBuffer));

			f_gets(readBuffer, BUFFER_SIZE, &file);
			f_gets(readBuffer, BUFFER_SIZE, &file);

			memset(readBuffer,0,strlen(readBuffer));
			for(int i=0;i<ArchiveFile->numberOfChannels;++i)
			{
				f_gets(readBuffer, BUFFER_SIZE, &file);
				channelHeader[i]->ID = atoi(strtok_r(readBuffer, ",\n", &readpointer));
				strncpy(channelHeader[i]->name,strtok_r(NULL, ",\n", &readpointer),51);
				strncpy(channelHeader[i]->unit,strtok_r(NULL, ",\n", &readpointer),31);
				strtok_r(NULL, ",\n", &readpointer);
				strtok_r(NULL, ",\n", &readpointer);
				channelHeader[i]->trendMin = atof(strtok_r(NULL, ",\n", &readpointer));
				channelHeader[i]->trendMax = atof(strtok_r(NULL, ",\n", &readpointer));

				memset(readBuffer,0,strlen(readBuffer));
			}
			f_gets(readBuffer, BUFFER_SIZE, &file);
			f_gets(readBuffer, BUFFER_SIZE, &file);
			ArchiveFile->archiveStart = file.fptr;
			ArchiveFile->packageStart = ArchiveFile->archiveStart;
			f_lseek(&file, fileInfo.fsize);
			ArchiveFile->archiveEnd = file.fptr;
			f_close(&file);
			ARCHIVE_READ_GiveSemaphore();
			memset(readBuffer,0,BUFFER_SIZE);
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return 1;
}

void ARCHIVE_READ_UpdateChannelsHeader(void)
{
	for(int j = 0;j<6;++j)
	{
		if(0<=ArchiveFile->channelsID[j])
		{
			strcpy(ArchiveFile->recordsHead[j].name, channelHeader[ArchiveFile->channelsID[j]]->name);
			strcpy(ArchiveFile->recordsHead[j].unit,channelHeader[ArchiveFile->channelsID[j]]->unit);
			ArchiveFile->recordsHead[j].trendMin = channelHeader[ArchiveFile->channelsID[j]]->trendMin;
			ArchiveFile->recordsHead[j].trendMax = channelHeader[ArchiveFile->channelsID[j]]->trendMax;
			trendOffsetY[j] = (short) ( -ArchiveFile->recordsHead[j].trendMin / (ArchiveFile->recordsHead[j].trendMax - ArchiveFile->recordsHead[j].trendMin) * 365.0);
		}
		else
		{
			strcpy(ArchiveFile->recordsHead[j].name,"");
			strcpy(ArchiveFile->recordsHead[j].unit,"");
			ArchiveFile->recordsHead[j].trendMin = 0.0;
			ArchiveFile->recordsHead[j].trendMax = 0.0;
			trendOffsetY[j] = 0;
		}
	}
}

int TEST_RecordNumber = -1;



void ARCHIVE_ReadPackage(void)
{
	FIL file;
	char *readpointer;
	TCHAR* readStatus = NULL;
	int i = 0;
	numberOfRecords = 0;

	uint64_t tempFilePointer = ArchiveFile->packageStart;
	TEST_RecordNumber = 0;
	ArchiveFile->isErrorInPackage = 0;
	while(i < NUMBER_OF_ARCHIVE_POINTS)
	{
		if(ARCHIVE_READ_TakeSemaphore())
		{
			if(FR_OK == f_open(&file, ArchiveFile->filePath, FA_READ|FA_OPEN_EXISTING))
			{
				if(FR_OK == f_lseek(&file, tempFilePointer))
				{
					memset(readBuffer, 0, BUFFER_SIZE);
					readStatus = f_gets(readBuffer, BUFFER_SIZE, &file);
					tempFilePointer = file.fptr;
					if (NULL != readStatus)
					{
						f_close(&file);
						ARCHIVE_READ_GiveSemaphore();

						readpointer = NULL;
						char * date = strtok_r(readBuffer, ",\n", &readpointer);
						strncpy(ArchiveFile->archiveRecords[i].date, date, 9);
						char *time = strtok_r(NULL, ",\n", &readpointer);
						strncpy(ArchiveFile->archiveRecords[i].time, time, 9);
						strtok_r(NULL, ",\n", &readpointer);
						for (int j = 0; j < ArchiveFile->numberOfChannels; ++j)
						{
							tempValues[j] = strtok_r(NULL, ",\n", &readpointer);
						}
						for (int j = 0; j < 6; ++j)
						{
							if (0 <= ArchiveFile->channelsID[j])
							{
								ArchiveFile->archiveRecords[i].status[j] = ARCHIVE_READ_DecodeStatus(tempValues[ArchiveFile->channelsID[j]]);
								if (1 == ArchiveFile->archiveRecords[i].status[j])
								{
									ArchiveFile->archiveRecords[i].value[j] = atof(tempValues[ArchiveFile->channelsID[j]]);
								}
								else
								{
									ArchiveFile->isErrorInPackage = 1;
									ArchiveFile->archiveRecords[i].value[j] = NAN;
								}
							}
							else
								ArchiveFile->archiveRecords[i].value[j] = NAN;
						}
						TEST_RecordNumber++;
						i++;

					}
					else
					{
						f_close(&file);
						ARCHIVE_READ_GiveSemaphore();
						break;
					}
				}
			}
		}

		if(tempFilePointer >= ArchiveFile->archiveEnd)
		{
			tempFilePointer = ArchiveFile->archiveEnd;
			break;
		}

	}

	numberOfRecords = i;
	ArchiveFile->packageEnd = tempFilePointer;

	float tF=1;
	for(int j=0;j<6;++j)
	{
		if(0<=ArchiveFile->channelsID[j])
		{

			tF = (ArchiveFile->recordsHead[j].trendMax - ArchiveFile->recordsHead[j].trendMin) / TREND_Y_AXIS_SIZE;
			for(int k = 0; k < numberOfRecords; ++k)
			{
				if(isnan(ArchiveFile->archiveRecords[k].value[j]))
					archiveGraphPoints[j][k].y = -50;
				else
					archiveGraphPoints[j][k].y = (short) (ArchiveFile->archiveRecords[k].value[j]/tF);
			}
		}
	}
}

int ARCHIVE_READ_SelectPrevPackage(uint64_t noPages)
{
	FIL file;
	uint64_t diff = 0;
	if(NUMBER_OF_ARCHIVE_POINTS > numberOfRecords)
	{
		uint64_t avgRecordLength = (ArchiveFile->packageEnd - ArchiveFile->packageStart) / numberOfRecords;
		diff = ((avgRecordLength * NUMBER_OF_ARCHIVE_POINTS) * 4 / 5) * noPages;

	}
	else
	{
		 diff = (uint64_t)((ArchiveFile->packageEnd - ArchiveFile->packageStart) * 4 / 5) * noPages;
	}

	uint64_t prevPackageStart = ArchiveFile->packageStart;
	if(ArchiveFile->packageStart == ArchiveFile->archiveStart)
	{
		return 0;
	}

	if((ArchiveFile->archiveStart + diff) >= ArchiveFile->packageStart)
	{
		ArchiveFile->packageStart = ArchiveFile->archiveStart;
		return 1;
	}
	else
	{
		if(ARCHIVE_READ_TakeSemaphore())
		{
			if(FR_OK == f_open(&file, ArchiveFile->filePath, FA_READ|FA_OPEN_EXISTING))
			{
				if(FR_OK == f_lseek(&file, (ArchiveFile->packageStart - diff)))
				{
					memset(readBuffer,0,BUFFER_SIZE);
					f_gets(readBuffer, BUFFER_SIZE, &file);
					ArchiveFile->packageStart = file.fptr;
					ARCHIVE_READ_GiveSemaphore();
					f_close(&file);
					return 1;
				}
				else
				{
					f_close(&file);
					ARCHIVE_READ_GiveSemaphore();
					ArchiveFile->packageStart = prevPackageStart;
					return 0;
				}
			}
			else
			{
				ARCHIVE_READ_GiveSemaphore();
				ArchiveFile->packageStart = prevPackageStart;
				return 0;
			}
		}
		ArchiveFile->packageStart = prevPackageStart;
		return 0;
	}
}

int ARCHIVE_READ_SelectNextPackage(uint64_t noPages)
{
	FIL file;
	uint64_t diff = 0;
	uint64_t prevPackageStart = ArchiveFile->packageStart;
	if(ArchiveFile->packageEnd == ArchiveFile->archiveEnd)
	{
		return 0;
	}
	else
	{
		if(1 == noPages)
		{
			if(NUMBER_OF_ARCHIVE_POINTS > numberOfRecords)
			{
				uint64_t avgRecordLength = (ArchiveFile->packageEnd - ArchiveFile->packageStart) / numberOfRecords;
				diff = ((avgRecordLength * NUMBER_OF_ARCHIVE_POINTS) * 4 / 5);
			}
			else
			{
				 diff = (uint64_t)((ArchiveFile->packageEnd - ArchiveFile->packageStart) * 4 / 5);
			}

			ArchiveFile->packageStart += diff;

			if(ArchiveFile->packageStart >= ArchiveFile->archiveEnd)
			{
				ArchiveFile->packageStart = prevPackageStart;
				ARCHIVE_READ_SelectLastPackage();
				return 1;
			}
			else if((ArchiveFile->packageStart + diff) >= ArchiveFile->archiveEnd)
			{
				ArchiveFile->packageStart = prevPackageStart;
				ARCHIVE_READ_SelectLastPackage();
				return 1;
			}
		}
		else
		{
			if(NUMBER_OF_ARCHIVE_POINTS > numberOfRecords)
			{
				uint64_t avgRecordLength = (ArchiveFile->packageEnd - ArchiveFile->packageStart) / numberOfRecords;
				diff = ((avgRecordLength * NUMBER_OF_ARCHIVE_POINTS) * 4 / 5) * noPages;
			}
			else
			{
				 diff = (uint64_t)((ArchiveFile->packageEnd - ArchiveFile->packageStart) * 4 / 5) * noPages;
			}
			uint64_t newPackageStart = ArchiveFile->packageStart + diff;

			if(newPackageStart >= ArchiveFile->archiveEnd)
			{
				ARCHIVE_READ_SelectLastPackage();
				return 1;
			}
			else if((ArchiveFile->packageStart + diff) >= ArchiveFile->archiveEnd)
			{
				ARCHIVE_READ_SelectLastPackage();
				return 1;
			}
		}
		if (ARCHIVE_READ_TakeSemaphore())
		{
			if (FR_OK == f_open(&file, ArchiveFile->filePath, FA_READ | FA_OPEN_EXISTING))
			{
				if (FR_OK == f_lseek(&file, ArchiveFile->packageStart))
				{
					memset(readBuffer, 0, BUFFER_SIZE);
					f_gets(readBuffer, BUFFER_SIZE, &file);
					ArchiveFile->packageStart = file.fptr;
					f_close(&file);
					ARCHIVE_READ_GiveSemaphore();
					return 1;
				}
				else
				{
					f_close(&file);
					ARCHIVE_READ_GiveSemaphore();
					ArchiveFile->packageStart = prevPackageStart;
					return 0;
				}
			}
			else
			{
				ARCHIVE_READ_GiveSemaphore();
				ArchiveFile->packageStart = prevPackageStart;
				return 0;
			}
		}
		else
		{
			ArchiveFile->packageStart = prevPackageStart;
			return 0;
		}
	}
}

void ARCHIVE_READ_SelectLastPackage(void)
{
	FIL file;
	numberOfRecords = 0;
	if (ARCHIVE_READ_TakeSemaphore())
	{
		if (FR_OK == f_open(&file, ArchiveFile->filePath, FA_READ|FA_OPEN_EXISTING))
		{
			if (FR_OK == f_lseek(&file, ArchiveFile->packageStart))
			{
				memset(readBuffer,0,BUFFER_SIZE);
				f_gets(readBuffer, BUFFER_SIZE, &file);
				uint64_t recordLenght = strlen(readBuffer);
				uint64_t packageLenght = NUMBER_OF_ARCHIVE_POINTS * recordLenght * 4 / 5;
				if((ArchiveFile->archiveEnd - ArchiveFile->archiveStart) <= packageLenght)
				{
					ArchiveFile->packageStart = ArchiveFile->archiveStart;
				}
				else
				{
					f_lseek(&file, ArchiveFile->archiveEnd - packageLenght);
					f_gets(readBuffer, BUFFER_SIZE, &file);
					ArchiveFile->packageStart = file.fptr;
				}
			}
			f_close(&file);
		}
		ARCHIVE_READ_GiveSemaphore();
	}
}

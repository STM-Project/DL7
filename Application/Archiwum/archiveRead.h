/*
 * archiveRead.h
 *
 *  Created on: 22.04.2021
 *      Author: TomaszSok
 */

#ifndef ARCHIWUM_ARCHIVEREAD_H_
#define ARCHIWUM_ARCHIVEREAD_H_

#include <stdint.h>
#include "GUI.h"

#include <stdint.h>
#include "GUI.h"

#define NUMBER_OF_ARCHIVE_POINTS 	480
#define TREND_Y_AXIS_SIZE					335

typedef struct {
	char date[9];
	char time[9];
	uint8_t status[6];
	float value[6];


}ARCHIVE_RECORD;

typedef struct {
	char name[52];
	char unit[32];
	float trendMin;
	float trendMax;
}ARCHIVE_RECORD_HEAD;

typedef struct {
	uint64_t archiveStart;
	uint64_t archiveEnd;
	uint64_t packageStart;
	uint64_t packageEnd;

	char filePath[32];
	uint8_t isCurrentArchive;
	uint8_t isErrorInPackage;
	ARCHIVE_RECORD archiveRecords[NUMBER_OF_ARCHIVE_POINTS];
	ARCHIVE_RECORD_HEAD recordsHead[6];
	int8_t channelsID[6];
	uint8_t numberOfChannels;
}ARCHIVE_FILE;

typedef struct{
	int32_t ID;
	char name[51];
	char unit[31];
	float trendMin;
	float trendMax;
}ARCHIVE_CHANNEL_HEADER;

extern ARCHIVE_FILE *ArchiveFile;
extern ARCHIVE_CHANNEL_HEADER *channelHeader[];

extern int numberOfRecords;
extern GUI_POINT archiveGraphPoints[6][NUMBER_OF_ARCHIVE_POINTS];
extern short trendOffsetY[6];

extern int8_t selectedChannelsID[];

void ARCHIVE_READ_ResetSelectedChannels(void);

void ARCHIVE_READ_Initialize(void);
void ARCHIVE_READ_Deinitalize(void);


int ARCHIVE_READ_ReadHeader(char * filePath);
void ARCHIVE_READ_UpdateChannelsHeader(void);
void ARCHIVE_READ_SelectLastPackage(void);

void ARCHIVE_ReadPackage(void);
int ARCHIVE_READ_SelectPrevPackage(uint64_t noPages);
int ARCHIVE_READ_SelectNextPackage(uint64_t noPages);

#endif /* ARCHIWUM_ARCHIVEREAD_H_ */

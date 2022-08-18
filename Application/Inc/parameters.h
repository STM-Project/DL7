/*
 * parameters.h
 *
 *  Created on: 9 lut 2016
 *      Author: Tomaszs
 */

#ifndef UTILITIES_PARAMETERS_H_
#define UTILITIES_PARAMETERS_H_
#include <stdint.h>
#include "ff.h"
#include "channels_typedef.h"

typedef struct
{
	uint8_t DeviceLanguage;
	char DeviceDescription[81];
	char DeviceID;
	uint8_t Brightness;
	uint8_t ScreenSaverBrightness;
	uint8_t ScreenSaverTime;
	uint8_t BeeperMode;
	uint8_t AlarmBeeperMode;
	uint8_t DSTMode;
} GENERAL_SETTINGS;

typedef struct
{
	int ArchivizationFrequency1;
	int ArchivizationFrequency2;
	int TotalizerArchivizationFrequency;
	unsigned char ArchiveMaxSize;
	unsigned char canUserStopArchive;
} ARCHIVE_SETTINGS;

typedef struct
{
	uint8_t IPAddress[4];
	uint16_t IPport;
	uint8_t NetMask[4];
	uint8_t IPGateway[4];
	uint8_t primaryDNS[4];
	uint8_t secondaryDNS[4];
} ETH_SETTINGS;

typedef struct
{
	short ComBaudrate;
	short ComParity;
	short ComStopBits;
	short MBAddress;
} COM_SETTINGS;

typedef struct
{
	char TablesTitles[6][41];
	short TablesData[90];
} USER_TABLES_SETTINGS;

typedef struct{
	char isActive;
	char description[48];
	float min;
	float max;
	int channelID[6];
	CHANNEL *pChannel[6];
}USER_TREND;


typedef struct {
	char name[61];
	float pointsX[100];
	float pointsY[100];
	uint8_t numberOfPoints;
} USER_CHAR;

extern GENERAL_SETTINGS GeneralSettings;
extern GENERAL_SETTINGS bkGeneralSettings;

extern ARCHIVE_SETTINGS ArchiveSettings;
extern ARCHIVE_SETTINGS bkArchiveSettings;

extern ETH_SETTINGS EthSettings;
extern ETH_SETTINGS bkEthSettings;

extern COM_SETTINGS ComSettings;
extern COM_SETTINGS bkComSettings;

extern USER_TABLES_SETTINGS UserTablesSettings;
extern USER_TABLES_SETTINGS bkUserTablesSettings;

extern USER_TREND UserTrendsSettings[6];
extern USER_TREND bkUserTrendsSettings[6];

extern USER_CHAR UserCharacteristcs[10];
extern USER_CHAR bkUserCharacteristics[10];

extern uint8_t NewSettings;
extern uint8_t NewArchive;
extern uint8_t RestartDevice;
extern uint8_t NewUserChar;

void CopyGeneralSettings(GENERAL_SETTINGS *Dst, GENERAL_SETTINGS *Src);
void CopyArchiveSettings(ARCHIVE_SETTINGS *Dst, ARCHIVE_SETTINGS *Src);
void CopyEthSettings(ETH_SETTINGS *Dst, ETH_SETTINGS *Src);
void CopyComSettings(COM_SETTINGS *Dst, COM_SETTINGS *Src);
void CopyUserTablesSettings(USER_TABLES_SETTINGS *Dst, USER_TABLES_SETTINGS *Src);
void CopyUserTrendsSettings(USER_TREND *Dst, USER_TREND *Src);
void CopyUserCharSettings(USER_CHAR *Dst, USER_CHAR *Src);

void SetLanguage(uint8_t newLang);
void InitLanguage(void);

void LoadParametersFile();
void saveParameters(void);
void RestoreDefaultSettings(void);
int loadParameters(const TCHAR* path);

void FillListOfActiveChannels (void);
uint8_t CheckChannelType(int ChannelIndex);

int ChcekParametersFile(const TCHAR* path);

void InitializeModules(void);

#endif /* UTILITIES_PARAMETERS_H_ */

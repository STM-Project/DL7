/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */
uint8_t retUSBH;    /* Return value for USBH */
char USBHPath[4];   /* USBH logical drive path */
FATFS USBHFatFS;    /* File system object for USBH logical drive */
FIL USBHFile;       /* File object for USBH */

/* USER CODE BEGIN Variables */
#include "rtc.h"
#include "archive.h"
#include "shift_reg.h"
#include "mini-printf.h"
#include "passwords.h"
static BYTE USBCopyBuffer[131072] __attribute__ ((section(".sdram")));

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);
  /*## FatFS: Link the USBH driver ###########################*/
  retUSBH = FATFS_LinkDriver(&USBH_Driver, USBHPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);

  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
	DWORD res;
	RTC_TimeTypeDef RTCTimeFAT;
	RTC_DateTypeDef RTCDateFAT;
	RTC_GetTimeAndDate(&RTCTimeFAT, &RTCDateFAT);
	res = ((DWORD) (RTCDateFAT.Year + 20) << 25)
			| ((DWORD) RTCDateFAT.Month << 21)
			| ((DWORD) RTCDateFAT.Date << 16)
			| (WORD) (RTCTimeFAT.Hours << 11)
			| (WORD) (RTCTimeFAT.Minutes << 5)
			| (WORD) (RTCTimeFAT.Seconds >> 1);

	return res;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
void FATFS_GetFileDate(const char *FileName, uint8_t *dateAndTime)
{
	FILINFO file;
	FRESULT fr;

	fr = f_stat(FileName, &file);
	if (fr == FR_OK)
	{
		dateAndTime[0] = (uint8_t) (((file.fdate & 0xFE00) >> 9) - 20);
		dateAndTime[1] = (uint8_t) ((file.fdate & 0x1E0) >> 5);
		dateAndTime[2] = (uint8_t) (file.fdate & 0x1F);

		dateAndTime[3] = (uint8_t) ((file.ftime & 0xF800) >> 11);
		dateAndTime[4] = (uint8_t) ((file.ftime & 0x7E0) >> 5);
		dateAndTime[5] = (uint8_t) ((file.ftime & 0xF) << 1);
	}
	else
	{
		dateAndTime[0] = 0;
		dateAndTime[1] = 0;
		dateAndTime[2] = 0;
		dateAndTime[3] = 0;
		dateAndTime[4] = 0;
		dateAndTime[5] = 0;
	}
}

void FATFS_CopyFileDateToString(char* buffer ,FILINFO *filinfo)
{
	uint8_t fileDate[3]={0};
	uint8_t fileTime[2]={0};

	fileDate[0] = (uint8_t) (((filinfo->fdate & 0xFE00) >> 9) - 20);
	fileDate[1] = (uint8_t) ((filinfo->fdate & 0x1E0) >> 5);
	fileDate[2] = (uint8_t) (filinfo->fdate & 0x1F);

	fileTime[0] = (uint8_t) ((filinfo->ftime & 0xF800) >> 11);
	fileTime[1] = (uint8_t) ((filinfo->ftime & 0x7E0) >> 5);
	mini_snprintf(buffer, 20, "%02d-%02d-%02d %02d:%02d", fileDate[0],fileDate[1],fileDate[2],fileTime[0],fileTime[1]);
}

float FATFS_GetFileSize(const char *FileName)
{
	FILINFO file;
	FRESULT fr;

	fr = f_stat(FileName, &file);
	if (fr == FR_OK)
	{
		return ((float)file.fsize)/1024.0;
	}
	else
		return 0.0;
}

int FATFS_GetFreeSpaceOfSD(void)
{
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT res;

	res = f_getfree("0:", &fre_clust, &fs);
	if (res == FR_OK)
	{
		tot_sect = (fs->n_fatent - 2) * fs->csize;
		fre_sect = fre_clust * fs->csize;
		return 100 - 100 * ((float) fre_sect / (float) tot_sect);
	}
	else
		return 0;
}

int FATFS_CopyFile(char *Src, char *Dst)
{
	FIL fsrc, fdst;
	FRESULT fr;
	UINT numberOfReadBytes, numberOfWrittenBytes;

	fr = f_open(&fsrc, Src, FA_OPEN_EXISTING | FA_READ);

	if (fr != FR_OK)
		return (int) fr;

	fr = f_open(&fdst, Dst, FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK)
		return (int) fr;

	BLUE_LED_OFF;

	for (;;)
	{
		BLUE_LED_TOGGLE;
		PASSWORDS_ResetLogOutTimer();
		fr = f_read(&fsrc, USBCopyBuffer, sizeof(USBCopyBuffer), &numberOfReadBytes);
		if (fr != FR_OK || numberOfReadBytes == 0)
			break;
		fr = f_write(&fdst, USBCopyBuffer, numberOfReadBytes, &numberOfWrittenBytes);
		if (fr != FR_OK || numberOfWrittenBytes < numberOfReadBytes)
			break;
	}
	BLUE_LED_OFF;

	f_close(&fsrc);
	f_close(&fdst);

	return (int) fr;
}

int FREERTOS_CopyFile(char *Src, char *Dst)
{
	FIL fsrc, fdst;
	FRESULT fr;
	UINT numberOfReadBytes, numberOfWrittenBytes;

	DWORD filePointer = 0;

	fr = f_open(&fdst, Dst, FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK)
		return (int) fr;

	BLUE_LED_OFF;

	for (;;)
	{
		BLUE_LED_TOGGLE;
		PASSWORDS_ResetLogOutTimer();
		if (ARCHIVE_TakeSemaphore(50))
		{
			fr = f_open(&fsrc, Src, FA_OPEN_EXISTING | FA_READ);
			f_lseek(&fsrc, filePointer);
			fr = f_read(&fsrc, USBCopyBuffer, sizeof(USBCopyBuffer), &numberOfReadBytes);
			filePointer = fsrc.fptr;
			f_close(&fsrc);
			ARCHIVE_GiveSemaphore();

			if (fr != FR_OK || numberOfReadBytes == 0)
				break;

			fr = f_write(&fdst, USBCopyBuffer, numberOfReadBytes, &numberOfWrittenBytes);
			if (fr != FR_OK || numberOfWrittenBytes < numberOfReadBytes)
				break;

		}
	}
	BLUE_LED_OFF;
	f_close(&fdst);

	return (int) fr;
}

/* USER CODE END Application */

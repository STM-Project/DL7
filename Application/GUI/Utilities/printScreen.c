/*
 * printscreen.c
 *
 *  Created on: 26 sie 2015
 *      Author: Tomaszs
 */

#include "printScreen.h"
#include "GUI.h"
#include "fatfs.h"
#include "mini-printf.h"
#include "shift_reg.h"

#define PRINTSCREEN_BUFFER_SIZE 8192

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

SDRAM U8 printScreenBuffer[PRINTSCREEN_BUFFER_SIZE];
SDRAM unsigned int printScreenBufferIndex;

static uint8_t PrintScreenNumber = 0;

static void _WriteBytes2File(U8 Data, void * p)
{
	UINT BytesNum = 0;
	printScreenBuffer[printScreenBufferIndex] = Data;
	printScreenBufferIndex++;
	if(printScreenBufferIndex == PRINTSCREEN_BUFFER_SIZE)
	{
		f_write((FIL *) p, &printScreenBuffer, PRINTSCREEN_BUFFER_SIZE, &BytesNum);
		printScreenBufferIndex=0;
		BLUE_LED_TOGGLE;
	}
}

void PrintScreen(void)
{
	FIL ParametersFile;
	char FilePath[20];
	UINT BytesNum = 0;

	BLUE_LED_ON;
	SCB_CleanInvalidateDCache();
	SCB_DisableDCache();
	mini_snprintf(FilePath,16, "0:prtscr/%02d.bmp", PrintScreenNumber);
	FILINFO filInfo={0};
	if(FR_NO_FILE == f_stat("0:prtscr", &filInfo))
		f_mkdir("0:prtscr");

	f_open(&ParametersFile, FilePath, FA_CREATE_ALWAYS | FA_WRITE);
	printScreenBufferIndex=0;
	GUI_BMP_SerializeEx(_WriteBytes2File, 0, 0, 800, 480, &ParametersFile);
	f_write(&ParametersFile, &printScreenBuffer, printScreenBufferIndex, &BytesNum);
	f_close(&ParametersFile);
	SCB_CleanInvalidateDCache();
	SCB_EnableDCache();
	BLUE_LED_OFF;

	if (++PrintScreenNumber > 99)
		PrintScreenNumber = 0;
}

/*
 * TaskCardsBoot.c
 *
 *  Created on: 04.09.2020
 *      Author: RafalMar
 */

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "ExpansionCards.h"
#include "shift_reg.h"
#include "usb_host.h"
#include "spi.h"
#include "UserMessage.h"

#include "UpgradeBoardFirmwareDLG.h"
#include "TaskUpgradeBoardFirmware.h"

#include "fatfs.h"
#include "USBtask.h"

#define	MAX_NUMBER_REPLAY	3
#define	WAIT_FOR_SPI_RECV_MS	100
#define	MAX_BYTES_IN_PACKET		1024

#define HEADER_COMMAND_3 	6

typedef struct
{
	uint8_t nrPacket2Send;
	uint8_t allPacket;
	uint16_t BytesInPacket;
	uint16_t BytesInLastPacket;
} FileInfo_struct;

FileInfo_struct FileInfo;

static uint8_t readFileBuffer[MAX_FILE_SIZE] __attribute__ ((section(".sdram")));
static uint8_t SPI_TxBuffer[1032] __attribute__ ((section(".sdram")));

static xTaskHandle vtskUploadHandle;
static uint8_t localBoardNumber = 0;
static uint8_t SPI_RxBuffer[3];
static int answer;
static UINT readFileSize = 0;
uint32_t isTaskCreated = 0;

static uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
	uint32_t crc = crc_in;
	uint32_t in = byte | 0x100;

	do
	{
		crc <<= 1;
		in <<= 1;
		if (in & 0x100)
			++crc;
		if (crc & 0x10000)
			crc ^= 0x1021;
	}

	while (!(in & 0x10000));

	return crc & 0xffffu;
}

static uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size)
{
	uint32_t crc = 0;
	const uint8_t* dataEnd = p_data + size;

	while (p_data < dataEnd)
		crc = UpdateCRC16(crc, *p_data++);

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);

	return crc & 0xffffu;
}

static uint8_t SPI_WaitForStateReady(void)
{
	uint32_t Timeout = 0;
	HAL_SPI_StateTypeDef SPI5Status = HAL_SPI_STATE_RESET;
	do
	{
		SPI5Status = SPI_GetState();
		if (SPI5Status == HAL_SPI_STATE_ERROR)
			return ERROR;

		else if (SPI5Status == HAL_SPI_STATE_RESET)
			return ERROR;

		if (++Timeout > 10000)
		{
			return ERROR;
		}
	} while (SPI5Status != HAL_SPI_STATE_READY);
	return SUCCESS;
}

static uint8_t SEND_Command(uint8_t *data, uint16_t len)
{
	if (IOBoards_SPI_Transmit(data, len) != HAL_OK)
		return ERROR;
	if (ERROR == SPI_WaitForStateReady())
		return ERROR;

	return SUCCESS;
}

static uint8_t RECEIVE_Responce(uint8_t *data, uint16_t len)
{
	if (IOBoards_SPI_Receive(data, len) != HAL_OK)
		return ERROR;
	if (ERROR == SPI_WaitForStateReady())
		return ERROR;

	return SUCCESS;
}

static int SEND_CommandFirmwareUpgradeInit(void)
{
	uint8_t command[4] = { 0xaa, 0x55, 0xaa, 0x55 };

	IOBoards_ChipSelect(localBoardNumber);

	if (ERROR == SEND_Command(command, 4))
	{
		IOBoards_DisableChipSelect();
		return 0;
	}

	memset(SPI_RxBuffer, 0, 3);
	if (SUCCESS == RECEIVE_Responce(SPI_RxBuffer, 3))
	{
		if ((SPI_RxBuffer[0] == command[0]) && (SPI_RxBuffer[1] == command[1]))
		{
			IOBoards_DisableChipSelect();
			return SPI_RxBuffer[2];
		}
	}

	IOBoards_DisableChipSelect();
	return 0;
}

static int SEND_CommandStartEraseFlash(void)
{
	uint8_t command[2] = { 0xaa, 0x01 };

	IOBoards_ChipSelect(localBoardNumber);

	if (ERROR == SEND_Command(command, 2))
	{
		IOBoards_DisableChipSelect();
		return 0;
	}

	memset(SPI_RxBuffer, 0, 3);
	if (SUCCESS == RECEIVE_Responce(SPI_RxBuffer, 3))
	{
		if ((SPI_RxBuffer[0] == command[0]) && (SPI_RxBuffer[1] == command[1]))
		{
			IOBoards_DisableChipSelect();
			return SPI_RxBuffer[2];
		}
	}

	IOBoards_DisableChipSelect();
	return 0;
}

static int SEND_CommandEraseFlash(void)
{
	uint8_t command[2] = { 0xaa, 0x02 };

	IOBoards_ChipSelect(localBoardNumber);

	if (ERROR == SEND_Command(command, 2))
	{
		IOBoards_DisableChipSelect();
		return 0;
	}

	memset(SPI_RxBuffer, 0, 3);
	if (SUCCESS == RECEIVE_Responce(SPI_RxBuffer, 3))
	{
		if ((SPI_RxBuffer[0] == command[0]) && (SPI_RxBuffer[1] == command[1]))
		{
			IOBoards_DisableChipSelect();
			return SPI_RxBuffer[2];
		}
	}

	IOBoards_DisableChipSelect();
	return 0;
}

static int SEND_CommandWriteData(void)
{
	SPI_TxBuffer[0] = 0xaa;
	SPI_TxBuffer[1] = 0x03;

	IOBoards_ChipSelect(localBoardNumber);

	if (ERROR == SEND_Command(SPI_TxBuffer, 2))
	{
		IOBoards_DisableChipSelect();
		return 0;
	}

	memset(SPI_RxBuffer, 0, 3);
	if (SUCCESS == RECEIVE_Responce(SPI_RxBuffer, 3))
	{
		if ((SPI_RxBuffer[0] == 0xaa) && (SPI_RxBuffer[1] == 0x03))
		{
			IOBoards_DisableChipSelect();
			return SPI_RxBuffer[2];
		}
	}

	IOBoards_DisableChipSelect();
	return 0;
}

static int SEND_CommandWriteDataInit(void)
{
	int checkSum;

	SPI_TxBuffer[0] = 0xaa;
	SPI_TxBuffer[1] = 0x03;
	SPI_TxBuffer[2] = FileInfo.nrPacket2Send;
	SPI_TxBuffer[3] = FileInfo.allPacket;

	if (FileInfo.nrPacket2Send == FileInfo.allPacket)
	{
		SPI_TxBuffer[4] = FileInfo.BytesInLastPacket >> 8;
		SPI_TxBuffer[5] = FileInfo.BytesInLastPacket;

		for (int i = 0; i < FileInfo.BytesInLastPacket; i++)
			SPI_TxBuffer[HEADER_COMMAND_3 + i] = readFileBuffer[(FileInfo.nrPacket2Send - 1) * MAX_BYTES_IN_PACKET + i];

		for (int i = 0; i < MAX_BYTES_IN_PACKET - FileInfo.BytesInLastPacket; i++)
			SPI_TxBuffer[HEADER_COMMAND_3 + FileInfo.BytesInLastPacket + i] = 0xFF;

		checkSum = Cal_CRC16(SPI_TxBuffer, HEADER_COMMAND_3 + FileInfo.BytesInLastPacket);
		SPI_TxBuffer[HEADER_COMMAND_3 + FileInfo.BytesInLastPacket] = checkSum >> 8;
		SPI_TxBuffer[HEADER_COMMAND_3 + FileInfo.BytesInLastPacket + 1] = checkSum;

	}
	else
	{
		int maxBytesInPacket = MAX_BYTES_IN_PACKET;
		SPI_TxBuffer[4] = maxBytesInPacket >> 8;
		SPI_TxBuffer[5] = maxBytesInPacket;

		for (int i = 0; i < MAX_BYTES_IN_PACKET; i++)
			SPI_TxBuffer[HEADER_COMMAND_3 + i] = readFileBuffer[(FileInfo.nrPacket2Send - 1) * MAX_BYTES_IN_PACKET + i];

		checkSum = Cal_CRC16(SPI_TxBuffer, HEADER_COMMAND_3 + MAX_BYTES_IN_PACKET);
		SPI_TxBuffer[HEADER_COMMAND_3 + MAX_BYTES_IN_PACKET] = checkSum >> 8;
		SPI_TxBuffer[HEADER_COMMAND_3 + MAX_BYTES_IN_PACKET + 1] = checkSum;

	}

	IOBoards_ChipSelect(localBoardNumber);

	if (ERROR == SEND_Command(SPI_TxBuffer, 1032))
	{
		IOBoards_DisableChipSelect();
		return ERROR;
	}

	IOBoards_DisableChipSelect();
	return SUCCESS;
}

static int ReadFile(char *Src)
{
	FIL fsrc;
	FRESULT fr;
	FILINFO filInfo;

	fr = f_stat(Src, &filInfo);
	if (FR_OK == fr)
	{
		if(filInfo.fsize <= MAX_FILE_SIZE)
		{
			fr = f_open(&fsrc, Src, FA_OPEN_EXISTING | FA_READ);

			if (fr != FR_OK)
				return (int) fr;

			fr = f_read(&fsrc, readFileBuffer, sizeof(readFileBuffer), &readFileSize);
			f_close(&fsrc);
		}
		else
		{
			fr = FR_NO_FILE;
		}
	}
	return (int) fr;
}

static void PrepareFile2Send(void)
{
	FileInfo.nrPacket2Send = 1;
	FileInfo.allPacket = readFileSize / MAX_BYTES_IN_PACKET;
	FileInfo.BytesInPacket = MAX_BYTES_IN_PACKET;
	FileInfo.BytesInLastPacket = readFileSize - MAX_BYTES_IN_PACKET * (readFileSize / MAX_BYTES_IN_PACKET);

	if (FileInfo.BytesInLastPacket > 0)
		FileInfo.allPacket++;
}

static void TaskBootMessageToDialog(int messageID, int status)
{
	UserMessage.MsgId = messageID;
	UserMessage.hWin = CardBootWindow;
	UserMessage.hWinSrc = 0;
	UserMessage.Data.v = status;
	WM_SendMessage(CardBootWindow, &UserMessage);
}

void vCloseTaskFirmwareUpgrade(void)
{
	if (isTaskCreated)
	{
		isTaskCreated = 0;
		sendUSBEvent(READY);
		vTaskDelete(vtskUploadHandle);
	}
}

static void vBoardFirmwareUpgrade_Init(void)
{
	answer = SEND_CommandFirmwareUpgradeInit();
	if (_OK == answer)
		TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, FIRMWARE_UPGRADE_INIT);
	else
	{
		TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, FIRMWARE_UPGRADE_INIT_ERROR);
		vCloseTaskFirmwareUpgrade();
	}
}

static void vStartEraseFlash(void)
{
	for (int i = 0; i < MAX_NUMBER_REPLAY; ++i)
	{
		answer = SEND_CommandStartEraseFlash();
		if (_OK == answer)
		{
			TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, START_ERASE_FLASH);
			break;
		}
		else
		{
			vTaskDelay(1000);

			if (MAX_NUMBER_REPLAY - 1 == i)
			{
				TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, START_ERASE_FLASH_FAIL);
				vCloseTaskFirmwareUpgrade();
			}
		}
	}
}

static void vEraseFlash(void)
{
	for (int i = 0; i < MAX_NUMBER_REPLAY; ++i)
	{
		answer = SEND_CommandEraseFlash();
		if (_OK == answer)
		{
			TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, ERASE_FLASH_SUCCESS);
			break;
		}
		else
		{
			vTaskDelay(2000);

			if (MAX_NUMBER_REPLAY - 1 == i)
			{
				TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, answer);
				vCloseTaskFirmwareUpgrade();
			}
		}
	}
}

static void vWriteData(void)
{
	for (int j = 0; j < FileInfo.allPacket; ++j)
	{
		for (int i = 0; i < MAX_NUMBER_REPLAY; ++i)
		{
			vTaskDelay(10);
			SEND_CommandWriteDataInit();
			vTaskDelay(100);

			answer = SEND_CommandWriteData();
			if (_OK == answer)
			{
				TaskBootMessageToDialog(WM_USER_BOARD_WRITE_DATA_INPROGRESS, FileInfo.nrPacket2Send);
				FileInfo.nrPacket2Send++;
				break;
			}
			else
			{
				if (MAX_NUMBER_REPLAY - 1 == i)
				{
					TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, answer);
					vCloseTaskFirmwareUpgrade();
				}
			}
		}
	}
}

static void vTaskFirmwareUpgrade(void *pvParameters)
{
	isTaskCreated = 1;

	if (FR_OK != ReadFile(SrcFilePath))
	{
		TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, READ_FILE_ERROR);
		vCloseTaskFirmwareUpgrade();
	}
	else
		TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, READ_FILE_OK);

	PrepareFile2Send();

	IOBOARD_PWR_OFF(localBoardNumber);
	vTaskDelay(500);
	IOBOARD_PWR_ON(localBoardNumber);
	vTaskDelay(100);

	vBoardFirmwareUpgrade_Init();
	vTaskDelay(500);

	vStartEraseFlash();
	vTaskDelay(2000);

	vEraseFlash();
	vTaskDelay(1000);

	TaskBootMessageToDialog(WM_USER_BOARD_WRITE_DATA_INPROGRESS, 0);
	vTaskDelay(500);

	vWriteData();
	vTaskDelay(1000);

	TaskBootMessageToDialog(WM_USER_BOARD_FIRMWARE_UPGRADE, WRITE_DATA_COMPLETE);
	vCloseTaskFirmwareUpgrade();

}

void CreateUpgradeBoardFirmwareTask(uint8_t boardNumber)
{
	if (0 == isTaskCreated)
	{
		localBoardNumber = boardNumber;
		xTaskCreate(vTaskFirmwareUpgrade, (char*) "vTaskFirmwareUpgrade", 2000, NULL, (unsigned portBASE_TYPE ) 2, &vtskUploadHandle);
	}
}

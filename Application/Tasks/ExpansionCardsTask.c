/*
 * ExpansionCardsTask.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#include "ExpansionCards.h"
#include "watchdog.h"
#include "ExpansionCardsTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "StartUp.h"

#include "StartUp.h"
#include "ChannelsSynchronization.h"

#define UPDATE_OUTPUTS							0
#define COMMUNICATE_TO_BOARDS				1
#define UPDATE_INPUTS								2
#define CALIBRATION_MODE						3

static void IOBoards_PowerUpAllBoards(void);

static EXPANSION_CARD localExpansionCards[7] __attribute__ ((section(".sdram")));

static uint8_t CalibrationMode = 0;
xTaskHandle vtaskSPIHandle;

void vtaskSPI(void *pvParameters)
{
	uint8_t boardNumber = BOARD_A;
	uint8_t taskState = COMMUNICATE_TO_BOARDS;
	portTickType xLastExecutionTime = 0;
	uint8_t BoardRestartDelayCounter[7] =
	{ 0, 0, 0, 0, 0, 0, 0 };
	uint8_t FirstIterationFlag = 1;

	while (1)
	{
		if (STARTUP_WaitForBits(0x0004))
		{
			for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
				IOBoards_PowerOff(boardNumber);

			vTaskDelay(50);
			IOBoards_Initalize();
			IOBoards_PowerUpAllBoards();
			IOBoards_InitalizeBoards();

			for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
			{
				for (int j=0;j<25;j++)
				{
					ExpansionCards[boardNumber].channel[j].value=0;
					ExpansionCards[boardNumber].channel[j].diffValue = 0;
				}

				IOBoards_ChipSelect(boardNumber);
				localExpansionCards[boardNumber] = ExpansionCards[boardNumber];
				IOBoards_WriteSettingsToBoard(&localExpansionCards[boardNumber]);
			}
			IOBoards_DisableChipSelect();

			while (1)
			{
				if (1 == CalibrationMode)
					taskState = CALIBRATION_MODE;

				switch (taskState)
				{
				case UPDATE_OUTPUTS:
					if (TakeChannelsMutex(0))
					{
						xLastExecutionTime = xTaskGetTickCount();
						for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
						{
							if(SPI_CARD_OUT6RL==localExpansionCards[boardNumber].type || SPI_CARD_OUT3==localExpansionCards[boardNumber].type)
							{
								localExpansionCards[boardNumber] = ExpansionCards[boardNumber];
							}
						}
						GiveChannelsMutex();
						taskState = COMMUNICATE_TO_BOARDS;
					}
					else
						vTaskDelay(5);

					break;
				case COMMUNICATE_TO_BOARDS:
					for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
					{
						switch (localExpansionCards[boardNumber].status)
						{
						case SPI_BOARD_OK:
							IOBoards_ChipSelect(boardNumber);
							switch (localExpansionCards[boardNumber].type)
							{
							case SPI_CARD_IN6I24:
								IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_IN6I:
								IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_IN6V:
								IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_IN6TC:
								if(110 < localExpansionCards[boardNumber].firmwareRevision)
									IOBoards_ReadFromTCBoard(&localExpansionCards[boardNumber], boardNumber);
								else
								{
									IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
									localExpansionCards[boardNumber].channel[6].status = DISABLED; //TODO: Do it once after card settings
									localExpansionCards[boardNumber].channel[6].value = 0.0;
									localExpansionCards[boardNumber].channel[7].status = DISABLED;
									localExpansionCards[boardNumber].channel[7].value = 0.0;
								}
								break;
							case SPI_CARD_IN6RTD:
							case SPI_CARD_IN3RTD:
								IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_OUT6RL:
								IOBoards_WriteToOutputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_2RS485:
							case SPI_CARD_2RS485I24:
								IOBoards_ReadFrom2RS485Board(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_IN3:
							case SPI_CARD_IN4SG:
								IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_IN6D:
								IOBoards_ReadFromIN6DBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_OUT3:
								IOBoards_WriteToOUT3Board(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_1HRT:
								IOBoards_ReadFromHARTBoard(&localExpansionCards[boardNumber], boardNumber);
								break;
							case SPI_CARD_PSBATT:
								IOBoards_ReadFromInputBoards(&localExpansionCards[boardNumber], boardNumber);
								break;
							}
							break;
						case SPI_BOARD_ERROR:
							IOBoards_PowerOn(boardNumber);
							localExpansionCards[boardNumber].status = SPI_BOARD_INCONFIG;
							BoardRestartDelayCounter[boardNumber] = 0;
							break;
						case SPI_BOARD_INCONFIG:
							BoardRestartDelayCounter[boardNumber]++;
							if (BoardRestartDelayCounter[boardNumber] > 3)
							{
								BoardRestartDelayCounter[boardNumber] = 0;
								IOBoards_ChipSelect(boardNumber);
								IOBoards_ReadBoardID(&localExpansionCards[boardNumber], boardNumber);
								IOBoards_DisableChipSelect();
								IOBoards_ChipSelect(boardNumber);
								IOBoards_WriteSettingsToBoard(&localExpansionCards[boardNumber]);

								for (int j=0;j<25;j++)
								{
									localExpansionCards[boardNumber].channel[j].value=0;
									localExpansionCards[boardNumber].channel[j].diffValue = 0;
								}
							}
							break;
						default:
							break;
						}
					}
					IOBoards_DisableChipSelect();

					taskState = UPDATE_INPUTS;
					break;
				case UPDATE_INPUTS:

					if (FirstIterationFlag)
					{
						FirstIterationFlag = 0;
						STARTUP_ClaerBits(0x0004);
						STARTUP_SetBits(0x0008);
					}

					if (TakeChannelsMutex(0))
					{
						for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
						{
							ExpansionCards[boardNumber].status = localExpansionCards[boardNumber].status;
							for(int i =0;i<25;++i)
							{
								ExpansionCards[boardNumber].channel[i].status = localExpansionCards[boardNumber].channel[i].status;
								ExpansionCards[boardNumber].channel[i].value = localExpansionCards[boardNumber].channel[i].value;
								ExpansionCards[boardNumber].channel[i].diffValue += localExpansionCards[boardNumber].channel[i].diffValue;
							}

						}
						GiveChannelsMutex();
						taskState = UPDATE_OUTPUTS;
						vTaskDelayUntil(&xLastExecutionTime, 1000);
					}
					else
						vTaskDelay(5);

					break;
				case CALIBRATION_MODE:
					if (0 == CalibrationMode)
					{
						for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
						{
							localExpansionCards[boardNumber] = ExpansionCards[boardNumber];
							IOBoards_ChipSelect(boardNumber);
							IOBoards_WriteSettingsToBoard(&localExpansionCards[boardNumber]);
						}
						IOBoards_DisableChipSelect();
						taskState = UPDATE_INPUTS;
						vTaskDelay(5);
					}
					else
						vTaskDelay(250);
					break;
				}

				WDFlags[1] = ALIVE;
			}
		}
		else
			continue;
	}
}

void CreateExpansionCardsTask(void)
{
	xTaskCreate(vtaskSPI, "vtaskSPI", 1000, NULL, (unsigned portBASE_TYPE ) 4, &vtaskSPIHandle);
}

static void IOBoards_PowerUpAllBoards(void)
{
	for (int boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
	{
		IOBoards_PowerOn(boardNumber);
		vTaskDelay(150);
	}
	vTaskDelay(1500);
}

void ActivateCalibrationMode(void)
{
	CalibrationMode = 1;
}

void DeactivateCalibrationMode(void)
{
	CalibrationMode = 0;
}

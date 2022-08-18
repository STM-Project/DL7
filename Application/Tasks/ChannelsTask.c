/*
 * ChannelsTask.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#include "channels.h"
#include "ChannelsTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <math.h>
#include "measurments.h"
#include "watchdog.h"
#include "fram.h"
#include "StartUp.h"
#include "archive.h"
#include "ChannelsSynchronization.h"
#include "ExpansionCards.h"
#include "ComputeChannel.h"
#include "outputExpansionCards.h"
#include "tcpipclient_netconn.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

#define CH_SOURCE_OFF 				0
#define CH_SOURCE_MEASURMENT	1
#define CH_SOURCE_COMPUTE			2
#define CH_SOURCE_REMOTE			3
#define CH_SOURCE_DEMO				4

static float UpdateDemoValue(void);
static void SetTimeCorrectionForTotalizers(uint32_t *prevTime, float *timeCorrection);

xTaskHandle vtaskChannelsManagmentHandle;
static double offDoubleValue = NAN;
static SDRAM TickType_t TotalizerTime[2][100];

void vtaskChannelsManagment(void *pvParameters)
{
	int channelNumber;
	float DemoValue = 0.0;

	float TotalizerMultipler = 1.0;
	float TotalizerPeriod = 1.0;
	float TotalizersTimeCorrect = 1.0;
	float NewValue = 0;

	portTickType xLastExecutionTime;

	uint8_t FirstIterationFlag = 1;
	while (1)
	{
		if (STARTUP_WaitForBits(0x0008))
		{

			CHANNELS_Initialize();

			for (channelNumber = 0; channelNumber < 100; channelNumber++)
			{
				TotalizerTime[0][channelNumber] = xTaskGetTickCount();
				TotalizerTime[1][channelNumber] = xTaskGetTickCount();

			}

			xLastExecutionTime = xTaskGetTickCount();
			vTaskDelayUntil(&xLastExecutionTime, 100);

			while (1)
			{
				if (TakeChannelsMutex(100))
				{

					xQueueReset(xDataQueue);
					xQueueReset(xDataStatusQueue);
					xQueueReset(xDataResolution);
					xQueueReset(xTotalizerQueue);
					xQueueReset(xTotalizerResolution);

					for (channelNumber = 0; channelNumber < 100; channelNumber++)
					{
						//-------------------POMIAR-------------------
						switch (Channels[channelNumber].source.type)
						{
						case CH_SOURCE_MEASURMENT:

							if (ExpansionCards[Channels[channelNumber].source.board].status == SPI_BOARD_INACTIVE)
								Channels[channelNumber].failureState = DISABLED;
							else
								Channels[channelNumber].failureState =
										ExpansionCards[Channels[channelNumber].source.board].channel[Channels[channelNumber].source.number].status;

							if (Channels[channelNumber].failureState == 1)
							{
								NewValue = CalculateCharacteristicValue(&Channels[channelNumber], &ExpansionCards[Channels[channelNumber].source.board]);

								if (Channels[channelNumber].failureState == 1)
									Channels[channelNumber].value = CalculateFilteredValue(&Channels[channelNumber], &NewValue);
								else
								{
									if (Channels[channelNumber].failureMode == 1)
										Channels[channelNumber].value = Channels[channelNumber].failureValue;
									else
										Channels[channelNumber].value = NAN;
								}
							}
							else
							{
								if (Channels[channelNumber].failureMode == 1)
									Channels[channelNumber].value = Channels[channelNumber].failureValue;
								else
									Channels[channelNumber].value = NAN;
							}
							break;

							/* --------------------------- KANAL OBLICZENIOWY ---------------------- */
						case CH_SOURCE_COMPUTE:
							Channels[channelNumber].failureState = 1;
							if (Channels[channelNumber].firstIterration == 0)
							{
								NewValue = computeRPN(Channels, channelNumber);
								if (Channels[channelNumber].failureState == 1)
								{
									NewValue = CalculateCharacteristicValueCompute(&Channels[channelNumber], NewValue);
									if (isnan(NewValue) || isinf(NewValue))
										Channels[channelNumber].failureState = 3;
								}
							}

							if (Channels[channelNumber].failureState == 1)
								Channels[channelNumber].value = CalculateFilteredValue(&Channels[channelNumber], &NewValue);
							else
							{
								if (Channels[channelNumber].failureMode == 1)
									Channels[channelNumber].value = Channels[channelNumber].failureValue;
								else
									Channels[channelNumber].value = NAN;
							}
							break;
						case CH_SOURCE_REMOTE:
							if(MBTCP_ERROR_REG_DISABLED == ModbusTCPregisters[Channels[channelNumber].source.number].error)
								Channels[channelNumber].failureState = DISABLED;
							else
								Channels[channelNumber].failureState = ModbusTCPregisters[Channels[channelNumber].source.number].error;

							if (Channels[channelNumber].failureState == OK)
							{
								NewValue = CalculateDemoCharacteristicValue(&Channels[channelNumber], ModbusTCPregisters[Channels[channelNumber].source.number].registerValue);
								Channels[channelNumber].value = CalculateFilteredValue(&Channels[channelNumber], &NewValue);
							}
							else
							{
								if (Channels[channelNumber].failureMode == 1)
									Channels[channelNumber].value = Channels[channelNumber].failureValue;
								else
									Channels[channelNumber].value = NAN;
							}
							break;
						case CH_SOURCE_DEMO:
							Channels[channelNumber].failureState = 1;
							NewValue = CalculateDemoCharacteristicValue(&Channels[channelNumber], DemoValue);
							Channels[channelNumber].value = CalculateFilteredValue(&Channels[channelNumber], &NewValue);
							break;

						default: // kanał wyłączony
							break;
						}

						if (Channels[channelNumber].source.type != CH_SOURCE_OFF)
						{
							//-------------------LICZNIKI-------------------
							if (Channels[channelNumber].source.type == 1 && ExpansionCards[Channels[channelNumber].source.board].type == SPI_CARD_IN6D
									&& ExpansionCards[Channels[channelNumber].source.board].settings.IOcard.channels[Channels[channelNumber].source.number].type == 3)
							{
								if (Channels[channelNumber].Tot1.type != 0 && 0 == isnanf(Channels[channelNumber].value))
								{
									TotalizerMultipler = SetTotalizerMultiplerValue(Channels[channelNumber].Tot1.multipler);
									Channels[channelNumber].Tot1.value += (double) (TotalizerMultipler * Channels[channelNumber].LowScaleValue * ExpansionCards[Channels[channelNumber].source.board].channel[Channels[channelNumber].source.number].diffValue );
									FRAM_WriteMultiple(START_COUNTER_ADDR + 16 * channelNumber, (uint8_t*) &Channels[channelNumber].Tot1.value, 8);
								}
								if (Channels[channelNumber].Tot2.type != 0 && 0 == isnanf(Channels[channelNumber].value))
								{
									TotalizerMultipler = SetTotalizerMultiplerValue(Channels[channelNumber].Tot2.multipler);
									Channels[channelNumber].Tot2.value += (double) (TotalizerMultipler * Channels[channelNumber].LowScaleValue * ExpansionCards[Channels[channelNumber].source.board].channel[Channels[channelNumber].source.number].diffValue );
									FRAM_WriteMultiple(START_COUNTER_ADDR + 16 * channelNumber + 8, (uint8_t*) &Channels[channelNumber].Tot2.value, 8);
								}
							}
							else
							{
								if (NumberOfHistoryItems > 0)
									SetTimeCorrectionForTotalizers(&TotalizerTime[0][channelNumber], &TotalizersTimeCorrect);
								else
									TotalizersTimeCorrect = 1;

								if (Channels[channelNumber].Tot1.type != 0 && 0 == isnanf(Channels[channelNumber].value))
								{
									TotalizerMultipler = SetTotalizerMultiplerValue(Channels[channelNumber].Tot1.multipler);
									TotalizerPeriod = SetTotalizerPeriodValue(Channels[channelNumber].Tot1.period);
									Channels[channelNumber].Tot1.value = Channels[channelNumber].Tot1.value
											+ (double) (TotalizerMultipler * (Channels[channelNumber].value / TotalizerPeriod) * TotalizersTimeCorrect);
									FRAM_WriteMultiple(START_COUNTER_ADDR + 16 * channelNumber, (uint8_t*) &Channels[channelNumber].Tot1.value, 8);
								}
								if (Channels[channelNumber].Tot2.type != 0 && 0 == isnanf(Channels[channelNumber].value))
								{
									TotalizerMultipler = SetTotalizerMultiplerValue(Channels[channelNumber].Tot2.multipler);
									TotalizerPeriod = SetTotalizerPeriodValue(Channels[channelNumber].Tot2.period);
									Channels[channelNumber].Tot2.value = Channels[channelNumber].Tot2.value
											+ (double) (TotalizerMultipler * (Channels[channelNumber].value / TotalizerPeriod) * TotalizersTimeCorrect);
									FRAM_WriteMultiple(START_COUNTER_ADDR + 16 * channelNumber + 8, (uint8_t*) &Channels[channelNumber].Tot2.value, 8);
								}
							}
							if (Channels[channelNumber].failureState == 1 || Channels[channelNumber].failureMode == 1)
							{
								HIST_DATA_AddNew(Channels[channelNumber].histData, Channels[channelNumber].value);

								if (Channels[channelNumber].value < Channels[channelNumber].min)
								{
									Channels[channelNumber].min = Channels[channelNumber].value;
									FRAM_WriteMultiple(START_MINIMUM_ADDR + 4 * channelNumber, (uint8_t*) &Channels[channelNumber].min, 4);
								}
								if (Channels[channelNumber].value > Channels[channelNumber].max)
								{
									Channels[channelNumber].max = Channels[channelNumber].value;
									FRAM_WriteMultiple(START_MAXIMUM_ADDR + 4 * channelNumber, (uint8_t*) &Channels[channelNumber].max, 4);
								}
							}
							else
								HIST_DATA_AddNew(Channels[channelNumber].histData, 0x7FFF);
						}

						if (Channels[channelNumber].ToArchive == 1)
						{
							if (Channels[channelNumber].failureMode == 1)
								xQueueSend(xDataStatusQueue, (void * ) &Channels[channelNumber].failureMode, 0);
							else
								xQueueSend(xDataStatusQueue, (void * ) &Channels[channelNumber].failureState, 0);

							xQueueSend(xDataQueue, (void * ) &Channels[channelNumber].value, 0);
							xQueueSend(xDataResolution, (void * ) &Channels[channelNumber].resolution, 0);
						}

						if (Channels[channelNumber].Tot1.ToArchive != 0)
						{
							if (Channels[channelNumber].source.type == 0)
								xQueueSend(xTotalizerQueue, (void * ) &offDoubleValue, 0);
							else
							{
								xQueueSend(xTotalizerQueue, (void * ) &Channels[channelNumber].Tot1.value, 0);
								xQueueSend(xTotalizerResolution, (void * ) &Channels[channelNumber].Tot1.resolution, 0);
							}
						}

						if (Channels[channelNumber].Tot2.ToArchive != 0)
						{
							if (Channels[channelNumber].source.type == 0)
								xQueueSend(xTotalizerQueue, (void * ) &offDoubleValue, 0);
							else
							{
								xQueueSend(xTotalizerQueue, (void * ) &Channels[channelNumber].Tot2.value, 0);
								xQueueSend(xTotalizerResolution, (void * ) &Channels[channelNumber].Tot2.resolution, 0);
							}
						}
					}

					UpdateOutputExpansionCardsValues();
					ResetIN6DCardsValues();

					DemoValue = UpdateDemoValue();

					if (NumberOfHistoryItems < 360 && FirstIterationFlag == 0)
						NumberOfHistoryItems++;

					GiveChannelsMutex();
					WDFlags[2] = ALIVE;
					vTaskDelayUntil(&xLastExecutionTime, 1000);
				}
				else
				{
					WDFlags[2] = ALIVE;
					vTaskDelay(10);
				}

				if (FirstIterationFlag)
				{
					FirstIterationFlag = 0;
					STARTUP_ClaerBits(0x0008);
					STARTUP_SetBits(0x0010);
				}
			}
		}
		else
			continue;
	}
}

void CreateChannelsTask(void)
{
	xTaskCreate(vtaskChannelsManagment, "vtaskChannelsManagment", 1000, NULL, (unsigned portBASE_TYPE ) 5, &vtaskChannelsManagmentHandle);
}

static float UpdateDemoValue(void)
{
	static float angle = 0.0;
	angle = angle + 2.0;
	if (angle >= 360)
	{
		angle = 0.0;
	}
	return sin((angle * 3.14159 / 180));
}

static void SetTimeCorrectionForTotalizers(uint32_t *prevTime, float *timeCorrection)
{
	uint32_t actualTime = xTaskGetTickCount();
	*timeCorrection = (float) (actualTime - *prevTime) / 1000;
	*prevTime = actualTime;
}


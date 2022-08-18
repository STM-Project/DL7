/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include "stm32h7xx_hal.h"
#include <stdlib.h>
#include "shift_reg.h"

#define SECONDS_PER_WEEK					604800
#define QUARTZ_FREQ_HZ		 				32768
#define MAX_CALR_VALUE			 			511
#define CONSTANT_FOR_32S_WINDOW	 	512
#define TIME_WINDOW_S		      		32
#define MAX_TIME_CORRECTION_VALUE	294

#define RECALPF_TIMEOUT    ((uint32_t) 0x00001000)

RTC_TimeTypeDef RTCTime;
RTC_DateTypeDef RTCDate;
static uint8_t DSTFlag;
static uint8_t localDSTmode;
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static void RTC_SetDST(uint32_t RTC_DSTOperation)
{
	hrtc.Instance->WPR = 0xCA;
	hrtc.Instance->WPR = 0x53;
	hrtc.Instance->CR &= (uint32_t) ~(RTC_CR_BKP);
	hrtc.Instance->CR |= RTC_DSTOperation;
	hrtc.Instance->WPR = 0xFF;
}

static uint8_t RTC_GetDST(void)
{
 return ((hrtc.Instance->CR & RTC_STOREOPERATION_SET ) && RTC_STOREOPERATION_SET);
}

static uint8_t CheckDateForDST(RTC_TimeTypeDef *RTCTime, RTC_DateTypeDef *RTCDate)
{
	uint8_t month = RTCDate->Month;
	uint8_t weekDay;
	if (RTCDate->WeekDay == 7)
		weekDay = 0;
	else
		weekDay = RTCDate->WeekDay;

	if (month < 3 || month > 10)
		return 0;
	else if (month == 10)
	{
		if (RTCDate->Date - weekDay >= 25 && (weekDay || RTCTime->Hours >= 3))
			return 0;
		else
			return 1;
	}
	else if (month == 3)
	{
		if (RTCDate->Date - weekDay >= 25 && (weekDay || RTCTime->Hours >= 2))
			return 1;
		else
			return 0;
	}
	else
		return 1;
}

static void DST_Update(void)
{
	HAL_RTC_GetTime(&hrtc, &RTCTime, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTCDate, FORMAT_BIN);

	if (CheckDateForDST(&RTCTime, &RTCDate))
	{
		RTC_SetDST(RTC_STOREOPERATION_SET);
		DSTFlag = 1;
	}
	else
	{
		RTC_SetDST(RTC_STOREOPERATION_RESET);
		DSTFlag = 0;
	}
}

static void DST_ChangeToWinterTime(void)
{
	RTC_SetDST(RTC_STOREOPERATION_RESET|RTC_DAYLIGHTSAVING_SUB1H);
	DSTFlag = 0;
}

static void DST_ChangeToSummerTime(void)
{
	RTC_SetDST(RTC_STOREOPERATION_SET|RTC_DAYLIGHTSAVING_ADD1H);
	DSTFlag = 1;
}

static int CalculateWeekDayFromDate(int day, int month,int year)
{
	return (day += month < 3 ? year-- : year - 2, 23 * month / 9 + day + 4 + year / 4 - year / 100 + year / 400) % 7;
}

static uint8_t GetWeekDay(int dd, int mm, int yyyy)
{
	uint8_t WeekDay;
	WeekDay = (uint8_t)CalculateWeekDayFromDate(dd, mm, yyyy);
	if (WeekDay == 0)
		return 7;
	else
		return WeekDay;
}

void RTC_Initialize(void)
{
	MX_RTC_Init();

	HAL_RTC_GetTime(&hrtc, &RTCTime, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTCDate, FORMAT_BIN);

	if ((RTCDate.Year < 21) || (RTCDate.Year > 90))
	{
		RTCTime.Hours = 0;
		RTCTime.Minutes = 0;
		RTCTime.Seconds = 0;
		RTCTime.SubSeconds = 0;
		RTCTime.TimeFormat = RTC_HOURFORMAT_24;
		RTCTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		RTCTime.StoreOperation = RTC_STOREOPERATION_RESET;
		RTCDate.Year = 21;
		RTCDate.Month = 1;
		RTCDate.Date = 1;
		RTCDate.WeekDay = 5;

		HAL_RTC_SetDate(&hrtc, &RTCDate, FORMAT_BIN);
		HAL_RTC_SetTime(&hrtc, &RTCTime, FORMAT_BIN);

	}
}

void DST_Initialize(uint8_t DSTMode)
{
	if(DSTMode == 1)
	{
		localDSTmode = 1;
		HAL_RTC_GetTime(&hrtc, &RTCTime, FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &RTCDate, FORMAT_BIN);

		uint8_t registerDST = RTC_GetDST();
		uint8_t calculateDST = CheckDateForDST(&RTCTime, &RTCDate);

		if(registerDST != calculateDST)
		{
			if(calculateDST == 1)
				DST_ChangeToSummerTime();
			else
				DST_ChangeToWinterTime();
		}
		else
			DSTFlag = registerDST;
	}
	else
	{
		DSTFlag = 0;
		localDSTmode = 0;
	}
}

void RTC_GetTimeAndDate(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
	HAL_RTC_GetTime(&hrtc, sTime, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, sDate, FORMAT_BIN);
}

void RTC_SetTime(RTC_TimeTypeDef *sTime)
{
	RTCTime.Hours = sTime->Hours;
	RTCTime.Minutes = sTime->Minutes;
	RTCTime.Seconds = sTime->Seconds;
	RTCTime.SubSeconds = 0;
	RTCTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	RTCTime.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&hrtc, &RTCTime, FORMAT_BIN);
	if(localDSTmode)
		DST_Update();
	else
		DSTFlag = 0;
}

void RTC_SetDate(RTC_DateTypeDef *sDate)
{
	sDate->WeekDay = GetWeekDay(sDate->Date, sDate->Month, sDate->Year +2000);
	HAL_RTC_SetDate(&hrtc, sDate, FORMAT_BIN);
	if(localDSTmode)
		DST_Update();
	else
		DSTFlag = 0;
}

uint8_t DST_GetFlagState(void)
{
	return DSTFlag;
}

void DST_AutoChange(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
	uint8_t DST = RTC_GetDST();
	uint8_t month = sDate->Month;
	uint8_t weekDay;
	if (sDate->WeekDay == 7)
		weekDay = 0;
	else
		weekDay = sDate->WeekDay;

	if (month == 10 && sDate->Date >= 25 && weekDay == 0 && sTime->Hours >= 3 && DST != 0)
		DST_ChangeToWinterTime();

	else if (month==3 && sDate->Date >= 25 && weekDay == 0 && sTime->Hours >= 2 && DST != 1)
		DST_ChangeToSummerTime();
}

/********************************************************************************************************************************/
static uint32_t secondsToTicks(int32_t seconds, int32_t windowSize)
{
	float ticks = abs((float)seconds);

	ticks *=QUARTZ_FREQ_HZ;
	ticks *=windowSize;
	ticks /= SECONDS_PER_WEEK;
	ticks=ceil(ticks);

	if (ticks > MAX_CALR_VALUE)
		ticks = MAX_CALR_VALUE;
	return (uint32_t)ticks;
}

void RTC_Calibration(int seconds) // seconds - sekundy do wykalibrowania w skali tygodnia
{
	uint32_t RTC_SmoothCalibPeriod;
	uint32_t RTC_SmoothCalibPlusPulses;
	uint32_t RTC_SmoothCalibMinusPulsesValue;
	int32_t absolutSeconds = 0;

	if (seconds > 0)
	{
		RTC_SmoothCalibPlusPulses = RTC_SMOOTHCALIB_PLUSPULSES_SET;
		if (seconds > MAX_TIME_CORRECTION_VALUE)
			seconds = MAX_TIME_CORRECTION_VALUE;

		RTC_SmoothCalibPeriod = RTC_SMOOTHCALIB_PERIOD_32SEC;
		RTC_SmoothCalibMinusPulsesValue = CONSTANT_FOR_32S_WINDOW - secondsToTicks(seconds, TIME_WINDOW_S);

	}
	else
	{
		RTC_SmoothCalibPlusPulses = RTC_SMOOTHCALIB_PLUSPULSES_RESET;
		absolutSeconds = -seconds;
		if(absolutSeconds > MAX_TIME_CORRECTION_VALUE)
			absolutSeconds = MAX_TIME_CORRECTION_VALUE;

		RTC_SmoothCalibPeriod = RTC_SMOOTHCALIB_PERIOD_32SEC;
		RTC_SmoothCalibMinusPulsesValue = secondsToTicks(absolutSeconds, TIME_WINDOW_S);
	}

	RTC_SmoothCalibConfig(RTC_SmoothCalibPeriod, RTC_SmoothCalibPlusPulses, RTC_SmoothCalibMinusPulsesValue);
}

ErrorStatus RTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod, uint32_t RTC_SmoothCalibPlusPulses, uint32_t RTC_SmoothCalibMinusPulsesValue)
{
	ErrorStatus status = ERROR;
	uint32_t recalpfcount = 0;
	int error;
	int sign = 1;
	int correction = 1;
	uint32_t checkRegisterPrevious;
	uint32_t checkRegisterNext;

	/* Check the parameters */
	assert_param(IS_RTC_SMOOTH_CALIB_PERIOD(RTC_SmoothCalibPeriod));
	assert_param(IS_RTC_SMOOTH_CALIB_PLUS(RTC_SmoothCalibPlusPulses));
	assert_param(IS_RTC_SMOOTH_CALIB_MINUS(RTC_SmoothCalibMinusPulsesValue));

	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	/* check if a calibration is pending*/
	if ((RTC->ISR & RTC_ISR_RECALPF) != RESET)
	{
		/* wait until the Calibration is completed*/
		while (((RTC->ISR & RTC_ISR_RECALPF) != RESET) && (recalpfcount != RECALPF_TIMEOUT))
		{
			recalpfcount++;
		}
	}

	/* check if the calibration pending is completed or if there is no calibration operation at all*/
	if ((RTC->ISR & RTC_ISR_RECALPF) == RESET)
	{
		/* Configure the Smooth calibration settings */
		RTC->CALR = (uint32_t) ((uint32_t) RTC_SmoothCalibPeriod | (uint32_t) RTC_SmoothCalibPlusPulses | (uint32_t) RTC_SmoothCalibMinusPulsesValue);
		/* Correction of register value; error occurred because of 8s calibration window */
		checkRegisterPrevious = RTC->CALR;
		uint32_t checkValue = (uint32_t) ((uint32_t) RTC_SmoothCalibPeriod | (uint32_t) RTC_SmoothCalibPlusPulses | (uint32_t) RTC_SmoothCalibMinusPulsesValue);
		if (checkValue != checkRegisterPrevious)
		{
			error = checkValue - checkRegisterPrevious;
			sign = (error > 0) ? 1 : ((error < 0) ? -1 : 1);
			RTC->CALR = RTC->CALR + sign * correction;
			checkRegisterNext = RTC->CALR;
			while ((checkRegisterNext == checkRegisterPrevious) && (2 * abs(error)) >= correction)
			{
				correction++;
				RTC->CALR = RTC->CALR + sign * correction;
				checkRegisterNext = RTC->CALR;
			}
		}
		status = SUCCESS;
	}
	else
		status = ERROR;

	/* Enable the write protection for RTC registers */
	RTC->WPR = 0xFF;
	return (ErrorStatus) (status);
}

float RTC_GetSecondsCorrection(void)
{
	int32_t timeCorrectionWindow = 0;
	uint32_t timeCorrection = RTC->CALR;
	int32_t timeCorrectionPulses = 0;

	float ret =0;

	uint32_t calp = 0x8000 & timeCorrection;
	uint32_t calm = 0x1FF & timeCorrection;

	if(0x8000 == calp)
			timeCorrectionPulses = CONSTANT_FOR_32S_WINDOW-calm;
	else
		timeCorrectionPulses = -calm;

	timeCorrectionWindow = QUARTZ_FREQ_HZ * TIME_WINDOW_S;

	ret = timeCorrectionPulses * SECONDS_PER_WEEK;
	ret /= timeCorrectionWindow;

	return ret;
}
/* USER CODE END 1 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   This file contains all the function prototypes for
  *          the rtc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */
extern RTC_TimeTypeDef RTCTime;
extern RTC_DateTypeDef RTCDate;
/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */

void RTC_Initialize(void);
void DST_Initialize(uint8_t DSTMode);
void DST_AutoChange(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);

void RTC_GetTimeAndDate(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
void RTC_SetTime(RTC_TimeTypeDef *sTime);
void RTC_SetDate(RTC_DateTypeDef *sDate);
uint8_t DST_GetFlagState(void);

void RTC_Calibration(int seconds);
ErrorStatus RTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod, uint32_t RTC_SmoothCalibPlusPulses, uint32_t RTC_SmouthCalibMinusPulsesValue);
float RTC_GetSecondsCorrection(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */


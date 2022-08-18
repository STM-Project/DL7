/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEMUX_A2_Pin GPIO_PIN_2
#define DEMUX_A2_GPIO_Port GPIOE
#define REG_STR1_Pin GPIO_PIN_13
#define REG_STR1_GPIO_Port GPIOC
#define DEMUX_A1_Pin GPIO_PIN_11
#define DEMUX_A1_GPIO_Port GPIOI
#define REG_OE_Pin GPIO_PIN_6
#define REG_OE_GPIO_Port GPIOF
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOA
#define POWER_FAIL_Pin GPIO_PIN_1
#define POWER_FAIL_GPIO_Port GPIOB
#define POWER_FAIL_EXTI_IRQn EXTI1_IRQn
#define DEMUX_A0_Pin GPIO_PIN_6
#define DEMUX_A0_GPIO_Port GPIOH
#define REG_CLK_Pin GPIO_PIN_1
#define REG_CLK_GPIO_Port GPIOI
#define REG_DATA_Pin GPIO_PIN_3
#define REG_DATA_GPIO_Port GPIOI
#define RS2_RTS_Pin GPIO_PIN_12
#define RS2_RTS_GPIO_Port GPIOG
#define DEMUX_E_Pin GPIO_PIN_6
#define DEMUX_E_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart6;

/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_9B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_EVEN;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart6, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart6, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART6;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PG9     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PG9     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

    /* USART6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
unsigned char MODBUS_UartInit(uint32_t baudRate, short parity)
{
	unsigned char bStatus;

	huart6.Instance = USART6;
	huart6.Init.BaudRate = baudRate;
	huart6.Init.StopBits = UART_STOPBITS_1;
	switch (parity)
	{
	case 0:
		huart6.Init.WordLength = UART_WORDLENGTH_9B;
		huart6.Init.Parity = UART_PARITY_ODD;
		break;
	case 1:
		huart6.Init.WordLength = UART_WORDLENGTH_9B;
		huart6.Init.Parity = UART_PARITY_EVEN;
		break;
	case 2:
		huart6.Init.WordLength = UART_WORDLENGTH_8B;
		huart6.Init.Parity = UART_PARITY_NONE;
		break;
	}
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart6) != HAL_OK)
	{
		bStatus = 0;
	}

	__HAL_UART_DISABLE_IT(&huart6, UART_IT_TXE);
	__HAL_UART_DISABLE_IT(&huart6, UART_IT_RXNE);
	bStatus = 1;
	return bStatus;
}

void MODBUS_EnableRxIT(void)
{
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
}

void MODBUS_EnableTxIT(void)
{
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_TXE);
}

void MODBUS_DisableRxIT(void)
{
	__HAL_UART_DISABLE_IT(&huart6, UART_IT_RXNE);
}

void MODBUS_DisableTxIT(void)
{
	__HAL_UART_DISABLE_IT(&huart6, UART_IT_TXE);
}

void MODBUS_SendByte(uint8_t *pByte)
{
	HAL_UART_Transmit(&huart6, pByte, 1, 10);
}

void MODBUS_RecvByte(uint8_t *pByte)
{
	HAL_UART_Receive(&huart6, pByte, 1, 10);
}

void MODBUS_PortSerialClose(void)
{
	/*##-1- Reset peripherals ##################################################*/
	__USART6_FORCE_RESET();
	__USART6_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks #################################*/
	/* Configure UART Tx as alternate function  */
	HAL_GPIO_DeInit(GPIOC, USART6_IRQn);
	/* Configure UART Rx as alternate function  */
	HAL_GPIO_DeInit(GPIOG, USART6_IRQn);

	/*##-3- Disable the NVIC for UART ##########################################*/
	HAL_NVIC_DisableIRQ(USART6_IRQn);
}

/* USER CODE END 1 */

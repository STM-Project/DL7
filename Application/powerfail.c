#include <powerfail.h>
#include "stm32h7xx_hal.h"
#include "shift_reg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "fram.h"
#include "backlight.h"
#include "rtc.h"

static int powerfailFlag = 0;
static GPIO_PinState PowerFailState;
xTaskHandle vtaskPowerFailHandle;

void POWERFAIL_CheckState(void)
{
	while (HAL_GPIO_ReadPin(POWER_FAIL_GPIO_Port, POWER_FAIL_Pin) == 0)
	{
		HAL_Delay(1);
	}
}

void vtaskPowerFail(void *pvParameters)
{
	RTC_TimeTypeDef PowerFailTime;
	RTC_DateTypeDef PowerFailDate;

	uint8_t tampBacklight = BACKLIGHT_Get();

	vTaskDelay(10);

	PowerFailState = HAL_GPIO_ReadPin(POWER_FAIL_GPIO_Port,POWER_FAIL_Pin);

	if(!PowerFailState)
	{
		BACKLIGHT_Set(0);
		BLUE_LED_OFF;
		RED_LED_OFF;

		RTC_GetTimeAndDate(&PowerFailTime, &PowerFailDate);
		FRAM_Write(ARCH_RTC_HOUR, PowerFailTime.Hours);
		FRAM_Write(ARCH_RTC_MINUTES, PowerFailTime.Minutes);
		FRAM_Write(ARCH_RTC_SECONDS, PowerFailTime.Seconds);
		FRAM_Write(ARCH_RTC_DATE, PowerFailDate.Date);
		FRAM_Write(ARCH_RTC_MONTH, PowerFailDate.Month);
		FRAM_Write(ARCH_RTC_YEAR, PowerFailDate.Year);
		FRAM_Write(ARCH_RTC_DST, DST_GetFlagState());

		do
		{
			HAL_Delay(50);
			PowerFailState = HAL_GPIO_ReadPin(POWER_FAIL_GPIO_Port,POWER_FAIL_Pin);
		}while(!PowerFailState);

	}

	uint8_t framZeroes[6]={0};
	FRAM_WriteMultiple(ARCH_RTC_HOUR, framZeroes,6);
	FRAM_Write(ARCH_RTC_DST, 0);

	BACKLIGHT_Set(tampBacklight);
	powerfailFlag = 0;
	vTaskDelete(NULL);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(powerfailFlag == 0)
	{
		if( xTaskCreate(vtaskPowerFail, "vtaskPowerFail", 800, NULL, ( unsigned portBASE_TYPE )7, &vtaskPowerFailHandle) == pdPASS)
			powerfailFlag = 1;
	}
}


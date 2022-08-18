
#include "buzzer.h"
#include "stm32h7xx_hal.h"
#include "tim.h"
#include "parameters.h"

typedef struct {
  uint16_t State;
  uint16_t Buzz_timer;
}
BUZZER_StructTypeDef;

static BUZZER_StructTypeDef Buzzer;

void Buzzer_On(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin,GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin,GPIO_PIN_RESET);
}

void BUZZER_Init(void)
{
	MX_TIM14_Init();
	Buzzer.State = 0;
}

void BUZZER_Beep(void)
{
	if (GeneralSettings.BeeperMode)
	{
		if (!Buzzer.State)
		{
			Buzzer.State = 1;
			Buzzer.Buzz_timer = 0;
			Buzzer_On();
			HAL_TIM_Base_Start_IT(&htim14);
		}
	}
}

void BUZZER_BeepOffCounter(void)
{
	if (Buzzer.State)
	{
		if(Buzzer.Buzz_timer)
		{
			Buzzer_Off();
			Buzzer.State = 0;
			HAL_TIM_Base_Stop_IT(&htim14);
		}
		else
			Buzzer.Buzz_timer++;
	}
}

void TestBUZZER(void)
{
	for (int i = 0; i < 3; i++)
	{
		Buzzer_On();
		HAL_Delay(100);
		Buzzer_Off();
		HAL_Delay(500);
	}
}


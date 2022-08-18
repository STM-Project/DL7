/*
 * backlight.c
 *
 *  Created on: 12.11.2020
 *      Author: RafalMar
 */

#include "backlight.h"
#include "tim.h"
#include "shift_reg.h"

#define PWM_PERIOD_VALUE	 800

static uint32_t Pulse;

void BACKLIGHT_Init(void)
{
	MX_TIM1_Init();
	PWM_Start();
}

void BACKLIGHT_Set(int level)
{
  if (level <= 0)
  {
    BLUE_LED_ON;
    BACKLIGHT_OFF;
    Pulse = 0;
  }
  else if (level > 0 || level < 101)
  {
    BLUE_LED_OFF;
    BACKLIGHT_ON;
    Pulse = (uint32_t) level * PWM_PERIOD_VALUE / 100;
  }
  else if (level > 100)
  {
  	Pulse = PWM_PERIOD_VALUE + 1;
  	BACKLIGHT_ON;
  	BLUE_LED_ON;
  }


  PWM_Config(Pulse);
  PWM_Start();
}

uint8_t BACKLIGHT_Get(void){
	return (uint8_t) (100 * Pulse / PWM_PERIOD_VALUE);
}

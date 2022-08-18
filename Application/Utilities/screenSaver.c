/*
 * screenSaver.c
 *
 *  Created on: 27.01.2017
 *      Author: Tomaszs
 */

#include "screenSaver.h"
#include "FreeRTOS.h"
#include "timers.h"

#include "backlight.h"
#include "shift_reg.h"
#include "WM.h"

#define SCREEN_SAVER_TIME 60000
#define SCREEN_SAVER_ACTIVATED 1
#define SCREEN_SAVER_DEACTIVATED 0

struct ScreenSaver
{
  uint8_t init;
  uint8_t brightness;
  uint8_t type;
  uint8_t state;
  uint32_t time;
  TimerHandle_t xTimer;
};

struct ScreenSaver screenSaver;

void vScreenSaverCallback(TimerHandle_t pxTimer)
{
  xTimerStop(pxTimer, 0);
  screenSaver.state = SCREEN_SAVER_ACTIVATED;
  if (screenSaver.type == 0)
  {
  	BACKLIGHT_Set(0);
    WM_SetCapture(WM_HBKWIN, 0);
  }
  else if (screenSaver.type == 1)
  {
  	BACKLIGHT_Set(20);
    WM_SetCapture(WM_HBKWIN, 0);
  }
  else
  {
  	BACKLIGHT_Set(screenSaver.brightness);
    screenSaver.state = SCREEN_SAVER_DEACTIVATED;
  }
}

void ScreenSaverInit(unsigned char brightness, unsigned char Time, unsigned char Type)
{
	screenSaver.brightness = brightness;
	screenSaver.type = Type;
	screenSaver.time = Time * SCREEN_SAVER_TIME;
	if (screenSaver.time == 0)
	{
		screenSaver.state = SCREEN_SAVER_DEACTIVATED;
	    screenSaver.init = 0;
	}
	else
	{
		screenSaver.xTimer = xTimerCreate("ScreenSaverTimer", screenSaver.time, 0, ( void * )0,  vScreenSaverCallback);
    screenSaver.init = 1;
    screenSaver.state = SCREEN_SAVER_DEACTIVATED;
	}
}

void ScreenSaverDeInit(void)
{
	if( screenSaver.init == 1)
	{
		xTimerDelete(screenSaver.xTimer,0);
    screenSaver.state = SCREEN_SAVER_DEACTIVATED;
    screenSaver.init = 0;
	}
	else
	{
    screenSaver.state = SCREEN_SAVER_DEACTIVATED;
    screenSaver.init = 0;
	}
}

void RestartScreenSaver(unsigned char brightness, unsigned char Time, unsigned char Type)
{
	ScreenSaverDeInit();
	ScreenSaverInit( brightness, Time, Type);
}


void ResetScreenSaverTimer(void)
{
  if (screenSaver.init == 1)
  {
    if (screenSaver.state == SCREEN_SAVER_DEACTIVATED)
    {
      if (WM_HasCaptured(WM_HBKWIN))
        WM_ReleaseCapture();
      xTimerReset(screenSaver.xTimer, screenSaver.time);
    }
    else if (screenSaver.state == SCREEN_SAVER_ACTIVATED)
    {
      xTimerStart(screenSaver.xTimer, 0);
      xTimerReset(screenSaver.xTimer, screenSaver.time);
      BACKLIGHT_Set(screenSaver.brightness);
      screenSaver.state = SCREEN_SAVER_DEACTIVATED;
    }
  }
  else
  {
    if (WM_HasCaptured(WM_HBKWIN))
      WM_ReleaseCapture();
    screenSaver.state = SCREEN_SAVER_DEACTIVATED;
  }
}

void StartScreenSaverTimer(void)
{
	if( screenSaver.init == 1)
		xTimerStart(screenSaver.xTimer, 0);
}



#include <string.h>
#include "passwords.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "archive.h"
#include "fram.h"

#define FACTORY_PASS 	(const char*)"71539"
#define LOG_OUT_TIME_MS 	300000

static uint8_t AccessLevel;
static char UsersPasswords[USER_NUMBER][MAX_PASS_LENGHT] __attribute__ ((section(".sdram")));

char LoginMessage[30]__attribute__ ((section(".sdram")));

TimerHandle_t xLogOutTimer;

static char UserPasswordDissable;

void SetDefaultPasswords(int UserLevel)
{
	char PassBuffer[16]={0};
	switch (UserLevel)
	{
	case FACTORY_LEVEL:
		strncpy(PassBuffer,"3719",5);
		PASSWORDS_Change(2, &PassBuffer[0]);
		/* no break */
	case SERVICE_LEVEL:
		strncpy(PassBuffer,"1",2);
		PASSWORDS_Change(1, &PassBuffer[0]);
		/* no break */
	case ADMIN_LEVEL:
		strncpy(PassBuffer,"0",2);
		PASSWORDS_Change(0, &PassBuffer[0]);
		break;
	}
}

void PASSWORDS_ResetLogOutTimer(void)
{
	xTimerReset(xLogOutTimer, LOG_OUT_TIME_MS);
}

void vLogOutTimerCallback(TimerHandle_t pxTimer)
{
	PASSWORDS_Logout();
	xTimerStop(pxTimer, 0);
}

uint8_t PASSWORDS_Login(int user, char* password)
{
	uint8_t LogInStatus = 0;
	char LoginMessage[30];

	switch (user)
	{
	case USER:
		if (strcmp(password, UsersPasswords[0]) == 0)
		{
			AccessLevel = USER_LEVEL;
			LogInStatus = 1;
			strcpy(LoginMessage,"SYS:LOGIN:USER");
			ARCHIVE_SendEvent(LoginMessage);
		}
		break;
	case ADMIN:
		if (strcmp(password, UsersPasswords[1]) == 0)
		{
			AccessLevel = ADMIN_LEVEL;
			LogInStatus = 1;
			strcpy(LoginMessage,"SYS:LOGIN:ADMIN");
			ARCHIVE_SendEvent(LoginMessage);
		}
		break;
	case SERVICE:
		if (strcmp(password, UsersPasswords[2]) == 0)
		{
			AccessLevel = SERVICE_LEVEL;
			LogInStatus = 1;
			strcpy(LoginMessage,"SYS:LOGIN:SERVICE");
			ARCHIVE_SendEvent(LoginMessage);
		}
		break;
	case FACTORY:
		if (strcmp(password, UsersPasswords[3]) == 0)
		{
			AccessLevel = FACTORY_LEVEL;
			LogInStatus = 1;
			strcpy(LoginMessage,"SYS:LOGIN:FACTORY");
			ARCHIVE_SendEvent(LoginMessage);
		}
		break;
	}

	if (UserPasswordDissable)
	{
		if (AccessLevel > USER_LEVEL)
			xTimerStart(xLogOutTimer, 0);
	}
	else
	{
		if (AccessLevel > NO_USER)
			xTimerStart(xLogOutTimer, 0);
	}
	FRAM_Write(ACCESS_LEVEL_ADDR, AccessLevel);
	return LogInStatus;
}

uint8_t PASSWORDS_GetCurrentLevel(void)
{
	return AccessLevel;
}

uint8_t PASSWORDS_IsUserPasswordDisable(void)
{
	return UserPasswordDissable;
}

int PASSWORDS_Logout(void)
{
	if (UserPasswordDissable)
	{
		if(FRAM_Read(ACCESS_LEVEL_ADDR) != USER_LEVEL )
			AccessLevel = USER_LEVEL;
		else
			return 0;
	}
	else
	{
		if(FRAM_Read(ACCESS_LEVEL_ADDR) !=NO_USER)
			AccessLevel = NO_USER;
		else
			return 0;
	}
	FRAM_Write(ACCESS_LEVEL_ADDR, AccessLevel);
	strcpy(LoginMessage,"SYS:LOGOUT");
	ARCHIVE_SendEvent(LoginMessage);
	return 1;
}

void PASSWORDS_Init(void)
{
	xLogOutTimer = xTimerCreate("LogOutTimer", LOG_OUT_TIME_MS, 0, ( void * ) 0, vLogOutTimerCallback);

	FRAM_ReadMultiple(USER_PASSWORD_ADDR, (uint8_t *) &UsersPasswords[0], MAX_PASS_LENGHT);

	if (UsersPasswords[0][0] == '\0')
		UserPasswordDissable = 1;
	else
		UserPasswordDissable = 0;

	FRAM_ReadMultiple(ADMIN_PASSWORD_ADDR, (uint8_t *) &UsersPasswords[1], MAX_PASS_LENGHT);
	FRAM_ReadMultiple(SERVICE_PASSWORD_ADDR, (uint8_t *) &UsersPasswords[2], MAX_PASS_LENGHT);

	strncpy(UsersPasswords[3], FACTORY_PASS, MAX_PASS_LENGHT);
	AccessLevel = FRAM_Read(ACCESS_LEVEL_ADDR);

	if (UserPasswordDissable)
	{
		if (AccessLevel > USER_LEVEL)
			xTimerStart(xLogOutTimer, 0);
	}
	else
	{
		if (AccessLevel > NO_USER)
			xTimerStart(xLogOutTimer, 0);
	}
}

void PASSWORDS_Change(int user, char *newPassword)
{
	switch (user)
	{
	case 0:
		FRAM_WriteMultiple(USER_PASSWORD_ADDR, (uint8_t *) newPassword, MAX_PASS_LENGHT);
		FRAM_ReadMultiple(USER_PASSWORD_ADDR, (uint8_t *) &UsersPasswords[0], MAX_PASS_LENGHT);
		if (UsersPasswords[0][0] == '\0')
			UserPasswordDissable = 1;
		else
			UserPasswordDissable = 0;
		break;
	case 1:
		FRAM_WriteMultiple(ADMIN_PASSWORD_ADDR, (uint8_t *) newPassword, MAX_PASS_LENGHT);
		FRAM_ReadMultiple(ADMIN_PASSWORD_ADDR, (uint8_t *) &UsersPasswords[1], MAX_PASS_LENGHT);
		break;
	case 2:
		FRAM_WriteMultiple(SERVICE_PASSWORD_ADDR, (uint8_t *) newPassword, MAX_PASS_LENGHT);
		FRAM_ReadMultiple(SERVICE_PASSWORD_ADDR, (uint8_t *) &UsersPasswords[2], MAX_PASS_LENGHT);
		break;

	}
}

void PASSWORDS_RestoreDefault(void)
{
	PASSWORDS_Logout();
	PASSWORDS_Change(USER, "0");
	PASSWORDS_Change(ADMIN, "1");
	PASSWORDS_Change(SERVICE, "3719");
}

char* getUserPassword(void)
{
	return UsersPasswords[0];
}

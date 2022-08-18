/*
 * passwords.h
 *
 *  Created on: 16 kwi 2015
 *      Author: Tomaszs
 */
#ifndef PASSWORDS_H_
#define PASSWORDS_H_

#include <stdint.h>

#define USER_NUMBER       4
#define MAX_PASS_LENGHT   (uint16_t) 16

#define NO_USER           	0
#define USER              	1
#define USER_LEVEL        	10
#define ADMIN             	2
#define ADMIN_LEVEL       	20
#define SERVICE           	3
#define SERVICE_LEVEL     	50
#define FACTORY           	4
#define FACTORY_LEVEL     	99

uint8_t PASSWORDS_Login(int user, char* password);
int PASSWORDS_Logout(void);

uint8_t PASSWORDS_GetCurrentLevel(void);
uint8_t PASSWORDS_IsUserPasswordDisable(void);

void PASSWORDS_Init(void);
void PASSWORDS_Change(int user, char *newPassword);

void SetDefaultPasswords(int UserLevel);

void PASSWORDS_ResetLogOutTimer(void);

void PASSWORDS_RestoreDefault(void);

char* getUserPassword(void);

#endif /* PASSWORDS_H_ */

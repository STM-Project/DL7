/*
 * fram.h
 *
 *  Created on: 13.11.2020
 *      Author: RafalMar
 */

#ifndef FRAM_FRAM_H_
#define FRAM_FRAM_H_

#include "stdint.h"

/*-----------------------------------------------------
 FRAM ADDRESS USAGE
 -----------------------------------------------------*/
#define ACCESS_LEVEL_ADDR		 				0x0			//1 bajt
#define USER_PASSWORD_ADDR		 			0x01		//16 bajt�w
#define ADMIN_PASSWORD_ADDR		 			0x11		//16 bajt�w
#define SERVICE_PASSWORD_ADDR	 			0x21		//16 bajt�w
#define WINDOWS_STATE_ADDR        	0x31    // 1 bajt
#define WINDOW_INSTANCE_ADDR    		0x32    // 1 bajt
#define AUTO_CHANGE_ADDR          	0x33    // 1 bajt
#define USER_TABLE_SCREEN_ADDR  		0x34    // 1 bajt
#define USER_TREND_SCREEN_ADDR  		0x35    // 1 bajt
#define ARCHIVE_STATE_ADDR        	0x36    // 1 bajt
#define NEW_PARAMETERS_ADDR       	0x37    // 1 bajt
#define CURRENT_ARCH_ADDR         	0x38    // 1 bajt
#define ARCHIVE_TIME_ADDR         	0x39    // 3 bajty dla czasu
#define SKIN_ADDR                 	0x42    // 1 bajt
#define ARCH_RTC_HOUR             	0x43    // 1 bajt�w
#define ARCH_RTC_MINUTES          	0x44    // 1 bajt�w
#define ARCH_RTC_SECONDS          	0x45    // 1 bajt�w
#define ARCH_RTC_DATE             	0x46    // 1 bajt�w
#define ARCH_RTC_MONTH            	0x47    // 1 bajt�w
#define ARCH_RTC_YEAR             	0x48    // 1 bajt�w
#define SERIALNUMBER_ADDR						0x49		// 4 bajty
#define MAC_ADDRESS_ADDR						0x4D		// 6 bajt�w
#define ARCH_RTC_DST								0x53		// 1 bajt
#define READ_ARCHIVE_SELECTION			0x54		// 6 bytes
#define TOUCHPANEL_CAL_ADDR					0x5A	// 17 bytes

#define NO_OF_ACTIVE_CHANNELS_ADDR	0x0100	// 1 bajt
#define NEW_ARCHIVE_FLAG_ADDR				0x0101	// 1 bajt
#define START_COUNTER_ADDR        	0x1900  // 1600 bajt�w dla licznik�w
#define START_MINIMUM_ADDR        	0x1770  // 400 bajt�w dla warto�ci minimum
#define START_MAXIMUM_ADDR        	0x15E0  // 400 bajt�w dla warto�ci maximum
#define START_RESET_DATE_ADDR   		0x13FC  // DD/MM/YY + hh:mm 5 bajt�w x 100 = 500 bajt�w

void FRAM_Init(void);
void FRAM_Write(uint16_t Reg, uint8_t Value);
uint8_t FRAM_Read(uint16_t Reg);
void FRAM_WriteMultiple(uint16_t Reg, uint8_t *pBuffer,	uint16_t Length);
uint16_t FRAM_ReadMultiple(uint16_t Reg, uint8_t *pBuffer, uint16_t Length);

#endif /* FRAM_FRAM_H_ */

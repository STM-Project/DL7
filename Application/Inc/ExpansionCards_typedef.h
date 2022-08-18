/*
 * ExpansionCards_typedef.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef EXPANSIONCARDS_TYPEDEF_H_
#define EXPANSIONCARDS_TYPEDEF_H_


#include <stdint.h>

#define MAX_HART_SENSOR_FOR_CARD		15

#define SPI_CARD_EMPTY	 			0x00
#define SPI_CARD_IN6I24 			0x01
//#define SPI_CARD_IN3I24 		0x02
#define SPI_CARD_IN6I 				0x03
//#define SPI_CARD_IN3I 			0x04
#define SPI_CARD_IN6TC 				0x05
//#define SPI_CARD_IN3TC 			0x06
#define SPI_CARD_IN6V 				0x07
//#define SPI_CARD_IN3V 			0x08
#define SPI_CARD_IN6RTD 			0x09
#define SPI_CARD_IN3RTD 			0x0A
#define SPI_CARD_IN6 					0x0B
#define SPI_CARD_IN3 					0x0C
#define SPI_CARD_IN6D 				0x0D
#define SPI_CARD_OUT6RL 			0x0E
#define SPI_CARD_OUT3 				0x0F
#define SPI_CARD_2RS485 			0x10
#define SPI_CARD_2RS485I24 		0x11
#define SPI_CARD_1HRT 				0x12
#define SPI_CARD_PSBATT				0x13
#define SPI_CARD_IN4SG				0x14

#define SPI_BOARD_INACTIVE				0
#define SPI_BOARD_OK 							1
#define SPI_BOARD_ERROR 					2
#define SPI_BOARD_INCONFIG 				3

enum eCHStatus
{
	DISABLED = 0,        	  //kana� wy��cznony
	OK,									//praca normalna
	ERROR1,          	  //awaria (I<3.6mA  dla 4-20mA)
	ERROR2,          	  //awaria (I>22.0mA dla 0/4-20mA)
	RANGE,							//przekroczenie zakresu
	NO_SETTINGS = 10,		//brak ustawie�
	ADC_ERROR,					//uszkodzony przetwornik
	OTHER_ERR
};

typedef struct
{
	uint8_t functionCode;
	uint8_t byteCounter;
} SPI_FUNC_StructTypeDef;

typedef struct
{
	float A;
	float B;
} CALIBRATION;

typedef struct
{
	uint8_t hw_default :1;
	uint8_t calibrated :1;
	uint8_t fault :1;
	uint8_t reserved;
	uint8_t CalYear;
	uint8_t CalMonth;
	uint8_t CalDay;
	uint8_t CalHour;
	uint8_t CalMin;
	uint8_t CalSec;
	CALIBRATION Coefficient[6];
} CALIBRATION_StructTypeDef;

enum
{
	BOARD_A = 0, BOARD_B, BOARD_C, BOARD_D, BOARD_E, BOARD_F, BOARD_G
};

typedef struct
{
	uint8_t baudrate;
	uint8_t parity;
	uint8_t stopBits;
	uint16_t timeout;
} RS_PORT_SETTINGS;

typedef struct
{
	uint8_t port;
	uint8_t deviceAdr;
	uint32_t registerAdr;
	uint8_t registerType;
} RS_CHANNEL_SETTINGS;

typedef struct
{
	RS_PORT_SETTINGS port[2];
	RS_CHANNEL_SETTINGS channels[25];
} RS_CARD_SETTINGS;

typedef struct
{
	uint8_t type;
	float adjusment;
	float compensationConstantValue;
	int8_t compChannel;
} IO_CHANNEL_SETTINGS;

typedef struct
{
	uint8_t type;
	uint8_t source;
	uint8_t failureMode;
	float failureValue;
	float lowScaleValue;
	float highScaleValue;
} AO_CHANNEL_SETTINGS;

typedef struct
{
	AO_CHANNEL_SETTINGS channels[3];
} OUT_CARD_SETTINGS;

typedef struct
{
	uint8_t generalSet;
	IO_CHANNEL_SETTINGS channels[6];
} IO_CARD_SETTINGS;

typedef struct
{
	uint8_t activity;
	uint8_t hartRev;
	uint64_t address;
} HART_SENSOR_SETTINGS;

typedef struct
{
	uint8_t sensorIdx;
	uint8_t valueType;
	uint8_t commandType;
	uint8_t valueAddr;
	uint8_t statusMode;
} HART_READ_CHANNEL;

typedef struct
{
	uint8_t isSecondary;
	uint8_t isResistor;
	uint8_t preambleLenght;
	HART_READ_CHANNEL readChannel[25];
	HART_SENSOR_SETTINGS sensors[MAX_HART_SENSOR_FOR_CARD];
} HART_CARD_SETTINGS;

typedef union
{
	IO_CARD_SETTINGS IOcard;
	RS_CARD_SETTINGS RScard;
	OUT_CARD_SETTINGS OUTcard;
	HART_CARD_SETTINGS HARTcard;
} EXPANSION_CARD_SETTINGS;

typedef struct
{
	uint8_t status;
	float value;
	float diffValue;
} CARD_CHANNEL;

typedef struct
{
	uint8_t status;
	uint8_t type;
	uint16_t firmwareRevision;
	uint8_t CommunicationErrorCounter;
	EXPANSION_CARD_SETTINGS settings;
	CARD_CHANNEL channel[25];
} EXPANSION_CARD;

#endif /* EXPANSIONCARDS_TYPEDEF_H_ */

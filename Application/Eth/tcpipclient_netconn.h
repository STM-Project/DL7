/*
 * tcpipclient_netconn.h
 *
 *  Created on: 11.05.2018
 *      Author: Tomaszs
 */

#ifndef ETH_TCPIPCLIENT_NETCONN_H_
#define ETH_TCPIPCLIENT_NETCONN_H_

#include <stdint.h>

#define MAX_MODBUS_TCP_CONNECTIONS 	20
#define MAX_MODBUS_TCP_REGISTERS 		100

#define MBTCP_ERROR_REG_DISABLED 		0x00
#define MBTCP_ERROR_OK 							0x01
#define MBTCP_ERROR_MODBUS_CODE 		0x02
#define MBTCP_ERROR_TIMEOUT 				0x03
#define MBTCP_ERROR_RANGE						0x04
#define MBTCP_ERROR_CONNETING				0x0A
#define MBTCP_ERROR_OTHER						0x20
#define MBTCP_ERROR_NO_CONNECTION		0x21

typedef struct
{
	uint8_t connectionID;
	uint8_t mode;
	uint8_t ip[4];
	uint16_t port;
	uint16_t responseTimeout;
	uint16_t frequency;
	int8_t state;
}MODBUS_TCP_SERVER;

enum MODBUS_REGISTER_TYPE
{
	UINT_16_BIT=0,
	INT_16_BIT,
	UINT_32_BIT,
	UINT_32_BIT_SW,
	INT_32_BIT,
	INT_32_BIT_SW,
	FLOAT_32_BIT,
	FLOAT_32_BIT_SW,
	INT_64_BIT,
	DOUBLE_64_BIT
};

enum MODBUS_FUNCTION
{
	FUNCTION_NONE,
	READ_HOLDING_REG = 0x03,
	READ_INPUT_REG = 0x04
};

typedef struct
{
	uint8_t connectionID;
	uint8_t deviceAddress;
	uint8_t prevError;
	uint8_t error;
	enum MODBUS_FUNCTION function;
	uint16_t registerAddress;
	uint32_t number;
	enum MODBUS_REGISTER_TYPE registerType;
	float registerValue;
	uint8_t length;
} MODBUS_TCP_REGISTER;

extern MODBUS_TCP_REGISTER ModbusTCPregisters[MAX_MODBUS_TCP_REGISTERS];
extern MODBUS_TCP_REGISTER bkModbusTCPregisters[MAX_MODBUS_TCP_REGISTERS];

extern MODBUS_TCP_SERVER MBserver[MAX_MODBUS_TCP_CONNECTIONS];
extern MODBUS_TCP_SERVER bkMBserver[MAX_MODBUS_TCP_CONNECTIONS];

void MBTCP_Initialize(void);

void MBTCP_InitDefaultServers(void);
void MBTCP_InitDefaultRegisters(void);

#endif /* ETH_TCPIPCLIENT_NETCONN_H_ */

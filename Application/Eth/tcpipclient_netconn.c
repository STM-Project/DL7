/*
 * tcpipclient_netconn.c
 *
 *  Created on: 11.05.2018
 *      Author: Tomaszs
 */

#include "tcpipclient_netconn.h"

#include <ctype.h>
#include <math.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mini-printf.h"
#include "parameters.h"

#ifndef SDRAM
#define SDRAM __attribute__ ((section(".sdram")))
#endif

#define MBTCP_CONNECT 				0
#define MBTCP_DISCONNECT 			1
#define MBTCP_COMMUNICATION		2
#define MBTCP_WAIT			 			3

#define MBTCP_RECONNECT_DELAY 60000

#define MBTCP_SERVER_OFF 0
#define MBTCP_SERVER_GROUP 26

#define MODBUS_TCP_REQUEST_LENGTH 	12

#define MBAP_LENGTH_OFFSET 6

static uint16_t TransactionID = 0;
SDRAM MODBUS_TCP_REGISTER ModbusTCPregisters[MAX_MODBUS_TCP_REGISTERS]={0};
MODBUS_TCP_SERVER MBserver[MAX_MODBUS_TCP_CONNECTIONS] = {0};

SDRAM MODBUS_TCP_REGISTER bkModbusTCPregisters[MAX_MODBUS_TCP_REGISTERS]={0};
SDRAM MODBUS_TCP_SERVER bkMBserver[MAX_MODBUS_TCP_CONNECTIONS]={0};

static void MBTCP_GroupRegisters(void)
{
	int groupStartAddress = -1;
	for (int i = 1; i < MAX_MODBUS_TCP_REGISTERS; i++)
	{
		if (ModbusTCPregisters[i].connectionID != MBTCP_SERVER_OFF)
		{
			if ((ModbusTCPregisters[i].deviceAddress == ModbusTCPregisters[i - 1].deviceAddress)
					&& (ModbusTCPregisters[i].connectionID == ModbusTCPregisters[i - 1].connectionID)
					&& (ModbusTCPregisters[i].function == ModbusTCPregisters[i - 1].function)
					&& (ModbusTCPregisters[i].registerAddress == (ModbusTCPregisters[i - 1].registerAddress + ModbusTCPregisters[i - 1].length)))
			{
				if (groupStartAddress == -1)
					groupStartAddress = i - 1;
				ModbusTCPregisters[groupStartAddress].length += ModbusTCPregisters[i].length;
			}
			else
			{
				if (groupStartAddress != -1)
				{
					for (int j = 1; j < (i - groupStartAddress); j++)
						ModbusTCPregisters[groupStartAddress + j].connectionID = MBTCP_SERVER_GROUP;
					groupStartAddress = -1;
				}
			}
		}
		else
		{
			if (groupStartAddress != -1)
			{
				for (int j = 1; j < (i - groupStartAddress); j++)
					ModbusTCPregisters[groupStartAddress + j].connectionID = MBTCP_SERVER_GROUP;
				groupStartAddress = -1;
			}
		}

		if (i == MAX_MODBUS_TCP_REGISTERS - 1)
		{
			if (groupStartAddress != -1)
			{
				for (int j = 1; j < (i - groupStartAddress); j++)
					ModbusTCPregisters[groupStartAddress + j].connectionID = MBTCP_SERVER_GROUP;
				groupStartAddress = -1;
			}
		}
	}
}

static void MBTCP_ConvertData(const uint8_t *receivedFrame, MODBUS_TCP_REGISTER* data)
{
	uint16_t tempResultUint16;
	int16_t tempResultInt16;
	uint32_t tempResultUint32;
	int32_t tempResultInt32;
	int64_t tempResultInt64;
	uint64_t tempResultUint64;
	double tempResultDouble;

	switch (data->registerType)
	{
	case UINT_16_BIT:
		*((uint8_t *) &tempResultUint16) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultUint16 + 1) = *(receivedFrame);
		data->registerValue = (float) tempResultUint16;
		break;
	case INT_16_BIT:
		*((uint8_t *) &tempResultInt16) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultInt16 + 1) = *(receivedFrame);
		data->registerValue = (float) tempResultInt16;
		break;
	case UINT_32_BIT:
		*((uint8_t *) &tempResultUint32) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultUint32 + 1) = *(receivedFrame);
		*((uint8_t *) &tempResultUint32 + 2) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultUint32 + 3) = *(receivedFrame + 2);
		data->registerValue = (float) tempResultUint32;
		break;
	case UINT_32_BIT_SW:
		*((uint8_t *) &tempResultUint32) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultUint32 + 1) = *(receivedFrame + 2);
		*((uint8_t *) &tempResultUint32 + 2) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultUint32 + 3) = *(receivedFrame + 0);
		data->registerValue = (float) tempResultUint32;
		break;
	case INT_32_BIT:
		*((uint8_t *) &tempResultInt32) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultInt32 + 1) = *(receivedFrame);
		*((uint8_t *) &tempResultInt32 + 2) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultInt32 + 3) = *(receivedFrame + 2);
		data->registerValue = (float) tempResultInt32;
		break;
	case INT_32_BIT_SW:
		*((uint8_t *) &tempResultInt32) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultInt32 + 1) = *(receivedFrame + 2);
		*((uint8_t *) &tempResultInt32 + 2) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultInt32 + 3) = *(receivedFrame);
		data->registerValue = (float) tempResultInt32;
		break;
	case FLOAT_32_BIT:
		*((uint8_t *) &tempResultUint32) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultUint32 + 1) = *(receivedFrame + 0);
		*((uint8_t *) &tempResultUint32 + 2) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultUint32 + 3) = *(receivedFrame + 2);
		data->registerValue = *((float*) &tempResultUint32);
		break;
	case FLOAT_32_BIT_SW:
		*((uint8_t *) &tempResultUint32) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultUint32 + 1) = *(receivedFrame + 2);
		*((uint8_t *) &tempResultUint32 + 2) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultUint32 + 3) = *(receivedFrame + 0);
		data->registerValue = *((float*) &tempResultUint32);
		break;
	case INT_64_BIT:
		*((uint8_t *) &tempResultInt64) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultInt64 + 1) = *(receivedFrame);
		*((uint8_t *) &tempResultInt64 + 2) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultInt64 + 3) = *(receivedFrame + 2);
		*((uint8_t *) &tempResultInt64 + 4) = *(receivedFrame + 5);
		*((uint8_t *) &tempResultInt64 + 5) = *(receivedFrame + 4);
		*((uint8_t *) &tempResultInt64 + 6) = *(receivedFrame + 7);
		*((uint8_t *) &tempResultInt64 + 7) = *(receivedFrame + 6);
		data->registerValue = (float) tempResultInt64;
		break;
	case DOUBLE_64_BIT:
		*((uint8_t *) &tempResultUint64) = *(receivedFrame + 1);
		*((uint8_t *) &tempResultUint64 + 1) = *(receivedFrame);
		*((uint8_t *) &tempResultUint64 + 2) = *(receivedFrame + 3);
		*((uint8_t *) &tempResultUint64 + 3) = *(receivedFrame + 2);
		*((uint8_t *) &tempResultUint64 + 4) = *(receivedFrame + 5);
		*((uint8_t *) &tempResultUint64 + 5) = *(receivedFrame + 4);
		*((uint8_t *) &tempResultUint64 + 6) = *(receivedFrame + 7);
		*((uint8_t *) &tempResultUint64 + 7) = *(receivedFrame + 6);
		tempResultDouble = *((double*) &tempResultUint64);
		data->registerValue = (float) tempResultDouble;
		break;
	default:
		data->registerValue = 0.0;
		return;
		break;
	}
}

static uint8_t MBTCP_TypeLength(uint8_t type)
{
	switch (type)
	{
	case UINT_16_BIT:
	case INT_16_BIT:
		return 1;
	case UINT_32_BIT:
	case UINT_32_BIT_SW:
	case INT_32_BIT:
	case INT_32_BIT_SW:
	case FLOAT_32_BIT:
	case FLOAT_32_BIT_SW:
		return 2;
	case INT_64_BIT:
	case DOUBLE_64_BIT:
		return 4;
	default:
		return 0;
	}
}

static void MBTCP_GetDataFromFrame(const uint8_t *receivedFrame, MODBUS_TCP_REGISTER* data)
{
	uint8_t dataLength = *(receivedFrame + 8);
	receivedFrame += 9;
	uint8_t typeLength = 0;
	while (1)
	{
		MBTCP_ConvertData(receivedFrame, data);
		if(0==isnanf(data->registerValue))
		{
			data->error = MBTCP_ERROR_OK;
			data->prevError = MBTCP_ERROR_OK;
		}
		else
		{
			if(data->prevError != MBTCP_ERROR_OK)
				data->error = MBTCP_ERROR_RANGE;
			else
				data->prevError = MBTCP_ERROR_RANGE;
		}

		typeLength = MBTCP_TypeLength(data->registerType) << 1;
		dataLength -= typeLength;
		if (dataLength != 0)
		{
			data++;
			receivedFrame += typeLength;
		}
		else
			break;
	}
}

static void MBTCP_SetErrorStatus(MODBUS_TCP_REGISTER* data, uint8_t newStatus)
{
	uint8_t dataLength = data->length;
	uint8_t typeLength = 0;

	while (1)
	{
		data->prevError = data->error;
		data->error = newStatus;
		data->registerValue = NAN;
		typeLength = MBTCP_TypeLength(data->registerType);
		dataLength -= typeLength;
		if (dataLength != 0)
			data++;
		else
			break;
	}
}

static err_t MBTCP_DisconnectFromServer(struct netconn *client)
{
	err_t status = ERR_OK;
	status = netconn_close(client);
	if (ERR_OK == status)
		return netconn_delete(client);
	else
		return status;
}

static struct netconn* MBTCP_ConnectToServer(MODBUS_TCP_SERVER *server)
{
	struct netconn *client = NULL;

	ip4_addr_t serverIP;
	ip4_addr_t clientIP;
	IP4_ADDR(&serverIP, server->ip[0], server->ip[1], server->ip[2], server->ip[3]);
	IP4_ADDR(&clientIP, EthSettings.IPAddress[0], EthSettings.IPAddress[1], EthSettings.IPAddress[2], EthSettings.IPAddress[3]);
	client = netconn_new(NETCONN_TCP);
	if(client == NULL)
		return NULL;
	client->recv_timeout = server->responseTimeout;
	if(ERR_OK != netconn_bind(client, &clientIP, 0))
	{
		netconn_delete (client);
		return NULL;
	}
	if(ERR_OK != netconn_connect(client, &serverIP, server->port))
	{
		netconn_delete (client);
		return NULL;
	}
	return client;
}

static void MBTCP_BuildReadRequestFrame(uint8_t *MBTCPframe, MODBUS_TCP_REGISTER *MBregister, uint16_t transactionID)
{
	*MBTCPframe = (uint8_t) (transactionID >> 8);
	*(++MBTCPframe) = (uint8_t) transactionID;
	*(++MBTCPframe) = 0x0;
	*(++MBTCPframe) = 0x0;
	*(++MBTCPframe) = 0x0;
	*(++MBTCPframe) = 0x6;
	*(++MBTCPframe) = MBregister->deviceAddress;
	*(++MBTCPframe) = MBregister->function;
	*(++MBTCPframe) = (uint8_t) (MBregister->registerAddress >> 8);
	*(++MBTCPframe) = (uint8_t) MBregister->registerAddress;
	*(++MBTCPframe) = (uint8_t) (MBregister->length >> 8);
	*(++MBTCPframe) = (uint8_t) MBregister->length;
}

static void MBTCP_ConvertRegister(MODBUS_TCP_REGISTER *reg)
{
	if ((30000 <= reg->number && reg->number <= 39999) || (300000 <= reg->number && reg->number <= 365535))
	{
		reg->function = READ_INPUT_REG;
		if (reg->number <= 39999)
			reg->registerAddress = reg->number - 30000;
		else
			reg->registerAddress = reg->number - 300000;
	}
	else if ((40000 <= reg->number && reg->number <= 49999) || (400000 <= reg->number && reg->number <= 465535))
	{
		reg->function = READ_HOLDING_REG;
		if (reg->number <= 49999)
			reg->registerAddress = reg->number - 40000;
		else
			reg->registerAddress = reg->number - 400000;
	}
	else
	{
		reg->number = 300000;
		reg->function = READ_INPUT_REG;
		reg->registerAddress = 0;
	}
	reg->length = MBTCP_TypeLength(reg->registerType);
}

static err_t MBTCP_SendRequest(struct netconn *conn, uint8_t *request)
{
	return netconn_write(conn, request, MODBUS_TCP_REQUEST_LENGTH, NETCONN_NOFLAG);
}

static uint8_t MBTCP_CheckData(const uint8_t *receivedFrame, uint16_t transactionID, uint16_t length, uint8_t unitIdentifier, uint8_t functionCode)
{
	uint16_t recvTransactionID = (((uint16_t) *receivedFrame) << 8);
	recvTransactionID |= *(++receivedFrame);

	if (transactionID == recvTransactionID)
	{
		receivedFrame = receivedFrame + 3;
		uint16_t recvLength = (((uint16_t) *receivedFrame) << 8);
		recvLength |= *(++receivedFrame);
		if (length == (recvLength + MBAP_LENGTH_OFFSET))
		{
			if (unitIdentifier == *(++receivedFrame))
			{
				uint8_t recvFunctionCode = *(++receivedFrame);
				if (functionCode == recvFunctionCode)
					return MBTCP_ERROR_OK;
				else if ((functionCode | 0x80) == recvFunctionCode)
				{
					uint8_t recvExeptionCode = *(++receivedFrame) + 0x80;
					return recvExeptionCode;
				}
				else
					return MBTCP_ERROR_MODBUS_CODE;
			}
			else
				return MBTCP_ERROR_MODBUS_CODE;
		}
		else
			return MBTCP_ERROR_MODBUS_CODE;
	}
	else
		return MBTCP_ERROR_MODBUS_CODE;
}

static uint8_t MBTCP_ReciveResponse(struct netconn *conn, MODBUS_TCP_REGISTER *values, uint16_t transactionID)
{
	struct netbuf *inbuf = NULL;
	uint8_t *data = NULL;
	uint16_t len = 0;
	uint16_t responseLength = 0;
	uint8_t responseError = MBTCP_ERROR_OK;
	if (ERR_OK == netconn_recv(conn, &inbuf))
	{
		do
		{
			netbuf_data(inbuf, (void *) &data, &len);
			responseLength += len;
		} while (netbuf_next(inbuf) >= 0);
		responseError = MBTCP_CheckData(data, transactionID, responseLength, values->deviceAddress, values->function);
		if (MBTCP_ERROR_OK == responseError)
			MBTCP_GetDataFromFrame(data, values);
		netbuf_delete(inbuf);
		inbuf = NULL;
	}
	else
		responseError = MBTCP_ERROR_TIMEOUT;

	return responseError;
}

static MODBUS_TCP_REGISTER* MBTCP_SelectRegisterToRead(uint8_t *currentPosition, uint8_t connectionID)
{
	uint8_t startPosiotion = *currentPosition;
	do
	{
		(*currentPosition)++;
		if (*currentPosition >= MAX_MODBUS_TCP_REGISTERS)
			*currentPosition = 0;
		if (connectionID == ModbusTCPregisters[*currentPosition].connectionID)
			return &ModbusTCPregisters[(*currentPosition)];
	}while(*currentPosition != startPosiotion);
	return NULL;
}

void vtaskMBTCPClient(void *pvParameters)
{
	MODBUS_TCP_SERVER *server = ((MODBUS_TCP_SERVER*) pvParameters);
	struct netconn *modbusClient = NULL;
	TickType_t frequency = server->frequency * 1000;

	uint16_t localTransactionID = 0;
	uint8_t RequestFrame[12] = { 0 };
	MODBUS_TCP_REGISTER *currentRegister = NULL;
	uint8_t RegisterPosition = MAX_MODBUS_TCP_REGISTERS - 1;
	uint8_t startRegisterPosition;
	server->state = MBTCP_CONNECT;

	uint8_t responseErrorState = MBTCP_ERROR_OK;

	while (1)
	{
		switch (server->state)
		{
		case MBTCP_CONNECT:
			modbusClient = NULL;
			modbusClient = MBTCP_ConnectToServer(server);
			if(NULL != modbusClient)
				server->state = MBTCP_COMMUNICATION;
			else
			{
				currentRegister = MBTCP_SelectRegisterToRead(&RegisterPosition, server->connectionID);
				if(currentRegister == NULL)
				{
					vTaskDelay(100);
					break;
				}
				startRegisterPosition = RegisterPosition;
				do
				{
					MBTCP_SetErrorStatus(currentRegister, MBTCP_ERROR_NO_CONNECTION);
					currentRegister = MBTCP_SelectRegisterToRead(&RegisterPosition, server->connectionID);
				}while(startRegisterPosition != RegisterPosition);
				server->state = MBTCP_WAIT;
			}
			break;
		case MBTCP_COMMUNICATION:
			currentRegister = MBTCP_SelectRegisterToRead(&RegisterPosition, server->connectionID);
			if(currentRegister == NULL)
			{
				vTaskDelay(100);
				break;
			}
			localTransactionID = TransactionID++;
			MBTCP_BuildReadRequestFrame(RequestFrame, currentRegister, localTransactionID);
			if (ERR_OK == MBTCP_SendRequest(modbusClient, RequestFrame))
			{
				responseErrorState = MBTCP_ReciveResponse(modbusClient, currentRegister, localTransactionID);
				if (MBTCP_ERROR_OK == responseErrorState)
				{
					vTaskDelay(frequency);
				}
				else if (responseErrorState >= 0x80)
				{
					MBTCP_SetErrorStatus(currentRegister, responseErrorState);
				}
				else
				{
					if(MBTCP_ERROR_OK != currentRegister->prevError)
					{
						server->state = MBTCP_DISCONNECT;
						startRegisterPosition = RegisterPosition;
						do
						{
							MBTCP_SetErrorStatus(currentRegister, MBTCP_ERROR_TIMEOUT);
							currentRegister = MBTCP_SelectRegisterToRead(&RegisterPosition, server->connectionID);
						}while(startRegisterPosition != RegisterPosition);

					}
					else
						currentRegister->prevError = MBTCP_ERROR_TIMEOUT;
				}
			}
			else
			{
				server->state = MBTCP_DISCONNECT;
				startRegisterPosition = RegisterPosition;
				do
				{
					MBTCP_SetErrorStatus(currentRegister, MBTCP_ERROR_OTHER);
					currentRegister = MBTCP_SelectRegisterToRead(&RegisterPosition, server->connectionID);
				}while(startRegisterPosition != RegisterPosition);
			}
			break;
		case MBTCP_WAIT:
			vTaskDelay(MBTCP_RECONNECT_DELAY);
			server->state = MBTCP_CONNECT;
			break;
		case MBTCP_DISCONNECT:
			MBTCP_DisconnectFromServer(modbusClient);
			server->state = MBTCP_WAIT;
			break;
		}
	}
}

void CreateMBTCPClientTask(MODBUS_TCP_SERVER *server)
{
	xTaskCreate(vtaskMBTCPClient, "MBTCPclient", 1000, (void*) server, (unsigned portBASE_TYPE ) 4, NULL);
}

void MBTCP_Initialize(void)
{
	for (int i = 0; i < MAX_MODBUS_TCP_CONNECTIONS; i++)
		bkMBserver[i] = MBserver[i];

	for (int i = 0; i < MAX_MODBUS_TCP_REGISTERS; i++)
	{
		if( ModbusTCPregisters[i].connectionID == 0)
			ModbusTCPregisters[i].error = MBTCP_ERROR_REG_DISABLED;
		else if(MBserver[ModbusTCPregisters[i].connectionID-1].mode == 0)
			ModbusTCPregisters[i].error = MBTCP_ERROR_REG_DISABLED;
		else
			ModbusTCPregisters[i].error = MBTCP_ERROR_CONNETING;

		ModbusTCPregisters[i].registerValue = 0.0;
		MBTCP_ConvertRegister(&ModbusTCPregisters[i]);
		bkModbusTCPregisters[i] = ModbusTCPregisters[i];
	}


	MBTCP_GroupRegisters();

	for (int i = 0; i < MAX_MODBUS_TCP_CONNECTIONS; i++)
	{
		if (1 == MBserver[i].mode)
			CreateMBTCPClientTask(&MBserver[i]);
		else
			MBserver[i].state = MBTCP_DISCONNECT;
		vTaskDelay(100);
	}
}

void MBTCP_InitDefaultServers(void)
{
	for(int i=0;i<MAX_MODBUS_TCP_CONNECTIONS;i++)
	{
		MBserver[i].connectionID =i+1;
		MBserver[i].mode = 0;
		MBserver[i].ip[0] = 192;
		MBserver[i].ip[1] = 168;
		MBserver[i].ip[2] = 0;
		MBserver[i].ip[3] = 10;
		MBserver[i].port = 502;
		MBserver[i].responseTimeout = 5000;
		MBserver[i].frequency = 5;
		MBserver[i].state = MBTCP_DISCONNECT;
	}
}

void MBTCP_InitDefaultRegisters(void)
{
	for (int i = 0; i < MAX_MODBUS_TCP_REGISTERS; i++)
	{
		ModbusTCPregisters[i].connectionID = 0;
		ModbusTCPregisters[i].deviceAddress = 1;
		ModbusTCPregisters[i].number = 300000;
		ModbusTCPregisters[i].registerType = UINT_16_BIT;
		ModbusTCPregisters[i].registerValue = 0.0;
		MBTCP_ConvertRegister(&ModbusTCPregisters[i]);
	}
}

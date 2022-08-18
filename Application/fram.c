/*
 * fram.c
 *
 *  Created on: 13.11.2020
 *      Author: RafalMar
 */

#include "fram.h"
#include "i2c.h"

#define FRAM_I2C_ADDRESS_READ							0xA2
#define FRAM_I2C_ADDRESS_WRITE						0xA3
#define FRAM_I2C_TIMEOUT_MAX              1000

uint32_t FramTimeout = FRAM_I2C_TIMEOUT_MAX;

static void FRAM_I2C_Init(void)
{
	MX_I2C1_Init();
}

static void FRAM_I2C_Error(void)
{
	/* De-initialize the SPI comunication BUS */
	HAL_I2C_DeInit(&FRAMI2CHandler);
	/* Re- Initiaize the SPI comunication BUS */
	FRAM_I2C_Init();
}

static void FRAM_I2C_WriteData(uint16_t Reg, uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&FRAMI2CHandler, FRAM_I2C_ADDRESS_WRITE, (uint16_t) Reg,
	I2C_MEMADD_SIZE_16BIT, &Value, 1, FramTimeout);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		FRAM_I2C_Error();
	}
}

static void FRAM_I2C_WriteData_IT(uint16_t Reg, uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write_IT(&FRAMI2CHandler, FRAM_I2C_ADDRESS_WRITE, (uint16_t) Reg,
	I2C_MEMADD_SIZE_16BIT, &Value, 1);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		FRAM_I2C_Error();
	}
}

static void FRAM_I2C_WriteBuffer(uint16_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&FRAMI2CHandler, FRAM_I2C_ADDRESS_WRITE, (uint16_t) Reg,
	I2C_MEMADD_SIZE_16BIT, pBuffer, Length, FramTimeout);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		FRAM_I2C_Error();
	}
}

static uint8_t FRAM_I2C_ReadData(uint16_t Reg)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;

	status = HAL_I2C_Mem_Read(&FRAMI2CHandler, FRAM_I2C_ADDRESS_READ, Reg, I2C_MEMADD_SIZE_16BIT, &value, 1, FramTimeout);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		FRAM_I2C_Error();

	}
	return value;
}

static uint8_t FRAM_I2C_ReadBuffer(uint16_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Read(&FRAMI2CHandler, FRAM_I2C_ADDRESS_READ, (uint16_t) Reg,
	I2C_MEMADD_SIZE_16BIT, pBuffer, Length, FramTimeout);

	/* Check the communication status */
	if (status == HAL_OK)
	{
		return 0;
	}
	else
	{
		/* Re-Initiaize the BUS */
		FRAM_I2C_Error();

		return 1;
	}
}

void FRAM_Init(void)
{
	FRAM_I2C_Init();
}

void FRAM_Write(uint16_t Reg, uint8_t Value)
{
	FRAM_I2C_WriteData(Reg, Value);
}

void FRAM_Write_IT(uint16_t Reg, uint8_t Value)
{
	FRAM_I2C_WriteData_IT(Reg, Value);
}

uint8_t FRAM_Read(uint16_t Reg)
{
	return FRAM_I2C_ReadData(Reg);
}

void FRAM_WriteMultiple(uint16_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	FRAM_I2C_WriteBuffer(Reg, pBuffer, Length);
}

uint16_t FRAM_ReadMultiple(uint16_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	return FRAM_I2C_ReadBuffer(Reg, pBuffer, Length);
}

uint16_t EraseFRAM(void)
{
	uint16_t Resault;
	uint16_t FRAMaddr;
	for (FRAMaddr = 0; FRAMaddr < 0x2000; FRAMaddr++)
		FRAM_Write(FRAMaddr, 0x0);

	Resault = 1;
	return Resault;
}

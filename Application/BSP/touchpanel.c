/*
 * touchpanel.c
 *
 *  Created on: 23.03.2018
 *      Author: Tomaszs
 */

#include "touchpanel.h"
#include <math.h>
#include "i2c.h"
#include "GUI.h"
#include "stmpe811/stmpe811.h"

#include "fram.h"

#include "passwords.h"
#include "screenSaver.h"

#define CONTROLER_TYPE_STMPE811		1
#define CONTROLER_TYPE_TSC2013		0

#define I2Cx_TIMEOUT_MAX			1000
#define TS_I2C_ADDRESS 				0x82
#define TP_MAX_X							799
#define TP_MAX_Y							479
#define TP_MIN_X							1
#define TP_MIN_Y							1


#define TSC2013_ADDRESS 	0x90
#define REG_CFR0_WR ((0x0c << 3) | 0x02)
#define REG_CFR1_WR ((0x0d << 3) | 0x02)
#define REG_CFR2_WR ((0x0e << 3) | 0x02)

#define REG_X1_RD 	((0x00 << 3) | 0x03)
#define REG_CFR0_RD ((0x0c << 3) | 0x03)
#define REG_CFR1_RD ((0x0d << 3) | 0x03)
#define REG_CFR2_RD ((0x0e << 3) | 0x03)
#define REG_STAT_RD ((0x08 << 3) | 0x03)

#define CFR0_PSM_0 (0 << 15)
#define CFR0_PSM_1 (1 << 15)

#define CFR0_STS_0 (0 << 14)
#define CFR0_STS_1 (1 << 14)

#define CFR0_RESOLUTION_10_BIT (0 << 13)
#define CFR0_RESOLUTION_12_BIT (1 << 13)

#define CFR0_CONVERSION_CLOCK_4MHz (0 << 11)
#define CFR0_CONVERSION_CLOCK_2MHz (1 << 11)
#define CFR0_CONVERSION_CLOCK_1MHz (2 << 11)

#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_0 (0 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_1 (1 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_2 (2 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_3 (3 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_4 (4 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_5 (5 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_6 (6 << 8)
#define CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_7 (7 << 8)

#define CFR0_PRECHARGE_TIME_0 (0 << 5)
#define CFR0_PRECHARGE_TIME_1 (1 << 5)
#define CFR0_PRECHARGE_TIME_2 (2 << 5)
#define CFR0_PRECHARGE_TIME_3 (3 << 5)
#define CFR0_PRECHARGE_TIME_4 (4 << 5)
#define CFR0_PRECHARGE_TIME_5 (5 << 5)
#define CFR0_PRECHARGE_TIME_6 (6 << 5)
#define CFR0_PRECHARGE_TIME_7 (7 << 5)

#define CFR0_SENSE_TIME_0 (0 << 2)
#define CFR0_SENSE_TIME_1 (1 << 2)
#define CFR0_SENSE_TIME_2 (2 << 2)
#define CFR0_SENSE_TIME_3 (3 << 2)
#define CFR0_SENSE_TIME_4 (4 << 2)
#define CFR0_SENSE_TIME_5 (5 << 2)
#define CFR0_SENSE_TIME_6 (6 << 2)
#define CFR0_SENSE_TIME_7 (7 << 2)

#define CFR0_DTW_ENABLED (1 << 1)
#define CFR0_DTW_DISABLED (0 << 1)

#define CFR0_LSM_ENABLED 1
#define CFR0_LSM_DISABLED 0

#define CFR2_PINTS_0 (0 << 14)
#define CFR2_PINTS_1 (1 << 14)
#define CFR2_PINTS_2 (2 << 14)
#define CFR2_PINTS_3 (3 << 14)

#define CFR2_M_0 (0 << 12)
#define CFR2_M_1 (1 << 12)
#define CFR2_M_2 (2 << 12)
#define CFR2_M_3 (3 << 12)

#define CFR2_W_0 (0 << 10)
#define CFR2_W_1 (1 << 10)
#define CFR2_W_2 (2 << 10)
#define CFR2_W_3 (3 << 10)

#define CFR2_MAVE_X_ENABLED (1 << 4)
#define CFR2_MAVE_X_DISABLED (0 << 4)

#define CFR2_MAVE_Y_ENABLED (1 << 3)
#define CFR2_MAVE_Y_DISABLED (0 << 3)

#define CFR2_MAVE_Z_ENABLED (1 << 2)
#define CFR2_MAVE_Z_DISABLED (0 << 2)

#define CFR2_MAVE_AUX_ENABLED (1 << 1)
#define CFR2_MAVE_AUX_DISABLED (0 << 1)

#define TP_COEF_A_X	 0.2079
#define TP_COEF_B_X	 -27.765

#define TP_COEF_A_Y		-0.1282
#define TP_COEF_B_Y		495.9728

typedef union
{
	uint16_t value;
	struct
	{
		uint8_t low_byte;
		uint8_t high_byte;
	} bytes;
} tsc2013_register;

typedef union
{
	uint8_t buffer[16];
	struct
	{
		uint16_t X1;
		uint16_t X2;
		uint16_t Y1;
		uint16_t Y2;
		uint16_t IX;
		uint16_t IY;
		uint16_t Z1;
		uint16_t Z2;
		uint16_t AUX;
	} values;
} tsc2013_data_set;

typedef struct {
	uint32_t TouchDetected;
	int32_t X;
	int32_t Y;
	int32_t Z;

} TP_StateTypeDef;

typedef struct
{
	float A;
	float B;
} TS_CAL_COEF;

static uint8_t TouchScreenControler = CONTROLER_TYPE_TSC2013;

TS_CAL_COEF tpCalibrationX,tpCalibrationY;
uint16_t xRaw=0, yRaw=0;

//-------STMPE811-------

static void I2Cx_Error(void)
{
	/* De-initialize the SPI comunication BUS */
	HAL_I2C_MspDeInit(&hi2c2);
	/* Re- Initiaize the SPI comunication BUS */
	MX_I2C2_Init();
}

void IOE_Init(void)
{
	MX_I2C2_Init();
}

void IOE_ITConfig(void)
{

}

void IOE_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}

uint8_t IOE_Read(uint8_t Addr, uint8_t Reg)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;

	status = HAL_I2C_Mem_Read(&hi2c2, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2Cx_TIMEOUT_MAX);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		I2Cx_Error();

	}
	return value;
}

void IOE_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&hi2c2, Addr, (uint16_t) Reg,
	I2C_MEMADD_SIZE_8BIT, &Value, 1, I2Cx_TIMEOUT_MAX);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		I2Cx_Error();
	}
}

uint16_t IOE_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Read(&hi2c2, Addr, (uint16_t) Reg,
	I2C_MEMADD_SIZE_8BIT, pBuffer, Length, I2Cx_TIMEOUT_MAX);

	/* Check the communication status */
	if (status == HAL_OK)
	{
		return 0;
	}
	else
	{
		/* Re-Initiaize the BUS */
		I2Cx_Error();

		return 1;
	}
}

void IOE_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&hi2c2, Addr, (uint16_t) Reg,
	I2C_MEMADD_SIZE_8BIT, pBuffer, Length, I2Cx_TIMEOUT_MAX);

	/* Check the communication status */
	if (status != HAL_OK)
	{
		/* Re-Initiaize the BUS */
		I2Cx_Error();
	}
}

//-------TSC2013-Q1-------

void SwapBytes(uint8_t *high_byte, uint8_t *low_byte)
{
	uint8_t temp;

	temp = *high_byte;
	*high_byte = *low_byte;
	*low_byte = temp;
}

uint16_t TS2013_ReadReg(uint8_t regAdr)
{
	tsc2013_register reg;
	reg.value = 0x0;
	HAL_I2C_Mem_Read(&hi2c2, TSC2013_ADDRESS, regAdr, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &reg.value, 2, I2Cx_TIMEOUT_MAX);
	SwapBytes(&reg.bytes.high_byte,&reg.bytes.low_byte);
	return reg.value;
}

static void TS2013_WriteReg(uint8_t regAdr, uint16_t value)
{
	HAL_I2C_Mem_Write(&hi2c2, TSC2013_ADDRESS, regAdr, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &value , 2, I2Cx_TIMEOUT_MAX);
}

static void TS2013_ReadTouchData(tsc2013_data_set *data)
{
	HAL_I2C_Mem_Read(&hi2c2, TSC2013_ADDRESS, REG_X1_RD, I2C_MEMADD_SIZE_8BIT, data->buffer, 16, I2Cx_TIMEOUT_MAX);
	for(int i = 0; i<8; ++i)
		SwapBytes(&data->buffer[i * 2], &data->buffer[i * 2 + 1]);

}

static uint32_t TS2013_DetectTouch(void)
{
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
		return 0;
	else
		return 1;
}

static void TS2013_Reset(void)
{
	uint8_t data = 0x82;
	HAL_I2C_Master_Transmit(&hi2c2, TSC2013_ADDRESS, &data, 1, I2Cx_TIMEOUT_MAX);
	HAL_Delay(5);
	data = 0x80;
	HAL_I2C_Master_Transmit(&hi2c2, TSC2013_ADDRESS, &data, 1, I2Cx_TIMEOUT_MAX);
}

void TS2013_Init(void)
{
	tsc2013_register cfr0Set;
	tsc2013_register cfr2Set;

	TS2013_Reset();

	cfr0Set.value = CFR0_PSM_1 |
	CFR0_STS_0 |
	CFR0_RESOLUTION_12_BIT |
	CFR0_CONVERSION_CLOCK_4MHz |
	CFR0_PANEL_VOLTAGE_STABILIZATION_TIME_2 |
	CFR0_PRECHARGE_TIME_2 |
	CFR0_SENSE_TIME_2 |
	CFR0_DTW_DISABLED |
	CFR0_LSM_DISABLED;
	SwapBytes(&cfr0Set.bytes.high_byte, &cfr0Set.bytes.low_byte);
	TS2013_WriteReg(REG_CFR0_WR,cfr0Set.value);
	HAL_Delay(5);
	TS2013_WriteReg(REG_CFR1_WR,0x0);
	HAL_Delay(5);

	cfr2Set.value = CFR2_PINTS_1 |
			CFR2_M_3 |
			CFR2_W_2 |
			CFR2_MAVE_X_ENABLED |
			CFR2_MAVE_Y_ENABLED |
			CFR2_MAVE_Z_DISABLED |
			CFR2_MAVE_AUX_DISABLED;
	SwapBytes(&cfr2Set.bytes.high_byte, &cfr2Set.bytes.low_byte);
	TS2013_WriteReg(REG_CFR2_WR,cfr2Set.value);
	HAL_Delay(5);
}

uint16_t cfr0, cfr1, cfr2, stat;

static void TS2013_GetXY(uint16_t *x, uint16_t *y)
{
	tsc2013_data_set tsc2012data={0};
	TS2013_ReadTouchData(&tsc2012data);
	*x = ((tsc2012data.values.X1 - tsc2012data.values.X2)>>1) + tsc2012data.values.X1;
	*y = ((tsc2012data.values.Y1 - tsc2012data.values.Y2)>>1) + tsc2012data.values.Y1;
}

void TOUCHPANEL_Init(void)
{
	uint16_t ret =0xFFFF;
	TP_CALIBRATION_ReadCoefFromFRAM();

	HAL_I2C_Mem_Read(&hi2c2, TS_I2C_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &ret, 2, I2Cx_TIMEOUT_MAX);

	if(0x1108 == ret )
	{
		TouchScreenControler = CONTROLER_TYPE_STMPE811;
		stmpe811_Init(TS_I2C_ADDRESS);
		stmpe811_TS_Start(TS_I2C_ADDRESS);
	}
	else
	{
		TouchScreenControler = CONTROLER_TYPE_TSC2013;
		TS2013_Init();
	}

}

void TP_CALIBRATION_SetDefaultsCoef(void)
{
	tpCalibrationX.A = TP_COEF_A_X;
	tpCalibrationX.B = TP_COEF_B_X;
	tpCalibrationY.A = TP_COEF_A_Y;
	tpCalibrationY.B = TP_COEF_B_Y;
}

void TP_CALIBARION_GetRawValues(uint16_t* X,uint16_t* Y)
{
	*X = xRaw;
	*Y = yRaw;
}

void TP_CALIBARION_UpdateCoefs(float AX,float BX, float AY, float BY)
{
	tpCalibrationX.A = AX;
	tpCalibrationX.B = BX;
	tpCalibrationY.A = AY;
	tpCalibrationY.B = BY;
}

void TP_CALIBRATION_WriteCoefToFRAM(void)
{
	FRAM_WriteMultiple(TOUCHPANEL_CAL_ADDR,(uint8_t*)&tpCalibrationX,8);
	FRAM_WriteMultiple(TOUCHPANEL_CAL_ADDR+8,(uint8_t*)&tpCalibrationY,8);
}

void TP_CALIBRATION_ReadCoefFromFRAM(void)
{
	FRAM_ReadMultiple(TOUCHPANEL_CAL_ADDR,(uint8_t*)&tpCalibrationX,8);
	FRAM_ReadMultiple(TOUCHPANEL_CAL_ADDR+8,(uint8_t*)&tpCalibrationY,8);

	if(isnanf(tpCalibrationX.A) || isnanf(tpCalibrationX.B) || (0 == tpCalibrationX.A && 0 == tpCalibrationX.B))
	{
		tpCalibrationX.A = TP_COEF_A_X;
		tpCalibrationX.B = TP_COEF_B_X;
	}


	if(isnanf(tpCalibrationY.A) || isnanf(tpCalibrationY.B) || (0 == tpCalibrationY.A && 0 == tpCalibrationY.B))
	{
		tpCalibrationY.A = TP_COEF_A_Y;
		tpCalibrationY.B = TP_COEF_B_Y;
	}

}

static void TOUCHPANEL_GetState(TP_StateTypeDef* TsState)
{
	if(CONTROLER_TYPE_STMPE811 == TouchScreenControler)
		TsState->TouchDetected = stmpe811_TS_DetectTouch(TS_I2C_ADDRESS);
	else
		TsState->TouchDetected = TS2013_DetectTouch();

	if (TsState->TouchDetected)
	{
		float x_t, y_t;
		int32_t x,y;
		if(CONTROLER_TYPE_STMPE811 == TouchScreenControler)
			stmpe811_TS_GetXY(TS_I2C_ADDRESS, &xRaw, &yRaw);
		else
			TS2013_GetXY(&xRaw, &yRaw);

		y_t = tpCalibrationY.A*(float)yRaw + tpCalibrationY.B;
		if (y_t <= TP_MIN_X)
			y = TP_MIN_Y;
		else if (y_t > TP_MAX_Y)
			y = TP_MAX_Y;
		else
			y = (int32_t)y_t;

		x_t = tpCalibrationX.A*(float)xRaw + tpCalibrationX.B;
		if (x_t <= TP_MIN_X)
			x = TP_MIN_X;
		else if (x_t >= TP_MAX_X)
			x = TP_MAX_X;
		else
			x = (int32_t)x_t;

		TsState->X = x;
		TsState->Y = y;
	}
}


void TOUCHPANEL_UpdateState(void)
{
	GUI_PID_STATE TS_State={-1,-1,0,0};
	static TP_StateTypeDef prev_state;
	TP_StateTypeDef ts={0,-1,-1, 0};
	uint16_t xDiff=0, yDiff=0;

	TOUCHPANEL_GetState(&ts);
    TS_State.Pressed = ts.TouchDetected;

    if(ts.TouchDetected)
    {
    	PASSWORDS_ResetLogOutTimer();
    	ResetScreenSaverTimer();
    }

	if ((prev_state.TouchDetected != ts.TouchDetected) )
	{
		prev_state.TouchDetected = ts.TouchDetected;
		if(1 == ts.TouchDetected)
		{
			prev_state.X = ts.X;
			prev_state.Y = ts.Y;
		}

		TS_State.x = prev_state.X;
		TS_State.y = prev_state.Y;

		GUI_TOUCH_StoreStateEx(&TS_State);
	}
	else if(1 == ts.TouchDetected)
	{
		xDiff = (prev_state.X > ts.X) ? (prev_state.X - ts.X) : (ts.X - prev_state.X);
		yDiff = (prev_state.Y > ts.Y) ? (prev_state.Y - ts.Y) : (ts.Y - prev_state.Y);

		if ((xDiff > 3) || (yDiff > 3))
		{
			prev_state.TouchDetected = 1;
			if(1 == ts.TouchDetected)
			{
				prev_state.X = ts.X;
				prev_state.Y = ts.Y;
			}

			TS_State.x = prev_state.X;
			TS_State.y = prev_state.Y;

			GUI_TOUCH_StoreStateEx(&TS_State);
		}
	}
	else if(0 == ts.TouchDetected)
	{
		prev_state.X = -1;
		prev_state.Y = -1;
		prev_state.TouchDetected = 0;

		TS_State.x = prev_state.X;
		TS_State.y = TS_State.y;

		GUI_TOUCH_StoreStateEx(&TS_State);
	}
}

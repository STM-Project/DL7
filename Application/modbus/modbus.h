/*
 * modbus.h
 *
 *  Created on: 29.05.2017
 *      Author: Tomaszs
 */

#ifndef STM32F429I_DL7_MODBUS_MODBUS_H_
#define STM32F429I_DL7_MODBUS_MODBUS_H_

void CreateModbusRTUTask(void);
void MODBUS_TimerExpired(void);
void MODBUS_UartHandler(void);

#endif /* STM32F429I_DL7_MODBUS_MODBUS_H_ */

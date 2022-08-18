/*
 * TaskCardsBoot.h
 *
 *  Created on: 04.09.2020
 *      Author: RafalMar
 */

#ifndef GUI_FACTORY_TASKUPGRADEBOARDFIRMWARE_H_
#define GUI_FACTORY_TASKUPGRADEBOARDFIRMWARE_H_

#include "stm32h7xx_hal.h"

#define MAX_FILE_SIZE	 112000

enum answerStatus{
	NONE_RESPONSE,
	_OK,
	UNEXPECTED,
	CRC_FAIL,
	ERASE_FLASH_FAIL,
	WRITE_FLASH_FAIL,
	READ_FILE_OK,
	READ_FILE_ERROR,
	FIRMWARE_UPGRADE_INIT,
	FIRMWARE_UPGRADE_INIT_ERROR,
	START_ERASE_FLASH,
	START_ERASE_FLASH_FAIL,
	ERASE_FLASH_SUCCESS,
	WRITE_DATA_COMPLETE
};

void CreateUpgradeBoardFirmwareTask(uint8_t boardNumber);
void vCloseTaskFirmwareUpgrade(void);

#endif /* GUI_FACTORY_TASKUPGRADEBOARDFIRMWARE_H_ */
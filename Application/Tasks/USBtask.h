/*
 * USBtask.h
 *
 *  Created on: 29 lis 2021
 *      Author: TomaszSokolowski
 */

#ifndef TASKS_USBTASK_H_
#define TASKS_USBTASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#define COPY_PAR_TO_USB 				( 1 << 0 )
#define COPY_PAR_TO_SD					( 1 << 1 )
#define DELETE_FILE							( 1 << 2 )
#define START										( 1 << 3 )
#define DEVICE_CONNECTED				( 1 << 4 )
#define STOP										( 1 << 5 )
#define DEVICE_DISCONNECTED			( 1 << 6 )
#define COPY_FILE								( 1 << 7 )
#define COPY_ACTUAL_ARCHIVES		( 1 << 8 )
#define DEVICE_CONNECTING				( 1 << 9 )
#define READY										( 1 << 10 )
#define START_TASK_FIRMWARE_UPGRADE		( 1 << 13 )

#define INIT_USB								0
#define REGISTER_USB						1
#define USB_OK									2
#define USB_CONNECTING					3
#define USB_BUSY								4
#define USB_TEST								5

extern char DeleteFilePath[];
extern char SrcFilePath[];
extern char DstFilePath[];

int GetUSBState(void);

void CreateUSBTask(void);
void sendUSBEvent(uint32_t event);

#endif /* TASKS_USBTASK_H_ */

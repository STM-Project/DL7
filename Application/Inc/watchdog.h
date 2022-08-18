/*
 * watchdog.h
 *
 *  Created on: 2 gru 2015
 *  Author: METRONIC AKP
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <stdint.h>

void WatchdogRefresh(void);

void CreateWatchdogTask(void);

void SetWatchdogFlag(void);

#define UNKNOWN 0
#define ALIVE 1

extern uint8_t WDFlags[];

void SetSoftwareWDFlag(void);

#endif /* WATCHDOG_H_ */

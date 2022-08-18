/*
 * MeasurementsSynchronization.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef CHANNELSSYNCHRONIZATION_H_
#define CHANNELSSYNCHRONIZATION_H_

#include <stdint.h>

void InitChannelsMutex(void);
uint32_t TakeChannelsMutex(uint32_t timeout);
void GiveChannelsMutex(void);

#endif /* CHANNELSSYNCHRONIZATION_H_ */

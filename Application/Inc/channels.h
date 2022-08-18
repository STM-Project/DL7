/*
 * channels.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef CHANNELS_H_
#define CHANNELS_H_

#include "channels_typedef.h"

extern CHANNEL Channels[];
extern CHANNEL bkChannels[];

extern uint16_t NumberOfHistoryItems;

void CHANNELS_Initialize(void);

void GetChannel(CHANNEL * Channel, int index);
void SetChannel(const CHANNEL * Channel, int index);

#endif /* CHANNELS_H_ */

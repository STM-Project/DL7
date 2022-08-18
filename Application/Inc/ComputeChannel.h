/*
 * ComputeChannel.h
 *
 *  Created on: 07.08.2017
 *      Author: MagdalenaRyb
 */

#ifndef COMPUTECHANNEL_H_
#define COMPUTECHANNEL_H_
#include <stdint.h>
#include "channels_typedef.h"

uint8_t CheckFormulaCorrectness(char * formula);
uint8_t StriToRPN(char *postfix_exp, char *infix_exp);
void convertSpecialCase(char* Dst, char* Src);
float computeRPN(CHANNEL *Channels, int channelNumber);

#endif /* COMPUTECHANNEL_H_ */

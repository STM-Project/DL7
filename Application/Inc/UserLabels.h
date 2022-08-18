/*
 * UserLabels.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef USERLABELS_H_
#define USERLABELS_H_

#include <stdint.h>

typedef struct {
	uint8_t labelType;	//0- wartość bieżąca, 1- maximum, 2-minimum, 3-sumator 1, 4-sumator 2
	uint8_t channelNumber;
	uint8_t channelType;

	char description[51];
	double value;
	char unit[21];
	char resolution;
	uint32_t Color;

	uint8_t failureState;
	uint8_t failureMode;
	float failureValue;
}USER_LABEL;

int GetUserLabel(USER_LABEL *TableLabel, short index);
int GetUserLabelValue(USER_LABEL *TableLabel, short index);

#endif /* USERLABELS_H_ */

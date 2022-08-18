/*
 * measurments.h
 *
 *  Created on: Jul 11, 2016
 *      Author: Tomaszs
 */

#ifndef MEASURMENTS_H_
#define MEASURMENTS_H_

#include <ExpansionCards_typedef.h>
#include "channels_typedef.h"

float SetTotalizerPeriodValue(uint8_t Period);
float SetTotalizerMultiplerValue(uint8_t Multipler);

float CalculateDemoCharacteristicValue(const CHANNEL * Channel, float RawValue);
float CalculateCharacteristicValue(CHANNEL * Channel, EXPANSION_CARD * card);
float CalculateCharacteristicValueCompute(CHANNEL * channel, float RawValue);
float CalculateFilteredValue(CHANNEL * channel, const float * NewValue);
#endif /* MEASURMENTS_H_ */

#include "UserChar.h"
#include <channels.h>
#include <math.h>

float CalcUserCharValue(float RawValue, const USER_CHAR * characteristic)
{
	float CalcValue = 0.0;
	uint8_t numberOfPoints = characteristic->numberOfPoints;
	uint8_t index =0;

	if (RawValue <= characteristic->pointsX[0]) {
		CalcValue = characteristic->pointsY[0] + ((characteristic->pointsY[1] - characteristic->pointsY[0])/(characteristic->pointsX[1] - characteristic->pointsX[0]))*(RawValue - characteristic->pointsX[0]);
		return CalcValue;
	}
	else if (RawValue >= characteristic->pointsX[numberOfPoints-1]) {
		CalcValue = characteristic->pointsY[numberOfPoints-2] + ((characteristic->pointsY[numberOfPoints-1] - characteristic->pointsY[numberOfPoints-2])/(characteristic->pointsX[numberOfPoints-1] - characteristic->pointsX[numberOfPoints-2]))*(RawValue - characteristic->pointsX[numberOfPoints-2]);
		return CalcValue;
	}
	else {
		while (RawValue >characteristic->pointsX[index]) {
			index ++ ;
		}
		CalcValue = characteristic->pointsY[index-1] + ((characteristic->pointsY[index] - characteristic->pointsY[index-1])/(characteristic->pointsX[index] - characteristic->pointsX[index-1]))*((RawValue - characteristic->pointsX[index-1]));
		return CalcValue;
	}

	return CalcValue;
}

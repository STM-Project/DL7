/*
 * UserLabels.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#include <channels.h>
#include <string.h>
#include <UserLabels.h>
#include "skins.h"
#include "mini-printf.h"

int GetUserLabel(USER_LABEL *TableLabel, short index)
{
	TableLabel->labelType = index / 100;
	TableLabel->channelNumber = index % 100;

	switch (TableLabel->labelType)
	{
	case 0: //value
		TableLabel->channelType = Channels[TableLabel->channelNumber].source.type;

		strcpy(TableLabel->description, Channels[TableLabel->channelNumber].description);
		TableLabel->value = Channels[TableLabel->channelNumber].value;
		TableLabel->resolution = Channels[TableLabel->channelNumber].resolution;
		strcpy(TableLabel->unit, Channels[TableLabel->channelNumber].unit);
		TableLabel->Color = Channels[TableLabel->channelNumber].Color;

		TableLabel->failureState = Channels[TableLabel->channelNumber].failureState;
		TableLabel->failureMode = Channels[TableLabel->channelNumber].failureMode;
		TableLabel->failureValue = Channels[TableLabel->channelNumber].failureValue;

		break;
	case 1: //max
		TableLabel->channelType = Channels[TableLabel->channelNumber].source.type;

		mini_snprintf(TableLabel->description, 50, "▲: %s", Channels[TableLabel->channelNumber].description);
		TableLabel->value = Channels[TableLabel->channelNumber].max;
		TableLabel->resolution = Channels[TableLabel->channelNumber].resolution;
		strcpy(TableLabel->unit, Channels[TableLabel->channelNumber].unit);
		TableLabel->Color = SKINS_GetTextColor();

		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		TableLabel->failureValue = 0.0;

		break;
	case 2: //min
		TableLabel->channelType = Channels[TableLabel->channelNumber].source.type;
		mini_snprintf(TableLabel->description, 50, "▼: %s", Channels[TableLabel->channelNumber].description);
		TableLabel->value = Channels[TableLabel->channelNumber].min;
		TableLabel->resolution = Channels[TableLabel->channelNumber].resolution;
		strcpy(TableLabel->unit, Channels[TableLabel->channelNumber].unit);
		TableLabel->Color = SKINS_GetTextColor();

		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		TableLabel->failureValue = 0.0;
		break;
	case 3: //tot 1
		TableLabel->channelType = Channels[TableLabel->channelNumber].source.type;
		mini_snprintf(TableLabel->description, 50, "∑1: %s", Channels[TableLabel->channelNumber].description);
		TableLabel->value = Channels[TableLabel->channelNumber].Tot1.value;
		TableLabel->resolution = Channels[TableLabel->channelNumber].Tot1.resolution;
		strcpy(TableLabel->unit, Channels[TableLabel->channelNumber].Tot1.unit);
		TableLabel->Color = SKINS_GetTextColor();

		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		TableLabel->failureValue = 0.0;
		break;
	case 4: //tot 2
		TableLabel->channelType = Channels[TableLabel->channelNumber].source.type;
		mini_snprintf(TableLabel->description, 50, "∑2: %s", Channels[TableLabel->channelNumber].description);
		TableLabel->value = Channels[TableLabel->channelNumber].Tot2.value;
		TableLabel->resolution = Channels[TableLabel->channelNumber].Tot2.resolution;
		strcpy(TableLabel->unit, Channels[TableLabel->channelNumber].Tot2.unit);
		TableLabel->Color = SKINS_GetTextColor();

		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		TableLabel->failureValue = 0.0;
		break;
	}
	return 0;
}

int GetUserLabelValue(USER_LABEL *TableLabel, short index)
{
	uint16_t labelType = index / 100;
	uint16_t channelNumber = index % 100;
	switch (labelType)
	{
	case 0:
		TableLabel->channelType = Channels[channelNumber].source.type;
		TableLabel->value = Channels[channelNumber].value;
		TableLabel->resolution = Channels[channelNumber].resolution;
		TableLabel->Color = Channels[channelNumber].Color;
		TableLabel->failureState = Channels[channelNumber].failureState;
		TableLabel->failureMode = Channels[channelNumber].failureMode;
		break;
	case 1:
		TableLabel->channelType = Channels[channelNumber].source.type;
		TableLabel->value = Channels[channelNumber].max;
		TableLabel->resolution = Channels[channelNumber].resolution;
		TableLabel->Color = SKINS_GetTextColor();
		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		break;
	case 2:
		TableLabel->channelType = Channels[channelNumber].source.type;
		TableLabel->value = Channels[channelNumber].min;
		TableLabel->resolution = Channels[channelNumber].resolution;
		TableLabel->Color = SKINS_GetTextColor();
		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		break;
	case 3:
		TableLabel->channelType = Channels[channelNumber].source.type;
		TableLabel->value = Channels[channelNumber].Tot1.value;
		TableLabel->resolution = Channels[channelNumber].Tot1.resolution;
		TableLabel->Color = SKINS_GetTextColor();
		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		break;
	case 4:
		TableLabel->channelType = Channels[channelNumber].source.type;
		TableLabel->value = Channels[channelNumber].Tot2.value;
		TableLabel->resolution = Channels[channelNumber].Tot2.resolution;
		TableLabel->Color = SKINS_GetTextColor();
		TableLabel->failureState = 1;
		TableLabel->failureMode = 0;
		break;
	default:
		return 0;
	}
	return 1;
}

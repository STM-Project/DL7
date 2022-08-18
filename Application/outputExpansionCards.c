/*
 * outputExpansionCards.c
 *
 *  Created on: 15.11.2017
 *      Author: Tomaszs
 */

#include "outputExpansionCards.h"
#include "channels.h"
#include "ExpansionCards.h"

enum { disable,current0to20,current4to20,current0to24,voltage0to5,voltage0to10};

static const float OutputRanges[6][2] =
{
		{0.0,0.0},
		{0.0,22.0},
		{3.6,22.0},
		{0.0,26.0},
		{0.0,6.0},
		{0.0,11.0},
};

static float ConvertToElectricValue (float rawValue, AO_CHANNEL_SETTINGS *settings)
{
	float newValue = 0;
	switch(settings->type)
	{
	case disable:
		break;
	case current0to20:
		newValue = ((rawValue - settings->lowScaleValue) * 20) / (settings->highScaleValue - settings->lowScaleValue);
		break;
	case current4to20:
		newValue = ((rawValue - settings->lowScaleValue) * 16) / (settings->highScaleValue - settings->lowScaleValue) + 4;
		break;
	case current0to24:
		newValue = ((rawValue - settings->lowScaleValue) * 24) / (settings->highScaleValue - settings->lowScaleValue);
		break;
	case voltage0to5:
		newValue = ((rawValue - settings->lowScaleValue) * 5) / (settings->highScaleValue - settings->lowScaleValue);
		break;
	case voltage0to10:
		newValue = ((rawValue - settings->lowScaleValue) * 10) / (settings->highScaleValue - settings->lowScaleValue);
		break;
	default:
		break;
	}
	return newValue;
}

static void SetOutputChannelStatus(CARD_CHANNEL *cardChannel, uint8_t channelType)
{
 if(OutputRanges[channelType][0] <= cardChannel->value &&  cardChannel->value <= OutputRanges[channelType][1])
	 cardChannel->status = OK;
 else
	 cardChannel->status = RANGE;
}

static void SetOutputChannelValue(CARD_CHANNEL *cardChannel, AO_CHANNEL_SETTINGS *settings)
{
	if(1 == Channels[settings->source].failureState)
	{
		cardChannel->value = ConvertToElectricValue(Channels[settings->source].value, settings);
		SetOutputChannelStatus(cardChannel, settings->type);
	}
	else
	{
		if(NO_SETTINGS == Channels[settings->source].failureState)
			cardChannel->status = NO_SETTINGS;
		else
		{
			if (1 == Channels[settings->source].failureMode)
			{
				cardChannel->value = ConvertToElectricValue(Channels[settings->source].value, settings);
				SetOutputChannelStatus(cardChannel, settings->type);
			}
			else
			{
				if (1 == settings->failureMode)
				{
					cardChannel->value = settings->failureValue;
					cardChannel->status = OK;
				}
				else
				{
					cardChannel->value =  0.0;
					cardChannel->status = OTHER_ERR;
				}
			}
		}
	}
}

void UpdateOutputExpansionCardsValues(void)
{
	for(int i=0; i<NUMBER_OF_CARD_SLOTS;i++)
	{
		if (SPI_CARD_OUT3 == ExpansionCards[i].type)
			for (int j=0;j<3;j++)
				SetOutputChannelValue(&ExpansionCards[i].channel[j], &ExpansionCards[i].settings.OUTcard.channels[j]);
	}
}

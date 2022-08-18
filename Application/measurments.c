/*
 * measurments.c
 *
 *  Created on: 26 sie 2015
 *      Author: Tomaszs
 */

#include "channels.h"
#include "measurments.h"
#include <math.h>
#include "RTD.h"
#include "TC.h"
#include "UserChar.h"
#include "parameters.h"

static const float FilterTimeConstant[10] =
{ 0, 1.5528195, 4.545465, 9.5659, 19.61976, 29.67619, 59.84823, 120.1944, 180.541, 300.22878 };

static float CalcIN6ICharValue(const CHANNEL * channel, float RawValue, float CorrectionValue);
static float CalcIN6RTDCharValue(CHANNEL * channel, float RawValue, float CorrectionValue);
static float CalcIN6TCCharValue(CHANNEL * Channel, CARD_CHANNEL *newMeasurement, IO_CHANNEL_SETTINGS * newMeasurementSettings);
static float CalcTCCharValue(CHANNEL * channel, float RawValue, float CorrectionValue, COMPENSATION *compensation);
static float CalcIN6DCharValue(CHANNEL * channel, float RawValue, float CorrectionValue, uint inputType);
static float hornerf(const float *coeffs, int s, float x);
static void FarenheitToCelcius(float *Temperature);
static void CelciusToFarenheit(float * Temperature);

float CalculateDemoCharacteristicValue(const CHANNEL * Channel, float RawValue)
{
	return CalcIN6ICharValue(Channel,RawValue,0.0);
}

float CalculateCharacteristicValue(CHANNEL * Channel, EXPANSION_CARD * card)
{
	float CharValue;
	switch (card->type)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
	case SPI_CARD_IN4SG:
		CharValue = CalcIN6ICharValue(Channel, card->channel[Channel->source.number].value, card->settings.IOcard.channels[Channel->source.number].adjusment);
		break;
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		CharValue = CalcIN6RTDCharValue(Channel, card->channel[Channel->source.number].value, card->settings.IOcard.channels[Channel->source.number].adjusment);
		break;
	case SPI_CARD_IN6TC:
		CharValue = CalcIN6TCCharValue(Channel, &card->channel[Channel->source.number],&card->settings.IOcard.channels[Channel->source.number]);
		break;
	case SPI_CARD_IN6V:
		CharValue = CalcIN6ICharValue(Channel, card->channel[Channel->source.number].value, card->settings.IOcard.channels[Channel->source.number].adjusment);
		break;
	case SPI_CARD_IN3:
		if(3 > Channel->source.number)
		{
			switch (card->settings.IOcard.channels[Channel->source.number].type)
			{
			case 1: //tryb I
			case 2: //tryb I
			case 3: //tryb V
			case 4: //tryb V
				CharValue = CalcIN6ICharValue(Channel, card->channel[Channel->source.number].value, card->settings.IOcard.channels[Channel->source.number].adjusment);
				break;
			case 5: //tryb TC
				CharValue = CalcIN6TCCharValue(Channel, &card->channel[Channel->source.number], &card->settings.IOcard.channels[Channel->source.number]);
				break;
			case 6:
			case 7:
			case 8:
				CharValue = CalcIN6RTDCharValue(Channel, card->channel[Channel->source.number].value, card->settings.IOcard.channels[Channel->source.number].adjusment);
				break;
			default:
				CharValue = 0.0;
			}
		}
		else
			CharValue = CalcIN6ICharValue(Channel, card->channel[Channel->source.number].value, 0.0);

		break;
	case SPI_CARD_IN6D:
		if(3 == card->settings.IOcard.channels[Channel->source.number].type)
		{
			CharValue = Channel->LowScaleValue * card->channel[Channel->source.number].diffValue;
		}
		else
		{
			CharValue = CalcIN6DCharValue(Channel,  card->channel[Channel->source.number].value,  card->settings.IOcard.channels[Channel->source.number].adjusment,card->settings.IOcard.channels[Channel->source.number].type);

		}
		break;
	case SPI_CARD_2RS485:
	case SPI_CARD_2RS485I24:
		CharValue = CalcIN6ICharValue(Channel, card->channel[Channel->source.number].value, 0);
		break;
	case SPI_CARD_OUT3:
		CharValue = CalcIN6ICharValue(Channel, card->channel[Channel->source.number].value, 0);
		break;
	default:
		CharValue = card->channel[Channel->source.number].value;
		break;
	}
	return CharValue;
}

float CalculateCharacteristicValueCompute(CHANNEL * channel, float RawValue)
{
	float CalcValue = 0.0;
	switch (channel->CharacteristicType)
	{
	case 0:
		CalcValue = ((RawValue - channel->LowSignalValue) * (channel->HighScaleValue - channel->LowScaleValue))
				/ (channel->HighSignalValue - channel->LowSignalValue) + channel->LowScaleValue;
		break;
	case 1:
		CalcValue = CalcUserCharValue(RawValue, &UserCharacteristcs[channel->UserCharacteristicType]);
		break;
	case 2:
		CalcValue = RawValue;
		break;
	default:
		CalcValue = channel->value;
	break;
	}
	return CalcValue;
}

static float CalcIN6ICharValue(const CHANNEL * channel, float RawValue, float CorrectionValue)
{
	float CalcValue = 0.0;
	RawValue = RawValue + CorrectionValue;
	switch (channel->CharacteristicType)
	{
	case 0:
		CalcValue = ((RawValue - channel->LowSignalValue) * (channel->HighScaleValue - channel->LowScaleValue))
				/ (channel->HighSignalValue - channel->LowSignalValue) + channel->LowScaleValue;
		break;
	case 1:
		CalcValue =CalcUserCharValue(RawValue, &UserCharacteristcs[channel->UserCharacteristicType]);
		break;
	case 2:
		CalcValue = RawValue;
		break;
	default:
		CalcValue = channel->value;
		break;
		}
		return CalcValue;
	}

static uint8_t RTD_CalculateCelciusTemperature(const RTD_RANGE *range, const float *coeffs, int coeffsNumber, float rawValue,
		float *calcValue)
{
	if (range->min <= rawValue && rawValue <= range->max)
	{
		*calcValue = hornerf(coeffs, coeffsNumber, rawValue);
		return OK;
	}
	else
		return RANGE;
}

static uint8_t RTD_CalculateFarenheitTemperature(const RTD_RANGE *range, const float *coeffs, int coeffsNumber, float rawValue,
		float *calcValue)
{
	if (OK == RTD_CalculateCelciusTemperature(range, coeffs, coeffsNumber, rawValue, calcValue))
	{
		CelciusToFarenheit(calcValue);
		return OK;
	}
	else
		return RANGE;
}

static float CalcIN6RTDCharValue(CHANNEL * channel, float RawValue, float CorrectionValue)
{
	float CalcValue = 0.0;
	RawValue = RawValue + CorrectionValue;
	switch (channel->CharacteristicType)
	{
	case 0:
		CalcValue = ((RawValue - channel->LowSignalValue) * (channel->HighScaleValue - channel->LowScaleValue))
				/ (channel->HighSignalValue - channel->LowSignalValue) + channel->LowScaleValue;
		break;
	case 1:
		CalcValue =CalcUserCharValue(RawValue, &UserCharacteristcs[channel->UserCharacteristicType]);
		break;
	case 2:
		CalcValue = RawValue;
		break;
	case 3:
		channel->failureState = RTD_CalculateCelciusTemperature(&PtRange, Pt100Coef, 6, RawValue, &CalcValue);
		break;
	case 4:
		channel->failureState = RTD_CalculateFarenheitTemperature(&PtRange, Pt100Coef, 6, RawValue, &CalcValue);
		break;
	case 5:
		channel->failureState = RTD_CalculateCelciusTemperature(&PtRange, Pt100Coef, 6, RawValue / 2, &CalcValue);
		break;
	case 6:
		channel->failureState = RTD_CalculateFarenheitTemperature(&PtRange, Pt100Coef, 6, RawValue / 2, &CalcValue);
		break;
	case 7:
		channel->failureState = RTD_CalculateCelciusTemperature(&PtRange, Pt100Coef, 6, RawValue / 5, &CalcValue);
		break;
	case 8:
		channel->failureState = RTD_CalculateFarenheitTemperature(&PtRange, Pt100Coef, 6, RawValue / 5, &CalcValue);
		break;
	case 9:
		channel->failureState = RTD_CalculateCelciusTemperature(&PtRange, Pt100Coef, 6, RawValue / 10, &CalcValue);
		break;
	case 10:
		channel->failureState = RTD_CalculateFarenheitTemperature(&PtRange, Pt100Coef, 6, RawValue / 10, &CalcValue);
		break;
	case 11:
		channel->failureState = RTD_CalculateCelciusTemperature(&NiRange, Ni100Coef, 5, RawValue, &CalcValue);
		break;
	case 12:
		channel->failureState = RTD_CalculateFarenheitTemperature(&NiRange, Ni100Coef, 5, RawValue, &CalcValue);
		break;
	case 13:
		channel->failureState = RTD_CalculateCelciusTemperature(&NiRange, Ni100Coef, 5, RawValue / 1.2, &CalcValue);
		break;
	case 14:
		channel->failureState = RTD_CalculateFarenheitTemperature(&NiRange, Ni100Coef, 5, RawValue / 1.2, &CalcValue);
		break;
	case 15:
		channel->failureState = RTD_CalculateCelciusTemperature(&NiRange, Ni100Coef, 5, RawValue / 10, &CalcValue);
		break;
	case 16:
		channel->failureState = RTD_CalculateFarenheitTemperature(&NiRange, Ni100Coef, 5, RawValue / 10, &CalcValue);
		break;
	case 17:
		channel->failureState = RTD_CalculateCelciusTemperature(&CuRange, Cu50Coef, 5, RawValue, &CalcValue);
		break;
	case 18:
		channel->failureState = RTD_CalculateFarenheitTemperature(&CuRange, Cu50Coef, 5, RawValue, &CalcValue);
		break;
	case 19:
		channel->failureState = RTD_CalculateCelciusTemperature(&CuRange, Cu50Coef, 5, RawValue / 1.06, &CalcValue);
		break;
	case 20:
		channel->failureState = RTD_CalculateFarenheitTemperature(&CuRange, Cu50Coef, 5, RawValue / 1.06, &CalcValue);
		break;
	case 21:
		channel->failureState = RTD_CalculateCelciusTemperature(&CuRange, Cu50Coef, 5, RawValue / 2, &CalcValue);
		break;
	case 22:
		channel->failureState = RTD_CalculateFarenheitTemperature(&CuRange, Cu50Coef, 5, RawValue / 2, &CalcValue);
		break;
	case 23:
		channel->failureState = RTD_CalculateCelciusTemperature(&KTY81Range, KTY81Coef, 5, RawValue, &CalcValue);
		break;
	case 24:
		channel->failureState = RTD_CalculateFarenheitTemperature(&KTY81Range, KTY81Coef, 5, RawValue, &CalcValue);
		break;
	case 25:
		channel->failureState = RTD_CalculateCelciusTemperature(&KTY83Range, KTY83Coef, 5, RawValue, &CalcValue);
		break;
	case 26:
		channel->failureState = RTD_CalculateFarenheitTemperature(&KTY83Range, KTY83Coef, 5, RawValue, &CalcValue);
		break;
	case 27:
		channel->failureState = RTD_CalculateCelciusTemperature(&KTY84Range, KTY84Coef, 5, RawValue, &CalcValue);
		break;
	case 28:
		channel->failureState = RTD_CalculateFarenheitTemperature(&KTY84Range, KTY84Coef, 5, RawValue, &CalcValue);
		break;
	default:
		CalcValue = channel->value;
		break;
	}
	return CalcValue;
}

static float CalcIN6TCCharValue(CHANNEL * Channel, CARD_CHANNEL *newMeasurement, IO_CHANNEL_SETTINGS * newMeasurementSettings)
{
	float CharValue = 0.0;
	COMPENSATION compensation =	{ 0, 0.0 };

	if(3 <= Channel->CharacteristicType)
	{
		if (newMeasurementSettings->compChannel > -1 && newMeasurementSettings->compChannel < 100)
		{
			if (Channels[newMeasurementSettings->compChannel].failureState == 1)
			{
				compensation.value = Channels[newMeasurementSettings->compChannel].value;
				compensation.state = 1;
			}
			else
			{
				Channel->failureState = Channels[newMeasurementSettings->compChannel].failureState;
				CharValue = 0.0;
				return CharValue;
			}
		}
		else if (newMeasurementSettings->compChannel == -1)
		{
			compensation.value = newMeasurementSettings->compensationConstantValue;
			compensation.state = 1;
		}
		else if (newMeasurementSettings->compChannel == -2)
		{
			compensation.value = 0.0;
			compensation.state = 1;
		}
	}
	CharValue = CalcTCCharValue(Channel, newMeasurement->value, newMeasurementSettings->adjusment, &compensation);
	return CharValue;
}

static uint8_t TC_CompensateMesurment(const COMPENSATION_RANGE *range, COMPENSATION *compensation, const float *coeffs, int coeffsNumber,
		float *rawValue)
{
	if (compensation->state)
	{
		if (range->min <= compensation->value && compensation->value <= range->max)
		{
			*rawValue += hornerf(coeffs, coeffsNumber, compensation->value);
			return OK;
		}
		else
			return RANGE;
	}
	else
		return compensation->state;
}

static uint8_t TC_CompensateFarenheitMesurment(const COMPENSATION_RANGE *range, COMPENSATION *compensation, const float *coeffs,
		int coeffsNumber, float *rawValue)
{
	FarenheitToCelcius(&compensation->value);
	return TC_CompensateMesurment(range, compensation, coeffs, coeffsNumber, rawValue);
}

static uint8_t TC_CalculateCelciusTemperature(char type, float *rawValue, float *calcValue)
{
	switch (type)
	{
	case 'J': //(Fe-CuNi) (-210� C .. +1200� C)
		if (-8095 <= *rawValue && *rawValue < 0)
			*calcValue = hornerf(TCJ1Coef, 8, *rawValue);
		else if (0 <= *rawValue && *rawValue < 42919)
			*calcValue = hornerf(TCJ2Coef, 7, *rawValue);
		else if (42919 <= *rawValue && *rawValue < 69553)
			*calcValue = hornerf(TCJ3Coef, 5, *rawValue);
		else
			return RANGE;
		break;

	case 'K': //(NiCr-NiAl) (-270� C .. + 1372� C)
		if (-5891 <= *rawValue && *rawValue < 0)
			*calcValue = hornerf(TCK1Coef, 8, *rawValue);
		else if (0 <= *rawValue && *rawValue < 20644)
			*calcValue = hornerf(TCK2Coef, 9, *rawValue);
		else if (20644 <= *rawValue && *rawValue < 54886)
			*calcValue = hornerf(TCK3Coef, 6, *rawValue);
		else
			return RANGE;
		break;
	case 'N': //(NiCrSi-NiSi) (-270� C .. 1300� C)
		if (-3990 <= *rawValue && *rawValue < 0)
			*calcValue = hornerf(TCN1Coef, 9, *rawValue);
		else if (0 <= *rawValue && *rawValue < 20613)
			*calcValue = hornerf(TCN2Coef, 7, *rawValue);
		else if (20613 <= *rawValue && *rawValue < 47513)
			*calcValue = hornerf(TCN3Coef, 5, *rawValue);
		else
			return RANGE;
		break;
	case 'R': //(PtRh 13-Pt) (-50� C .. 1768� C)
		if (-226 <= *rawValue && *rawValue < 1923)
			*calcValue = hornerf(TCR1Coef, 10, *rawValue);
		else if (1923 <= *rawValue && *rawValue < 11361)
			*calcValue = hornerf(TCR2Coef, 9, *rawValue);
		else if (11361 <= *rawValue && *rawValue < 19739)
			*calcValue = hornerf(TCR3Coef, 5, *rawValue);
		else if (19739 <= *rawValue && *rawValue < 21103)
			*calcValue = hornerf(TCR4Coef, 4, *rawValue);
		else
			return RANGE;
		break;
	case 'S': //(PtRh 10-Pt) (-50� C .. 1768� C)
		if (-235 <= *rawValue && *rawValue < 1874)
			*calcValue = hornerf(TCS1Coef, 9, *rawValue);
		else if (1874 <= *rawValue && *rawValue < 10332)
			*calcValue = hornerf(TCS2Coef, 9, *rawValue);
		else if (10332 <= *rawValue && *rawValue < 17536)
			*calcValue = hornerf(TCS3Coef, 5, *rawValue);
		else if (17536 <= *rawValue && 18694 < *rawValue)
			*calcValue = hornerf(TCS4Coef, 4, *rawValue);
		else
			return RANGE;
		break;
	case 'T': //(Cu-CuNi) (-200� C .. 400� C)
		if (-5603 <= *rawValue && *rawValue < 0)
			*calcValue = hornerf(TCT1Coef, 7, *rawValue);
		else if (0 <= *rawValue && *rawValue < 20872)
			*calcValue = hornerf(TCT2Coef, 6, *rawValue);
		else
			return RANGE;
		break;
	case 'E':  //(NiCr-CuNi) (-270� C .. 1000� C)
		if (-8825 <= *rawValue && *rawValue < 0)
			*calcValue = hornerf(TCE1Coef, 8, *rawValue);
		else if (0 <= *rawValue && *rawValue < 76373)
			*calcValue = hornerf(TCE2Coef, 9, *rawValue);
		else
			return RANGE;
		break;
	case 'B':  //(PtRh30-PtRh6) (+250� C .. 1820� C)
		if (291 <= *rawValue && *rawValue < 2431)
			*calcValue = hornerf(TCB1Coef, 8, *rawValue);
		else if (2431 <= *rawValue && *rawValue < 13820)
			*calcValue = hornerf(TCB2Coef, 8, *rawValue);
		else
			return RANGE;
		break;
	case 'L': //(Fe-CuNi) (-200� C .. 900� C)
		if (-8.15 <= *rawValue && *rawValue <= 53.14)
			*calcValue = hornerf(TCLCoef, 6, *rawValue);
		else
			return RANGE;
		break;
	case 'U': //(Cu-CuNi) (-200� C .. 600� C)
		if (-5.7 <= *rawValue && *rawValue <= 34.31)
			*calcValue = hornerf(TCUCoef, 6, *rawValue);
		else
			return RANGE;
		break;
	default:
		return RANGE;
	}
	return OK;
}

static uint8_t TC_CalculateFarenheitTemperature(char type, float *rawValue, float *calcValue)
{
	if (OK == TC_CalculateCelciusTemperature(type, rawValue, calcValue))
	{
		CelciusToFarenheit(calcValue);
		return OK;
	}
	else
		return RANGE;
}

static float CalcTCCharValue(CHANNEL * channel, float RawValue, float CorrectionValue, COMPENSATION *compensation)
{
	float CalcValue = 0.0;
	RawValue = RawValue + CorrectionValue;

	switch (channel->CharacteristicType)
	{
	case 0:
		CalcValue = ((RawValue - channel->LowSignalValue) * (channel->HighScaleValue - channel->LowScaleValue))
				/ (channel->HighSignalValue - channel->LowSignalValue) + channel->LowScaleValue;
		break;
	case 1:
		CalcValue =CalcUserCharValue(RawValue, &UserCharacteristcs[channel->UserCharacteristicType]);
		break;
	case 2:
		CalcValue = RawValue;
		break;
	case 3:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateMesurment(&TCtypeJ, compensation, TCJCoefCompensation, 4, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('J', &RawValue, &CalcValue);
		break;
	case 4:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeJ, compensation, TCJCoefCompensation, 4, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('J', &RawValue, &CalcValue);
		break;
	case 5:
		RawValue = RawValue * 1000; //
		channel->failureState = TC_CompensateMesurment(&TCtypeK, compensation, TCKCoefCompensation, 6, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('K', &RawValue, &CalcValue);
		break;
	case 6:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeK, compensation, TCKCoefCompensation, 6, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('K', &RawValue, &CalcValue);
		break;
	case 7:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateMesurment(&TCtypeN, compensation, TCNCoefCompensation, 6, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('N', &RawValue, &CalcValue);
		break;
	case 8:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeN, compensation, TCNCoefCompensation, 6, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('N', &RawValue, &CalcValue);
		break;
	case 9:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateMesurment(&TCtypeR, compensation, TCRCoefCompensation, 4, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('R', &RawValue, &CalcValue);
		break;
	case 10:
		RawValue = RawValue * 1000;

		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeR, compensation, TCRCoefCompensation, 4, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('R', &RawValue, &CalcValue);
		break;
	case 11:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateMesurment(&TCtypeS, compensation, TCSCoefCompensation, 4, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('S', &RawValue, &CalcValue);
		break;
	case 12:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeS, compensation, TCSCoefCompensation, 4, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('S', &RawValue, &CalcValue);
		break;
	case 13:
		RawValue = RawValue * 1000;

		channel->failureState = TC_CompensateMesurment(&TCtypeT, compensation, TCTCoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('T', &RawValue, &CalcValue);
		break;
	case 14:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeT, compensation, TCTCoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('T', &RawValue, &CalcValue);
		break;
	case 15:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateMesurment(&TCtypeE, compensation, TCECoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('E', &RawValue, &CalcValue);
		break;
	case 16:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeE, compensation, TCECoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('E', &RawValue, &CalcValue);
		break;
	case 17:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CalculateCelciusTemperature('B', &RawValue, &CalcValue);
		break;
	case 18:
		RawValue = RawValue * 1000;
		channel->failureState = TC_CalculateFarenheitTemperature('B', &RawValue, &CalcValue);
		break;
	case 19:
		channel->failureState = TC_CompensateMesurment(&TCtypeL, compensation, TCLCoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('L', &RawValue, &CalcValue);
		break;
	case 20:
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeL, compensation, TCLCoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('L', &RawValue, &CalcValue);
		break;
	case 21:
		channel->failureState = TC_CompensateMesurment(&TCtypeU, compensation, TCUCoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateCelciusTemperature('U', &RawValue, &CalcValue);
		break;
	case 22:
		channel->failureState = TC_CompensateFarenheitMesurment(&TCtypeU, compensation, TCUCoefCompensation, 3, &RawValue);
		if (OK == channel->failureState)
			channel->failureState = TC_CalculateFarenheitTemperature('U', &RawValue, &CalcValue);
		break;
	default:
		CalcValue = channel->value;
		break;
	}
	return CalcValue;
}

static float CalcIN6DCharValue(CHANNEL * channel, float RawValue, float CorrectionValue, uint inputType)
{
	float CalcValue = 0.0;
	switch (inputType)
	{
	case 1: //STATE
		if (RawValue == 0)
			CalcValue = channel->LowScaleValue;
		else if (RawValue == 1)
			CalcValue = channel->HighScaleValue;
		else
		{
			channel->failureState = ADC_ERROR;
			CalcValue = 0.0;
		}
		break;
	case 2: //FREQENCY
		RawValue = RawValue + CorrectionValue;
		switch (channel->CharacteristicType)
		{
		case 0:
			CalcValue = ((RawValue - channel->LowSignalValue) * (channel->HighScaleValue - channel->LowScaleValue))
					/ (channel->HighSignalValue - channel->LowSignalValue) + channel->LowScaleValue;
			break;
		case 1:
			CalcValue =CalcUserCharValue(RawValue, &UserCharacteristcs[channel->UserCharacteristicType]);
			break;
		case 2:
			CalcValue = RawValue;
			break;
		default:
			CalcValue = channel->value;
			break;
		}
		break;
	case 3: //COUNTER
		CalcValue = RawValue * channel->LowScaleValue;
		break;
	default:
		CalcValue = 0.0;
	}
	return CalcValue;
}

static float hornerf(const float *coeffs, int s, float x)
{
	float res = 0;
	for (int i = 0; i < s + 1; ++i)
		res = res * x + coeffs[i];
	return res;
}

static void FarenheitToCelcius(float *Temperature)
{
	*Temperature = 5 * (*Temperature - 32) / 9;
}

static void CelciusToFarenheit(float * Temperature)
{
	*Temperature = 9 * *Temperature / 5 + 32;
}

float CalculateFilteredValue(CHANNEL * channel, const float *NewValue)
{
	float FliteredValue;
	if (channel->firstIterration == 0)
	{
		if (channel->filterType == 0 || isnan(channel->value))
			FliteredValue = *NewValue;
		else
			FliteredValue = (FilterTimeConstant[channel->filterType] * channel->value + *NewValue)
					/ (FilterTimeConstant[channel->filterType] + 1);
	}
	else
	{
		FliteredValue = *NewValue;
		channel->firstIterration = 0;
	}
	return FliteredValue;
}

float SetTotalizerPeriodValue(uint8_t Period)
{
	switch (Period)
	{
	case 1:
		return 1.0;
	case 2:
		return 60.0;
	case 3:
		return 3600.0;
	default:
		return 1.0;
	}
}

float SetTotalizerMultiplerValue(uint8_t Multipler)
{
	switch (Multipler)
	{
	case 0:
		return 0.001;
	case 1:
		return 1.0;
	case 2:
		return 1000.0;
	default:
		return 1.0;
	}
}

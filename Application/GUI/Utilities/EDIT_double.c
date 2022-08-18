/*
 * EDIT_double.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */
#include "EDIT_double.h"
#include "dtos.h"
#include <stdlib.h>

void EDIT_SetDouble(EDIT_Handle hObj, const double value, const char resolution)
{
	char StringValue[21];
	dbl2stri(StringValue, value, resolution);
	EDIT_SetText(hObj, StringValue);
}

void EDIT_SetFloat(EDIT_Handle hObj, const float value, const char resolution)
{
	char StringValue[21];
	float2stri(StringValue, value, resolution);
	EDIT_SetText(hObj, StringValue);
}

void EDIT_SetInt(EDIT_Handle hObj, const int value)
{
	char StringValue[21];
	itoa(value, StringValue, 10);
	EDIT_SetText(hObj, StringValue);
}


void EDIT_SetShortStringDouble(EDIT_Handle hObj, const double value, const char resolution)
{
	if(value >=99999999999)
		EDIT_SetText(hObj, "-----");
	else if (value <= -9999999999)
		EDIT_SetText(hObj, "-----");
	else
	{
		char StringValue[17];
		dbl2stri(StringValue, value, resolution);
		EDIT_SetText(hObj, StringValue);
	}
}

void EDIT_SetLongStringDouble(EDIT_Handle hObj, const double value, char resolution)
{

	if(value >=1000000000000000)
		EDIT_SetText(hObj, "-----");
	else if (value <= -100000000000000)
		EDIT_SetText(hObj, "-----");
	else
	{
		char StringValue[21];
		if(value>=10000000000000 || value <= -1000000000000)
		{
			resolution = 0;
		}
		else if(value>=1000000000000 || value <= -100000000000)
		{
			if(resolution>1)
				resolution = 1;
		}
		else if(value>=100000000000 || value <= -10000000000)
		{
			if(resolution>2)
				resolution = 2;
		}
		else if(value>=10000000000 || value <= -1000000000)
		{
			if(resolution>3)
				resolution = 3;
		}
		dbl2stri(StringValue, value, resolution);
		EDIT_SetText(hObj, StringValue);
	}
}


void EDIT_SetShortFloat(EDIT_Handle hObj, const float value, char resolution)
{
	char StringValue[21];
	if(value>=1000000000 || value <= -100000000)
	{
		resolution = 0;
	}
	else if(value>=100000000 || value <= -10000000)
	{
		if(resolution>1)
			resolution = 1;
	}
	else if(value>=10000000 || value <= -1000000)
	{
		if(resolution>2)
			resolution = 2;
	}
	else if(value>=1000000 || value <= -100000)
	{
		if(resolution>3)
			resolution = 3;
	}
	float2stri(StringValue, value, resolution);
	EDIT_SetText(hObj, StringValue);
}

void EDIT_SetLongFloat(EDIT_Handle hObj, const float value, char resolution)
{
	char StringValue[21];
	if(value>=10000000000000 || value <= -1000000000000)
	{
		resolution = 0;
	}
	else if(value>=1000000000000 || value <= -100000000000)
	{
		if(resolution>1)
			resolution = 1;
	}
	else if(value>=100000000000 || value <= -10000000000)
	{
		if(resolution>2)
			resolution = 2;
	}
	else if(value>=10000000000 || value <= -1000000000)
	{
		if(resolution>3)
			resolution = 3;
	}
	float2stri(StringValue, value, resolution);
	EDIT_SetText(hObj, StringValue);
}

float EDIT_GetFlaot(EDIT_Handle hObj)
{
	char TextBuff[20];
	EDIT_GetText(hObj,TextBuff,20);
	float ret = atof(TextBuff);
	return ret;
}

void TEXT_SetDouble(TEXT_Handle hObj, const double value, const char resolution)
{
	char StringValue[21];
	dbl2stri(StringValue, value, resolution);
	TEXT_SetText(hObj, StringValue);
}

void TEXT_SetFloat(TEXT_Handle hObj, const float value, const char resolution)
{
	char StringValue[21];

	float2stri(StringValue, value, resolution);
	TEXT_SetText(hObj, StringValue);
}

void TEXT_SetShortFloat(TEXT_Handle hObj, const float value, char resolution)
{
	char StringValue[21];
	if(value>=1000000000 || value <= -100000000)
	{
		resolution = 0;
	}
	else if(value>=100000000 || value <= -10000000)
	{
		if(resolution>1)
			resolution = 1;
	}
	else if(value>=10000000 || value <= -1000000)
	{
		if(resolution>2)
			resolution = 2;
	}
	else if(value>=1000000 || value <= -100000)
	{
		if(resolution>3)
			resolution = 3;
	}
	float2stri(StringValue, value, resolution);
	TEXT_SetText(hObj, StringValue);
}

void TEXT_SetFloatTirmmed(TEXT_Handle hObj, const float value, const char resolution)
{
	char StringValue[21];

	float2striTrimmed(StringValue, value, resolution);
	TEXT_SetText(hObj, StringValue);
}





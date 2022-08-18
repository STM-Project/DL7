#include <stdlib.h>
#include <string.h>
#include "UserLabels.h"
#include "website_process.h"
#include "parameters.h"

#include "version.h"
#include "dtos.h"

#include "mini-printf.h"

int measurement_fulfill(short tabNum, short cellNum, char* str)
{
	USER_LABEL tempTableLabel;
	char val[24];
	int strlength=0;

	short* mResult;
	if(0<=tabNum && tabNum<=6)
		mResult = &UserTablesSettings.TablesData[15*tabNum + cellNum];
	else
	{
		strlength = mini_snprintf(str,2000, "[[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]");
		return strlength;
	}

	strlength = mini_snprintf(str,2000, "[");
	for(int i=0;i<15;i++)
	{
		if(*mResult != -1)
		{
			if(!GetUserLabel(&tempTableLabel, *mResult))
			{
				dbl2stri(val, tempTableLabel.value, tempTableLabel.resolution);
				strlength += mini_snprintf(str + strlength, 2000, "[ \"%s\", \"%s\", \"%s\", %d, %d, %d, %d]", tempTableLabel.description,
											val, tempTableLabel.unit,	(int)tempTableLabel.Color, tempTableLabel.failureState,	tempTableLabel.failureMode,
											tempTableLabel.channelType);
			}
			if(i!=14)
				strlength += mini_snprintf(str + strlength, 2000, ",");
		}
		else
		{
			strlength += mini_snprintf(str + strlength, 2000, "[]");
			if(i!=14)
				strlength += mini_snprintf(str + strlength, 2000, ",");
		}
		mResult++;
	}
	strlength += mini_snprintf(str + strlength, 2000, "]");
	return strlength;
}

int measurement_fulfill_value(short tabNum, short cellNum, char* str)
{
	USER_LABEL tempTableLabel;
	char val[24];
	int strlength=0;

	short* mResult;
	if(0<=tabNum && tabNum<=6)
		mResult = &UserTablesSettings.TablesData[15*tabNum + cellNum];
	else
	{
		strlength = mini_snprintf(str,2000, "[[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]");
		return strlength;
	}

	strlength = mini_snprintf(str,2000, "[");
	for(int i=0;i<15;i++)
	{
		if(*mResult != -1)
		{
			if(GetUserLabelValue(&tempTableLabel, *mResult))
			{
				dbl2stri(val, tempTableLabel.value, tempTableLabel.resolution);
				strlength += mini_snprintf(str + strlength, 2000, "[ \"%s\", %d, %d, %d, %d]",	val, (int)tempTableLabel.Color,
											tempTableLabel.failureState, tempTableLabel.failureMode,	tempTableLabel.channelType);
			}
			if(i!=14)
				strlength += mini_snprintf(str + strlength,2000, ",");
		}
		else
		{
			strlength += mini_snprintf(str + strlength, 2000, "[]");
			if(i!=14)
				strlength += mini_snprintf(str + strlength,2000, ",");
		}
		mResult++;
	}
	strlength += mini_snprintf(str + strlength,2000, "]");
	return strlength;
}

int bar_fulfill(char* str)
{
	int strlength=0;
	strlength = mini_snprintf(str,300, "[");
	for(int i = 0; i < 6 ; i++)
	{
		short k = 0;
		for (int j = 0; j < 15; j++)
			k += UserTablesSettings.TablesData[i * 15 + j];
		if (k == -15)
			strlength += mini_snprintf(str + strlength,300, "0,");
		else
			strlength += mini_snprintf(str + strlength,300, "\"%s\",",UserTablesSettings.TablesTitles[i]);
	}
	mini_snprintf(str + strlength - 1,300, "]");
	return strlength;
}

int main_fulfill(char* str)
{
	int strlength=0;
	strlength = mini_snprintf(str, 300, "{\"title\":\"%s Id: %d\",\"name\":\"%s\",\"tag\":\"%s\",\"ver\":\"%s\",\"lang\":%d}",
							NAME, GeneralSettings.DeviceID,	NAME,	GeneralSettings.DeviceDescription, VERSION,	bkGeneralSettings.DeviceLanguage);
	return strlength;
}

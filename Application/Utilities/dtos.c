#include "dtos.h"
#include <stdint.h>
#include <math.h>
#include <string.h>

const double round_nums[MAX_ROUND_SIZE] =
{ 0.5, 0.05, 0.005, 0.0005, 0.00005,0.000005,0.0000005};

void dbl2stri(char *buffer, double value, unsigned int decDigits)
{
	int idx;
	int64_t dbl_int, dbl_frac;
	int64_t mult = 1;
	char *output = buffer;
	char tbfr[40];

	if (isfinite(value))
	{
		if ((value <= -99999999999999) || (value >= 99999999999999))
		{
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
		}
		else
		{
			if (value < 0.0)
			{
				*output++ = '-';
				value *= -1.0;
			}

			if (decDigits < MAX_ROUND_SIZE)
			{

				value += round_nums[decDigits];
				for (idx = 0; idx < decDigits; idx++)
					mult *= 10;
			}
			else
			{
				decDigits = 5;
				value += round_nums[6];
				mult = 100000;
			}

			dbl_int = (int64_t) value;
			dbl_frac = (int64_t) ((value - (double) dbl_int) * (double) mult);

			idx = 0;
			while (dbl_int != 0)
			{
				tbfr[idx++] = '0' + (dbl_int % 10);
				dbl_int /= 10;
			}

			if (idx == 0)
				*output++ = '0';
			else
			{
				while (idx > 0)
				{
					*output++ = tbfr[idx - 1];
					idx--;
				}
			}

			if (decDigits > 0)
			{
				*output++ = '.';

				idx = 0;
				while (dbl_frac != 0)
				{
					tbfr[idx++] = '0' + (dbl_frac % 10);
					dbl_frac /= 10;
				}
				while (idx < decDigits)
					tbfr[idx++] = '0';

				if (idx == 0)
					*output++ = '0';
				else
				{
					while (idx > 0)
					{
						*output++ = tbfr[idx - 1];
						idx--;
					}
				}
			}
		}
	}
	else
	{
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
	}
	*output = 0;
}

void float2stri(char *buffer, float value, unsigned int decDigits)
{
	int idx;
	int64_t dbl_int, dbl_frac;
	int64_t mult = 1;
	char *output = buffer;
	char tbfr[40];

	if (isfinite(value))
	{
		if ((value <= -99999999999999) || (value >= 99999999999999))
		{
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
			*output++ = '-';
		}
		else
		{
			if (value < 0.0)
			{
				*output++ = '-';
				value *= -1.0;
			}

			if (decDigits < MAX_ROUND_SIZE)
			{

				value += round_nums[decDigits];
				for (idx = 0; idx < decDigits; idx++)
					mult *= 10;
			}
			else
			{
				decDigits = 5;
				value += round_nums[6];
				mult = 100000;
			}

			dbl_int = (int64_t) value;
			dbl_frac = (int64_t) ((value - (double) dbl_int) * (double) mult);

			idx = 0;
			while (dbl_int != 0)
			{
				tbfr[idx++] = '0' + (dbl_int % 10);
				dbl_int /= 10;
			}

			if (idx == 0)
				*output++ = '0';
			else
			{
				while (idx > 0)
				{
					*output++ = tbfr[idx - 1];
					idx--;
				}
			}

			if (decDigits > 0)
			{
				*output++ = '.';

				idx = 0;
				while (dbl_frac != 0)
				{
					tbfr[idx++] = '0' + (dbl_frac % 10);
					dbl_frac /= 10;
				}
				while (idx < decDigits)
					tbfr[idx++] = '0';

				if (idx == 0)
					*output++ = '0';
				else
				{
					while (idx > 0)
					{
						*output++ = tbfr[idx - 1];
						idx--;
					}
				}
			}
		}
	}
	else
	{
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
		*output++ = '-';
	}
	*output = 0;
}

void dbl2striTrimmed(char *buffer, double value, unsigned int decDigits)
{
	dbl2stri(buffer, value, decDigits);
	int stringLength = strlen(buffer) - 1;
	while(stringLength>=0)
	{
		if(buffer[stringLength] == '0')
		{
			buffer[stringLength] = 0;
			--stringLength;
		}
		else if(buffer[stringLength] == '.')
		{
			buffer[stringLength+1] = '0';
			break;
		}
		else
		{
			break;
		}
	}
}

void float2striTrimmed(char *buffer, float value, unsigned int decDigits)
{
	float2stri(buffer, value, decDigits);
	int stringLength = strlen(buffer) - 1;
	while(stringLength>=0)
	{
		if(buffer[stringLength] == '0')
		{
			buffer[stringLength] = 0;
			--stringLength;
		}
		else if(buffer[stringLength] == '.')
		{
			buffer[stringLength+1] = '0';
			break;
		}
		else
		{
			break;
		}
	}
}


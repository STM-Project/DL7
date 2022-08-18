/*
 * RTD.h
 *
 *  Created on: 17 wrz 2015
 *      Author: Tomaszs
 */

#ifndef INC_CHARCOEFFICIENTS_RTD_H_
#define INC_CHARCOEFFICIENTS_RTD_H_

typedef struct{
float min;
float max;

}RTD_RANGE;

const RTD_RANGE PtRange = {18.52, 390.481};
const RTD_RANGE NiRange = {69.52, 289.16};
const RTD_RANGE CuRange = {10.264, 92.8};
const RTD_RANGE KTY81Range = {490.0, 2211.0};
const RTD_RANGE KTY83Range = {500.0, 2535.0};
const RTD_RANGE KTY84Range = {359.0, 2624.0};

const float Pt100Coef[7] =
		{
				6.717179e-14,
				-9.214219e-11,
				5.189297e-08,
				-1.403443e-05,
				2.966861e-03,
				2.221168,
				-2.421271e2
		};

//zakres -60° C .. +250° C
const float Ni100Coef[6] =
		{
				0.0000000001096112,
				-0.0000001128072,
				0.00004858441,
				-0.01296324,
				3.357806,
				-244.5529
		};

//zakres -180° C .. 200° C
const float Cu50Coef[6] =
		{
				0.000000009957121,
				-0.000002393127,
				0.0001760483,
				-0.001302648,
				4.356662,
				-224.7562
		};

const float KTY81Coef[6] =
{
		1.588561E-14,
		-1.077078E-10,
		2.969300E-07,
		-4.399146E-04,
		4.672893E-01,
		-207.5343
};

const float KTY83Coef[6] =
{
		3.676253e-15,
		-3.347814e-11,
		1.241736e-07,
		-2.491143e-04,
		3.719554e-01,
		-192.1264
};

const float KTY84Coef[6] =
{
		8.907741e-15,
		-7.330942e-11,
		2.423869e-07,
		-4.246555e-04,
		5.327946e-01,
		-186.0705
		};

#endif /* INC_CHARCOEFFICIENTS_RTD_H_ */

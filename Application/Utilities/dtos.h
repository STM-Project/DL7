/*
 * dtos.h
 *
 *  Created on: 14 paź 2016
 *      Author: Tomaszs
 */

#ifndef UTILITIES_DTOS_H_
#define UTILITIES_DTOS_H_

#define MAX_ROUND_SIZE	7

void dbl2stri(char *buffer, double value, unsigned int decDigits);
void float2stri(char *buffer, float value, unsigned int decDigits);

void dbl2striTrimmed(char *buffer, double value, unsigned int decDigits);
void float2striTrimmed(char *buffer, float value, unsigned int decDigits);

#endif /* UTILITIES_DTOS_H_ */

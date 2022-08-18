/*
 * EDIT_double.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_UTILITIES_EDIT_DOUBLE_H_
#define GUI_UTILITIES_EDIT_DOUBLE_H_

#include "EDIT.h"
#include "TEXT.h"

void EDIT_SetDouble(EDIT_Handle hObj, const double value, const char resolution);
void EDIT_SetFloat(EDIT_Handle hObj, const float value, const char resolution);
void EDIT_SetInt(EDIT_Handle hObj, const int value);

void EDIT_SetShortStringDouble(EDIT_Handle hObj, const double value, char resolution);
void EDIT_SetLongStringDouble(EDIT_Handle hObj, const double value, char resolution);


void EDIT_SetShortFloat(EDIT_Handle hObj, const float value, char resolution);
void EDIT_SetLongFloat(EDIT_Handle hObj, const float value, char resolution);
float EDIT_GetFlaot(EDIT_Handle hObj);

void TEXT_SetFloat(TEXT_Handle hObj, const float value, const char resolution);
void TEXT_SetShortFloat(TEXT_Handle hObj, const float value, char resolution);
void TEXT_SetDouble(TEXT_Handle hObj, const double value, const char resolution);

void TEXT_SetFloatTirmmed(TEXT_Handle hObj, const float value, const char resolution);

#endif /* GUI_UTILITIES_EDIT_DOUBLE_H_ */

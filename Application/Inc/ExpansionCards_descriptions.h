/*
 * ExpansionCards_descriptions.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_UTILITIES_EXPANSIONCARDS_DESCRIPTIONS_H_
#define GUI_UTILITIES_EXPANSIONCARDS_DESCRIPTIONS_H_

#include "TEXT.h"
#include "DROPDOWN.h"

const char * GetExpansionCardDescription(unsigned int cardType);
void SetCardUnit(TEXT_Handle hObj, unsigned char cardType,
		unsigned char inputMode);
void FillInputModeList(DROPDOWN_Handle hItem, unsigned char BoardType);

#endif /* GUI_UTILITIES_EXPANSIONCARDS_DESCRIPTIONS_H_ */

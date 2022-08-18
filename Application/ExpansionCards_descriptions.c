/*
 * ExpansionCards_descriptions.c
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#include <ExpansionCards_typedef.h>
#include "ExpansionCards_descriptions.h"
#include <string.h>

#define EXPANSION_CARD_DESCRYPTION_TABLE_SIZE 21

const char ExpansionCardDescryption[EXPANSION_CARD_DESCRYPTION_TABLE_SIZE][12] =
{ "-------", "IN6I(24V)", "IN3I(24V)", "IN6I", "IN3I", "IN6TC", "IN3TC", "IN6V", "IN3V", "IN6RTD",
	"IN3RTD", "IN6", "IN3", "IN6D", "OUT6RL",	"OUT3", "2RS485", "2RS485(24V)", "1HRT", "PSBATT", "IN4SG" };

const char * GetExpansionCardDescription(unsigned int cardType)
{
	if (cardType < EXPANSION_CARD_DESCRYPTION_TABLE_SIZE)
		return ExpansionCardDescryption[cardType];
	else
		return ExpansionCardDescryption[0];
}

static void SetIN3CardUnit(TEXT_Handle hObj, uint8_t inputMode)
{
	switch (inputMode)
	{
	case 1:
	case 2:
		TEXT_SetText(hObj, "mA");
		break;
	case 3:
	case 4:
		TEXT_SetText(hObj, "V");
		break;
	case 5:
		TEXT_SetText(hObj, "mV");
		break;
	case 6:
	case 7:
	case 8:
		TEXT_SetText(hObj, "Ω");
		break;
	case 0xF0:
		TEXT_SetText(hObj, "°C");
		break;
	case 0xF1:
		TEXT_SetText(hObj, "°F");
		break;
	default:
		TEXT_SetText(hObj, " ");
	}
}

static void SetIN6DCardUnit(TEXT_Handle hObj, uint8_t inputMode)
{
	switch (inputMode)
	{
	case 2:
		TEXT_SetText(hObj, "Hz");
		break;
	case 3:
		TEXT_SetText(hObj, "imp.");
		break;
	default:
		TEXT_SetText(hObj, " ");
	}
}

static void SetOUT3CardUnit(TEXT_Handle hObj, uint8_t inputMode)
{
	switch (inputMode)
	{
	case 1:
	case 2:
	case 3:
		TEXT_SetText(hObj, "mA");
		break;
	case 4:
	case 5:
		TEXT_SetText(hObj, "V");
		break;
	default:
		TEXT_SetText(hObj, " ");
	}
}

void SetCardUnit(TEXT_Handle hObj, unsigned char cardType, unsigned char inputMode)
{
	switch (cardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
		TEXT_SetText(hObj, "mA");
		break;
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		TEXT_SetText(hObj, "Ω");
		break;
	case SPI_CARD_IN6V:
		TEXT_SetText(hObj, "V");
		break;
	case SPI_CARD_IN6TC:
		if (0xF0 == inputMode)
			TEXT_SetText(hObj, "°C");
		else if (0xF1 == inputMode)
			TEXT_SetText(hObj, "°F");
		else
			TEXT_SetText(hObj, "mV");
		break;
	case SPI_CARD_IN4SG:
			TEXT_SetText(hObj, "mV");
		break;
	case SPI_CARD_IN3:
		SetIN3CardUnit(hObj, inputMode);
		break;
	case SPI_CARD_IN6D:
		SetIN6DCardUnit(hObj, inputMode);
		break;
	case SPI_CARD_OUT3:
		SetOUT3CardUnit(hObj, inputMode);
		break;
	default:
		TEXT_SetText(hObj, " ");
	}
}

void FillInputModeList( DROPDOWN_Handle hItem, unsigned char BoardType)
{
	char GUITextBuffer[20];
	while (DROPDOWN_GetNumItems(hItem))
	{
		DROPDOWN_DeleteItem(hItem, 0);
	}
	switch (BoardType)
	{
	case SPI_CARD_IN6I24:
	case SPI_CARD_IN6I:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, "0-20mA");
		DROPDOWN_AddString(hItem, "4-20mA");
		break;
	case SPI_CARD_IN6RTD:
	case SPI_CARD_IN3RTD:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		strcpy(GUITextBuffer, "2 ");
		strcat(GUITextBuffer,GUI_LANG_GetText(153));
		DROPDOWN_AddString(hItem, GUITextBuffer);
		strcpy(GUITextBuffer, "3 ");
		strcat(GUITextBuffer,GUI_LANG_GetText(153));
		DROPDOWN_AddString(hItem, GUITextBuffer);
		strcpy(GUITextBuffer, "4 ");
		strcat(GUITextBuffer,GUI_LANG_GetText(153));
		DROPDOWN_AddString(hItem, GUITextBuffer);
		break;
	case SPI_CARD_IN6TC:
	case SPI_CARD_IN4SG:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(154));
		break;
	case SPI_CARD_IN6V:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, "-10V - +10V");
		DROPDOWN_AddString(hItem, " 0-10V");
		break;
	case SPI_CARD_OUT6RL:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(155));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(156));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(157));
		break;
	case SPI_CARD_IN3:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, "0-20mA");
		DROPDOWN_AddString(hItem, "4-20mA");
		DROPDOWN_AddString(hItem, "-10V - +10V");
		DROPDOWN_AddString(hItem, " 0-10V");
		DROPDOWN_AddString(hItem, "TC");
		strcpy(GUITextBuffer, "RTD 2 ");
		strcat(GUITextBuffer,GUI_LANG_GetText(153));
		DROPDOWN_AddString(hItem, GUITextBuffer);
		strcpy(GUITextBuffer, "RTD 3 ");
		strcat(GUITextBuffer,GUI_LANG_GetText(153));
		DROPDOWN_AddString(hItem, GUITextBuffer);
		strcpy(GUITextBuffer, "RTD 4 ");
		strcat(GUITextBuffer,GUI_LANG_GetText(153));
		DROPDOWN_AddString(hItem, GUITextBuffer);
		break;
	case SPI_CARD_IN6D:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(181));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(182));
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(183));
		break;
	case SPI_CARD_OUT3:
		DROPDOWN_AddString(hItem, GUI_LANG_GetText(27));
		break;
	default:
		break;
	}
}

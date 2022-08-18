/*
 * ExpansionCards.h
 *
 *  Created on: 22.05.2017
 *      Author: Tomaszs
 */

#ifndef EXPANSIONCARDS_H_
#define EXPANSIONCARDS_H_

#include <ExpansionCards_typedef.h>

#define NUMBER_OF_CARD_SLOTS 7

extern EXPANSION_CARD ExpansionCards[];
extern EXPANSION_CARD bkExpansionCards[];

void IOBoards_InitalizeBoards(void);
void IOBoards_InitalizeBoard(uint8_t boardNumber, EXPANSION_CARD *card);

void IOBoards_ReadBoardID(EXPANSION_CARD * expansionCard, uint8_t boardNumber);

void IOBoards_ReadFromInputBoards(EXPANSION_CARD *expansionCard, uint8_t cardNumber);
void IOBoards_ReadFromIN6DBoards(EXPANSION_CARD *expansionCard, uint8_t boardNumber);
void IOBoards_ReadFrom2RS485Board(EXPANSION_CARD *expansionCard, uint8_t boardNumber);
void IOBoards_ReadFromHARTBoard(EXPANSION_CARD *expansionCard, uint8_t boardNumber);
void IOBoards_ReadFromTCBoard(EXPANSION_CARD *expansionCard, uint8_t boardNumber);
void IOBoards_WriteToOutputBoards(EXPANSION_CARD *expansionCard, uint8_t boardNumber);
void IOBoards_WriteToOUT3Board(EXPANSION_CARD *expansionCard, uint8_t boardNumber);
void IOBoards_WriteSettingsToBoard(const EXPANSION_CARD * expansionCard);

void IOBoards_Initalize(void);
void IOBoards_ChipSelect(uint8_t boardNumber);
void IOBoards_SetChipSelect(uint8_t boardNumber);
void IOBoards_DisableChipSelect(void);
void IOBoards_EnableChipSelect(void);

void IOBoards_ReadCalibrationFromBoard(CALIBRATION_StructTypeDef * calibrationData, uint8_t boardNumber, uint8_t InputMode);
void Calibration_Write(CALIBRATION_StructTypeDef * calibrationData, uint8_t BoardNumber, uint8_t InputMode);
void Calibration_ReadValues(float *CalValue, float *RawValue, uint8_t IONumber, uint8_t boardNumber);
void SetBoardToCalibartionMode(uint8_t boardNumber, uint8_t InputsMode);
uint8_t GetActiveCalibrationPointsMode(uint8_t boardNumber, uint8_t inputsMode);
void HART_GetLongAddr(uint8_t boardNumber, uint8_t shortAddr, uint8_t *ptr);
void HART_SendCommand_ID(uint8_t boardNumber, uint8_t shortAddr);
void HART_ChangeAddress(uint8_t boardNumber, uint64_t addr, uint8_t newAddr);
void HART_Init2SendCommandGetLongAddr(uint8_t boardNumber, uint8_t InputsMode);
void HART_SendRequestReadID(uint8_t boardNumber, uint8_t shortAddr);
void HART_ServiceReadValue(char *CalValue, uint8_t boardNumber);
void HART_SendCmdResistorOnOff(uint8_t boardNumber, uint8_t resistorOnOff);

uint8_t GetBoardType(uint8_t boardNumber);
void GetExpansionBoard(uint8_t boardNumber, EXPANSION_CARD * Board);

void IOBoards_PowerOn(uint8_t boardNumber);
void IOBoards_PowerOff(uint8_t boardNumber);

void ResetIN6DCardsValues(void);

#endif /* EXPANSIONCARDS_H_ */

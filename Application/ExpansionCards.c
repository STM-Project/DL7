#include "ExpansionCards.h"
#include "spi.h"
#include <string.h>
#include "stm32h7xx_hal.h"
#include "shift_reg.h"
#include "rtc.h"
#include "spi.h"
#include "PopUpMessageDLG.h"
#include "archive.h"
#include "mini-printf.h"

#define RESET_BOARD

#define SIZEOF_SPI_CODES 20
#define CMD_LENGTH                (uint8_t) 0x02
#define ANSW_DATA_LENGTH				((uint16_t) 0x0020)
#define MAX_DATA_LENGHT					((uint16_t) 500)
#define HEADER_LENGHT       		((uint8_t)  0x06)
#define FRAME_LENGHT       		 (HEADER_LENGHT+MAX_DATA_LENGHT)

#define ADDRCMD_RD_ID             			(uint8_t) 0x01
#define ADDRCMD_WR_CONF_IN_B      			(uint8_t) 0x02
#define ADDRCMD_WR_CONF_RS_B	  				(uint8_t) 0x03
#define ADDRCMD_WR_CONF_OUT_B     			(uint8_t) 0x04
#define ADDRCMD_RD_RAW_VAL     					(uint8_t) 0x10
#define ADDRCMD_RD_CALIBR_VAL    				(uint8_t) 0x11
#define ADDRCMD_RD_VALUES_RS	  				(uint8_t) 0x12
#define ADDRCMD_WR_STATES		  					(uint8_t) 0x20
#define ADDRCMD_WR_VALUES		  					(uint8_t) 0x21
#define ADDRCMD_WR_CAL_POINTS     			(uint8_t) 0xA1
#define ADDRCMD_RD_CAL_POINTS     			(uint8_t) 0xA0
#define ADDRCMD_WR_VALUES_HART		  		(uint8_t) 0x22
#define ADDRCMD_WR_CMD_ID			  				(uint8_t) 0x23
#define ADDRCMD_RD_HART_LONG_ADDR		  	(uint8_t) 0x24
#define ADDRCMD_WR_HART_CMD_CHANGE_ADDR	(uint8_t) 0x25
#define ADDRCMD_WR_HART_CMD_RESISTOR		(uint8_t) 0x26
#define ADDRCMD_WR_CONF_HART	      		(uint8_t) 0x027
#define ADDRCMD_RD_VALUE_HART	      		(uint8_t) 0x028
#define ADDRCMD_RD_VALUE_TC	      			(uint8_t) 0x029
#define ADDRCMD_RD_HART_FRAME	      		(uint8_t) 0x02A

static void IOBoards_WriteSettingsToOUT6RLBoard(const EXPANSION_CARD * expansionCard);
static void IOBoards_WriteSettingsToOUT3Board(const EXPANSION_CARD * expansionCard);
static void IOBoards_WriteSettingsToInputBoard(const EXPANSION_CARD * expansionCard);
static void IOBoards_WriteSettingsToRS485Board(const EXPANSION_CARD * expansionCard);

static void IOBoards_HandleBoardError(EXPANSION_CARD *expansionCard, uint16_t boardNumber);
static uint8_t IOBoards_SetNumberOfBytesToSPICommunication(uint8_t functionCode);

static uint8_t IOBoards_ReadData(uint8_t functionCode, uint8_t subfunctionCode);
static uint8_t IOBoards_WriteData(uint8_t functionCode, uint8_t subfunctionCode);
static uint8_t IOBoards_WriteCalibrationData(uint8_t InputMode);

static uint8_t IOBoards_WriteCalibrationToBoard(CALIBRATION_StructTypeDef * calibrationData, uint8_t boardNumber, uint8_t InputMode);

static void DelayNOP(int x);
static uint8_t IOBoards_WaitForSPIStateReady(void);

static void SendNoCardErrorMessage(uint8_t boardNumber);
static void SendConfigurationCardErrorMessage(uint8_t boardNumber);

static const SPI_FUNC_StructTypeDef SPIFunctionCodesAndByteCounter[SIZEOF_SPI_CODES] =
{
	{ ADDRCMD_RD_ID, 5 },
	{ ADDRCMD_WR_CONF_IN_B, 8 },
	{ ADDRCMD_RD_RAW_VAL, 32 },
	{ ADDRCMD_RD_CALIBR_VAL, 32 },
	{ ADDRCMD_WR_CONF_RS_B, 187 },
	{ ADDRCMD_RD_VALUES_RS, 127 },
	{ ADDRCMD_WR_STATES, 8 },
	{ ADDRCMD_WR_CAL_POINTS, 58 },
	{ ADDRCMD_RD_CAL_POINTS, 58 },
	{ ADDRCMD_WR_CONF_OUT_B, 8 },
	{ ADDRCMD_WR_VALUES, 14 },
	{ ADDRCMD_WR_VALUES_HART, 32 },
	{ ADDRCMD_WR_CMD_ID, 3 },
	{ ADDRCMD_RD_HART_LONG_ADDR, 10 },
	{ ADDRCMD_WR_HART_CMD_CHANGE_ADDR, 8 },
	{ ADDRCMD_WR_HART_CMD_RESISTOR, 3 },
	{ ADDRCMD_WR_CONF_HART, 235 },
	{ ADDRCMD_RD_VALUE_HART, 127 },
	{ ADDRCMD_RD_VALUE_TC, 42 },
	{ ADDRCMD_RD_HART_FRAME, 242 },
};

static const IO_CHANNEL_SETTINGS defaultIOchannelSettings =
{ 0, 0.0, 0.0, 0 };

static const AO_CHANNEL_SETTINGS defaultAOchannelSettings =
{ 0, 0, 0, 0.0, 0.0, 0.0 };

static const RS_CHANNEL_SETTINGS defaultRSchannelSettings =
{ 0, 1, 30000, 0 };

static const RS_PORT_SETTINGS defaultRSportSettings =
{ 5, 2, 0, 2000 };

static CALIBRATION_StructTypeDef localCalibration =
{ 0, 1, 0, 0, 17, 1, 1, 0, 0, 0,
{
{ 1.0, 0.0 },
{ 1.0, 0.0 },
{ 1.0, 0.0 },
{ 1.0, 0.0 },
{ 1.0, 0.0 },
{ 1.0, 0.0 } } };

static const HART_READ_CHANNEL defaultHARTchannelSettings =
{ 0, 0, 0, 0, 0 };

static const HART_SENSOR_SETTINGS defaultHARTSensors =
{ 0, 0, 0 };

EXPANSION_CARD bkExpansionCards[7] __attribute__ ((section(".sdram")));
EXPANSION_CARD ExpansionCards[7] __attribute__ ((section(".sdram")));

volatile uint8_t SPITxBuffer[FRAME_LENGHT] =
{ 0 };
volatile uint8_t SPIRxBuffer[FRAME_LENGHT] =
{ 0 };

void SetDefaultCardParameters(EXPANSION_CARD *card, uint8_t type)
{
	if (type == SPI_CARD_2RS485 || type == SPI_CARD_2RS485I24)
	{
		card->settings.RScard.port[0] = defaultRSportSettings;
		card->settings.RScard.port[1] = defaultRSportSettings;
		for (int i = 0; i < 25; ++i)
			card->settings.RScard.channels[i] = defaultRSchannelSettings;
	}
	else if (type == SPI_CARD_OUT3)
	{
		for (int i = 0; i < 3; ++i)
			card->settings.OUTcard.channels[i] = defaultAOchannelSettings;
	}
	else if (type == SPI_CARD_1HRT)
	{
		card->settings.HARTcard.isSecondary = 0;
		card->settings.HARTcard.isResistor = 0;
		card->settings.HARTcard.preambleLenght = 5;
		for (int i = 0; i < 25; ++i)
			card->settings.HARTcard.readChannel[i] = defaultHARTchannelSettings;
		for (int i = 0; i < MAX_HART_SENSOR_FOR_CARD; ++i)
			card->settings.HARTcard.sensors[i] = defaultHARTSensors;
	}
	else if(SPI_CARD_IN6TC == type)
	{
		card->settings.IOcard.generalSet = 0;
		for (int i = 0; i < 8; ++i)
			card->settings.IOcard.channels[i] = defaultIOchannelSettings;
	}
	else
	{
		card->settings.IOcard.generalSet = 0;
		for (int i = 0; i < 6; ++i)
			card->settings.IOcard.channels[i] = defaultIOchannelSettings;
	}
}

void IOBoards_InitalizeBoard(uint8_t boardNumber, EXPANSION_CARD *card)
{
	uint8_t isSuccess = ERROR;

	IOBoards_ChipSelect(boardNumber);
	isSuccess = IOBoards_ReadData(ADDRCMD_RD_ID, 0);
	IOBoards_DisableChipSelect();
	if (isSuccess == SUCCESS)
	{
		ExpansionCards[boardNumber].status = SPI_BOARD_OK;
		ExpansionCards[boardNumber].CommunicationErrorCounter = 0;
		if (ExpansionCards[boardNumber].type != SPIRxBuffer[2])
		{
			if ((ExpansionCards[boardNumber].type == SPI_CARD_2RS485 && SPIRxBuffer[2] == SPI_CARD_2RS485I24)
					|| (ExpansionCards[boardNumber].type == SPI_CARD_2RS485I24 && SPIRxBuffer[2] == SPI_CARD_2RS485))
			{

			}
			else
			{
				SetDefaultCardParameters(&ExpansionCards[boardNumber], SPIRxBuffer[2]);
				SendConfigurationCardErrorMessage(boardNumber);
			}

		}
		ExpansionCards[boardNumber].type = SPIRxBuffer[2];
		ExpansionCards[boardNumber].firmwareRevision = (uint16_t) (SPIRxBuffer[3] << 8 | SPIRxBuffer[4]);
	}
	else
	{
		ExpansionCards[boardNumber].status = SPI_BOARD_INACTIVE;

		if (ExpansionCards[boardNumber].type != 0)
		{
			SetDefaultCardParameters(&ExpansionCards[boardNumber], ExpansionCards[boardNumber].type);
			SendNoCardErrorMessage(boardNumber);
		}
		ExpansionCards[boardNumber].type = 0;
		ExpansionCards[boardNumber].firmwareRevision = 0;

#ifdef RESET_BOARD
		IOBOARD_PWR_OFF(boardNumber);
#endif
	}
	bkExpansionCards[boardNumber] = ExpansionCards[boardNumber];
	*card = ExpansionCards[boardNumber];
}

void IOBoards_InitalizeBoards(void)
{
	uint8_t boardNumber = BOARD_A;
	uint8_t isSuccess = ERROR;

	for (boardNumber = BOARD_A; boardNumber <= BOARD_G; boardNumber++)
	{
		IOBoards_ChipSelect(boardNumber);
		isSuccess = IOBoards_ReadData(ADDRCMD_RD_ID, 0);
		IOBoards_DisableChipSelect();
		if (isSuccess == SUCCESS)
		{
			ExpansionCards[boardNumber].status = SPI_BOARD_OK;
			ExpansionCards[boardNumber].CommunicationErrorCounter = 0;
			if (ExpansionCards[boardNumber].type != SPIRxBuffer[2])
			{
				if ((ExpansionCards[boardNumber].type == SPI_CARD_2RS485 && SPIRxBuffer[2] == SPI_CARD_2RS485I24)
						|| (ExpansionCards[boardNumber].type == SPI_CARD_2RS485I24 && SPIRxBuffer[2] == SPI_CARD_2RS485))
				{

				}
				else
				{
					SetDefaultCardParameters(&ExpansionCards[boardNumber], SPIRxBuffer[2]);
					SendConfigurationCardErrorMessage(boardNumber);
				}

			}
			ExpansionCards[boardNumber].type = SPIRxBuffer[2];
			ExpansionCards[boardNumber].firmwareRevision = (uint16_t) (SPIRxBuffer[3] << 8 | SPIRxBuffer[4]);
		}
		else
		{
			ExpansionCards[boardNumber].status = SPI_BOARD_INACTIVE;

			if (ExpansionCards[boardNumber].type != 0)
			{
				SetDefaultCardParameters(&ExpansionCards[boardNumber], ExpansionCards[boardNumber].type);
				SendNoCardErrorMessage(boardNumber);
			}
			ExpansionCards[boardNumber].type = 0;
			ExpansionCards[boardNumber].firmwareRevision = 0;

#ifdef RESET_BOARD
			IOBOARD_PWR_OFF(boardNumber);
#endif
		}
		bkExpansionCards[boardNumber] = ExpansionCards[boardNumber];
	}
}

void IOBoards_ReadBoardID(EXPANSION_CARD * expansionCard, uint8_t boardNumber)
{
	if (SUCCESS == IOBoards_ReadData(ADDRCMD_RD_ID, 0))
		expansionCard->status = SPI_BOARD_OK;
	else
	{
		expansionCard->status = SPI_BOARD_ERROR;
#ifdef RESET_BOARD
		IOBOARD_PWR_OFF(boardNumber);
#endif
	}
}

static void IOBoards_WriteSettingsToInputBoard(const EXPANSION_CARD * expansionCard)
{
	if (expansionCard->status == SPI_BOARD_OK)
	{
		memset((uint8_t *) &SPITxBuffer[2], 0, 6);
		for (uint8_t i = 0; i < 6; i++)
			SPITxBuffer[2 + i] = expansionCard->settings.IOcard.channels[i].type;
		IOBoards_WriteData(ADDRCMD_WR_CONF_IN_B, expansionCard->settings.IOcard.generalSet);
	}
}

static void IOBoards_WriteSettingsToOUT6RLBoard(const EXPANSION_CARD * expansionCard)
{
	if (expansionCard->status == SPI_BOARD_OK)
	{
		memset((uint8_t *) &SPITxBuffer[2], 0, 6);
		for (uint8_t i = 0; i < 6; i++)
			SPITxBuffer[2 + i] = expansionCard->settings.IOcard.channels[i].type;
		IOBoards_WriteData(ADDRCMD_WR_CONF_IN_B, 0);
	}
}

static void IOBoards_WriteSettingsToOUT3Board(const EXPANSION_CARD * expansionCard)
{
	if (expansionCard->status == SPI_BOARD_OK)
	{
		memset((uint8_t *) &SPITxBuffer[2], 0, 6);

		if (100 == expansionCard->firmwareRevision)
		{
			for (uint8_t i = 0; i < 3; ++i)
				SPITxBuffer[2 + i] = expansionCard->settings.OUTcard.channels[2 - i].type;
		}
		else
		{
			for (uint8_t i = 0; i < 3; ++i)
				SPITxBuffer[2 + i] = expansionCard->settings.OUTcard.channels[i].type;
		}
		IOBoards_WriteData(ADDRCMD_WR_CONF_OUT_B, 0);
	}
}

static void IOBoards_WriteSettingsToHARTBoard(const EXPANSION_CARD * expansionCard)
{
	int i, j;
	if (expansionCard->status == SPI_BOARD_OK)
	{
		i = 0;
		SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.isSecondary;
		SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.isResistor;
		SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.preambleLenght;

		for (j = 0; j < 25; j++)
		{
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.readChannel[j].sensorIdx;
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.readChannel[j].valueType;
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.readChannel[j].commandType;
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.readChannel[j].valueAddr;
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.readChannel[j].statusMode;

		}
		for (j = 0; j < MAX_HART_SENSOR_FOR_CARD; j++)
		{
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.sensors[j].activity;
			SPITxBuffer[2 + i++] = expansionCard->settings.HARTcard.sensors[j].hartRev;
			SPITxBuffer[2 + i++] = (uint8_t) (expansionCard->settings.HARTcard.sensors[j].address >> 32);
			SPITxBuffer[2 + i++] = (uint8_t) (expansionCard->settings.HARTcard.sensors[j].address >> 24);
			SPITxBuffer[2 + i++] = (uint8_t) (expansionCard->settings.HARTcard.sensors[j].address >> 16);
			SPITxBuffer[2 + i++] = (uint8_t) (expansionCard->settings.HARTcard.sensors[j].address >> 8);
			SPITxBuffer[2 + i++] = (uint8_t) (expansionCard->settings.HARTcard.sensors[j].address >> 0);

		}
		IOBoards_WriteData(ADDRCMD_WR_CONF_HART, 0);
	}
}

void IOBoards_WriteSettingsToRS485Board(const EXPANSION_CARD * expansionCard)
{
	if (expansionCard->status == SPI_BOARD_OK)
	{

		for (uint8_t i = 0; i < 2; i++)
		{
			SPITxBuffer[2 + 5 * i] = expansionCard->settings.RScard.port[i].baudrate;
			SPITxBuffer[3 + 5 * i] = expansionCard->settings.RScard.port[i].parity;
			SPITxBuffer[4 + 5 * i] = expansionCard->settings.RScard.port[i].stopBits;
			SPITxBuffer[5 + 5 * i] = *((uint8_t *) &expansionCard->settings.RScard.port[i].timeout);
			SPITxBuffer[6 + 5 * i] = *((uint8_t *) &expansionCard->settings.RScard.port[i].timeout + 1);
		}

		for (uint8_t i = 0; i < 25; i++)
		{
			SPITxBuffer[12 + 7 * i] = expansionCard->settings.RScard.channels[i].port;
			SPITxBuffer[13 + 7 * i] = expansionCard->settings.RScard.channels[i].deviceAdr;
			SPITxBuffer[14 + 7 * i] = expansionCard->settings.RScard.channels[i].registerType;
			SPITxBuffer[15 + 7 * i] = *((uint8_t *) &expansionCard->settings.RScard.channels[i].registerAdr);
			SPITxBuffer[16 + 7 * i] = *((uint8_t *) &expansionCard->settings.RScard.channels[i].registerAdr + 1);
			SPITxBuffer[17 + 7 * i] = *((uint8_t *) &expansionCard->settings.RScard.channels[i].registerAdr + 2);
			SPITxBuffer[18 + 7 * i] = *((uint8_t *) &expansionCard->settings.RScard.channels[i].registerAdr + 3);
		}
		IOBoards_WriteData(ADDRCMD_WR_CONF_RS_B, 0);
	}
}

void IOBoards_WriteSettingsToBoard(const EXPANSION_CARD * expansionCard)
{
	if (expansionCard->status == SPI_BOARD_OK)
	{
		switch (expansionCard->type)
		{
		case SPI_CARD_IN6I24:
		case SPI_CARD_IN4SG:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_IN6I:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_IN6V:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_IN6TC:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_IN6RTD:
		case SPI_CARD_IN3RTD:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_OUT6RL:
			IOBoards_WriteSettingsToOUT6RLBoard(expansionCard);
			break;
		case SPI_CARD_2RS485:
		case SPI_CARD_2RS485I24:
			IOBoards_WriteSettingsToRS485Board(expansionCard);
			break;
		case SPI_CARD_IN3:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_IN6D:
			IOBoards_WriteSettingsToInputBoard(expansionCard);
			break;
		case SPI_CARD_OUT3:
			IOBoards_WriteSettingsToOUT3Board(expansionCard);
			break;
		case SPI_CARD_1HRT:
			IOBoards_WriteSettingsToHARTBoard(expansionCard);
			break;
		}
	}
	else
		return;
}

void TEST_ExpansionCardERROR(EXPANSION_CARD *expansionCard)
{
	static char counter = 0;
	if (counter < 5)
		counter++;
	else
	{
		expansionCard->CommunicationErrorCounter = 3;
		counter = 0;
	}
}

void IOBoards_ReadFromInputBoards(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	if (SUCCESS == IOBoards_ReadData(ADDRCMD_RD_CALIBR_VAL, 0))
	{
		expansionCard->CommunicationErrorCounter = 0;
		for (int input = 0; input < 6; input++)
		{
			expansionCard->channel[input].status = *(uint8_t *) (SPIRxBuffer + 2 + 5 * input);
			expansionCard->channel[input].value = *(float *) (SPIRxBuffer + 3 + 5 * input);

			if (expansionCard->channel[input].status == ADC_ERROR)
			{
				expansionCard->CommunicationErrorCounter = 3;
				break;
			}
		}
	}
	else
		expansionCard->CommunicationErrorCounter++;

	if (expansionCard->CommunicationErrorCounter >= 3)
	{
		IOBoards_HandleBoardError(expansionCard, boardNumber);

		for (int input = 0; input < 6; input++)
			if (expansionCard->channel[input].status != DISABLED)
				expansionCard->channel[input].status = NO_SETTINGS;
	}
}

static void IOBoards_HandleBoardError1(uint16_t boardNumber)
{
	char SPIMessage[20];
	mini_snprintf(SPIMessage, 20, "SYS:BOARD %c ERROR1", boardNumber + 'A');
	ARCHIVE_SendEvent(SPIMessage);
}

void ResetIN6DCardsValues(void)
{
	for(int i=0; i<NUMBER_OF_CARD_SLOTS;i++)
	{
		if (SPI_CARD_IN6D == ExpansionCards[i].type)
		{
			for (int j=0;j<25;j++)
				ExpansionCards[i].channel[j].diffValue=0;
		}
	}
}

void IOBoards_ReadFromIN6DBoards(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	if (SUCCESS == IOBoards_ReadData(ADDRCMD_RD_CALIBR_VAL, 0))
	{
		expansionCard->CommunicationErrorCounter = 0;
		float tempValue=0;
		for (int input = 0; input < 6; input++)
		{
			expansionCard->channel[input].status = *(uint8_t *) (SPIRxBuffer + 2 + 5 * input);
			tempValue = *(float *) (SPIRxBuffer + 3 + 5 * input);

			if(102==expansionCard->firmwareRevision)
			{
					if(3 == expansionCard->settings.IOcard.channels[input].type)
					{
						if(expansionCard->channel[input].status == SPI_BOARD_OK)
						{
							if (expansionCard->channel[input].value <= tempValue)
								expansionCard->channel[input].diffValue = tempValue - expansionCard->channel[input].value;
							else
								expansionCard->channel[input].diffValue = (20000 - expansionCard->channel[input].value) + tempValue;
							expansionCard->channel[input].value = tempValue;
						}
					}
					else
						expansionCard->channel[input].value = tempValue;
			}
			else
			{
				if(3 == expansionCard->settings.IOcard.channels[input].type)
					expansionCard->channel[input].diffValue = tempValue;
				else
					expansionCard->channel[input].value = tempValue;
			}


			if (expansionCard->channel[input].status == ADC_ERROR)
			{
				expansionCard->CommunicationErrorCounter = 3;
				break;
			}
		}
		memset((uint8_t *) &SPIRxBuffer, 0, 3 + 5 * 6);
	}
	else
		expansionCard->CommunicationErrorCounter++;

	if (expansionCard->CommunicationErrorCounter == 1)
		IOBoards_HandleBoardError1(boardNumber);

	if (expansionCard->CommunicationErrorCounter >= 3)
	{
		IOBoards_HandleBoardError(expansionCard, boardNumber);

		for (int input = 0; input < 6; input++)
			if (expansionCard->channel[input].status != DISABLED)
			{
				expansionCard->channel[input].status = NO_SETTINGS;
				expansionCard->channel[input].diffValue=0;
			}
	}
}

void IOBoards_ReadFrom2RS485Board(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	if (SUCCESS == IOBoards_ReadData(ADDRCMD_RD_VALUES_RS, 0))
	{
		for (int entry = 0; entry < 25; entry++)
		{
			expansionCard->channel[entry].status = *(uint8_t *) (SPIRxBuffer + 2 + 5 * entry);
			expansionCard->channel[entry].value = *(float *) (SPIRxBuffer + 3 + 5 * entry);
		}
		expansionCard->CommunicationErrorCounter = 0;
	}
	else
		expansionCard->CommunicationErrorCounter++;

	if (expansionCard->CommunicationErrorCounter >= 3)
		IOBoards_HandleBoardError(expansionCard, boardNumber);
}

void IOBoards_ReadFromHARTBoard(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	if (SUCCESS == IOBoards_ReadData(ADDRCMD_RD_VALUE_HART, 0))
	{
		for (int entry = 0; entry < 25; entry++)
		{
			expansionCard->channel[entry].status = *(uint8_t *) (SPIRxBuffer + 2 + 5 * entry);
			expansionCard->channel[entry].value = *(float *) (SPIRxBuffer + 3 + 5 * entry);
		}
		expansionCard->CommunicationErrorCounter = 0;
	}
	else
	{
		expansionCard->CommunicationErrorCounter++;
	}

	if (expansionCard->CommunicationErrorCounter >= 3)
		IOBoards_HandleBoardError(expansionCard, boardNumber);
}

void IOBoards_ReadFromTCBoard(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	if (SUCCESS == IOBoards_ReadData(ADDRCMD_RD_VALUE_TC, 0))
	{
		expansionCard->CommunicationErrorCounter = 0;
		for (int input = 0; input < 8; input++)
		{
			expansionCard->channel[input].status = *(uint8_t *) (SPIRxBuffer + 2 + 5 * input);
			expansionCard->channel[input].value = *(float *) (SPIRxBuffer + 3 + 5 * input);

			if (expansionCard->channel[input].status == ADC_ERROR)
			{
				expansionCard->CommunicationErrorCounter = 3;
				break;
			}
		}
	}
	else
		expansionCard->CommunicationErrorCounter++;

	if (expansionCard->CommunicationErrorCounter >= 3)
	{
		IOBoards_HandleBoardError(expansionCard, boardNumber);

		for (int input = 0; input < 6; input++)
			if (expansionCard->channel[input].status != DISABLED)
				expansionCard->channel[input].status = NO_SETTINGS;
	}
}

void IOBoards_WriteToOutputBoards(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	memset((uint8_t *) &SPITxBuffer[2], 0, 6);
	for (uint8_t i = 0; i < 6; i++)
		SPITxBuffer[2 + i] = expansionCard->channel[i].value;

	if (SUCCESS == IOBoards_WriteData(ADDRCMD_WR_STATES, 0))
		expansionCard->CommunicationErrorCounter = 0;
	else
		expansionCard->CommunicationErrorCounter++;

	if (expansionCard->CommunicationErrorCounter >= 3)
		IOBoards_HandleBoardError(expansionCard, boardNumber);
}

void IOBoards_WriteToOUT3Board(EXPANSION_CARD *expansionCard, uint8_t boardNumber)
{
	memset((uint8_t *) &SPITxBuffer[2], 0, 6);
	for (uint8_t i = 0; i < 3; i++)
	{
		SPITxBuffer[2 + 4 * i] = *((uint8_t *) &expansionCard->channel[i].value);
		SPITxBuffer[2 + 4 * i + 1] = *((uint8_t *) &expansionCard->channel[i].value + 1);
		SPITxBuffer[2 + 4 * i + 2] = *((uint8_t *) &expansionCard->channel[i].value + 2);
		SPITxBuffer[2 + 4 * i + 3] = *((uint8_t *) &expansionCard->channel[i].value + 3);
	}

	if (SUCCESS == IOBoards_WriteData(ADDRCMD_WR_VALUES, 0))
		expansionCard->CommunicationErrorCounter = 0;
	else
		expansionCard->CommunicationErrorCounter++;

	if (expansionCard->CommunicationErrorCounter >= 3)
		IOBoards_HandleBoardError(expansionCard, boardNumber);
}

static uint8_t IOBoards_SetNumberOfBytesToSPICommunication(uint8_t functionCode)
{
	for (uint8_t i = 0; i < SIZEOF_SPI_CODES; ++i)
	{
		if (SPIFunctionCodesAndByteCounter[i].functionCode == functionCode)
			return SPIFunctionCodesAndByteCounter[i].byteCounter;
	}
	return 0;
}

static void IOBoards_HandleBoardError(EXPANSION_CARD *expansionCard, uint16_t boardNumber)
{
	char SPIMessage[19];
#ifdef RESET_BOARD
	IOBOARD_PWR_OFF(boardNumber);
#endif
	expansionCard->status = SPI_BOARD_ERROR;
	mini_snprintf(SPIMessage, 19, "SYS:BOARD %c RESET", boardNumber + 'A');
	ARCHIVE_SendEvent(SPIMessage);
	expansionCard->CommunicationErrorCounter = 0;
}

static uint8_t IOBoards_TransmitCommand(uint8_t functionCode, uint8_t subfunctionCode)
{
	uint8_t SPICommanadBuffer[2];
	SPICommanadBuffer[0] = functionCode;
	SPICommanadBuffer[1] = subfunctionCode;

	if (IOBoards_SPI_Transmit(SPICommanadBuffer, CMD_LENGTH) != HAL_OK)
		return ERROR;
	if (ERROR == IOBoards_WaitForSPIStateReady())
		return ERROR;

	return SUCCESS;
}

static uint8_t IOBoards_ReciveResponse(uint8_t functionCode, uint8_t dataLength)
{
	if (IOBoards_SPI_Receive((uint8_t *) &SPIRxBuffer, dataLength) != HAL_OK)
		return ERROR;
	if (ERROR == IOBoards_WaitForSPIStateReady())
		return ERROR;
	if (SPIRxBuffer[0] != functionCode)
		return ERROR;
	return SUCCESS;
}

static uint8_t IOBoards_TransmitData(uint8_t functionCode, uint8_t subfunctionCode, uint8_t dataLength)
{
	SPITxBuffer[0] = functionCode;
	SPITxBuffer[1] = subfunctionCode;

	if (IOBoards_SPI_Transmit((uint8_t *) &SPITxBuffer, dataLength) != HAL_OK)
		return ERROR;
	if (ERROR == IOBoards_WaitForSPIStateReady())
		return ERROR;
	else
		return SUCCESS;
}

static uint8_t IOBoards_ReadData(uint8_t functionCode, uint8_t subfunctionCode)
{
	if (ERROR == IOBoards_TransmitCommand(functionCode, subfunctionCode))
		return ERROR;
	DelayNOP(100000);
	if (ERROR == IOBoards_ReciveResponse(functionCode, IOBoards_SetNumberOfBytesToSPICommunication(functionCode)))
		return ERROR;
	else
		return SUCCESS;
}

static uint8_t IOBoards_WriteData(uint8_t functionCode, uint8_t subfunctionCode)
{
	memset((uint8_t *) &SPIRxBuffer, 0, 3);
	if (ERROR == IOBoards_TransmitCommand(functionCode, subfunctionCode))
		return ERROR;
	DelayNOP(100000);
	if (ERROR == IOBoards_TransmitData(functionCode, subfunctionCode, IOBoards_SetNumberOfBytesToSPICommunication(functionCode)))
		return ERROR;
	DelayNOP(50000);
	if (ERROR == IOBoards_ReciveResponse(functionCode, 3))
		return ERROR;
	else if (SPIRxBuffer[2] == 1)
		return SUCCESS;
	else
		return ERROR;
}

static void DelayNOP(int x)
{
	for (int i = 0; i < x; i++)
		__NOP();
}

uint8_t GetBoardType(uint8_t boardNumber)
{
	return ExpansionCards[boardNumber].type;
}

void GetExpansionBoard(uint8_t boardNumber, EXPANSION_CARD * Board)
{
	*Board = ExpansionCards[boardNumber];
}

static uint8_t IOBoards_WaitForSPIStateReady(void)
{
	uint32_t Timeout = 0;
	HAL_SPI_StateTypeDef SPI5Status = HAL_SPI_STATE_RESET;
	do
	{
		SPI5Status = SPI_GetState();
		if (SPI5Status == HAL_SPI_STATE_ERROR)
			return ERROR;

		else if (SPI5Status == HAL_SPI_STATE_RESET)
			return ERROR;

		if (++Timeout > 100000000)
		{
			SPI_Stop();
			return ERROR;
		}
	} while (SPI5Status != HAL_SPI_STATE_READY);
	return SUCCESS;
}

static void SendNoCardErrorMessage(uint8_t boardNumber)
{
	char SPIMessage[30];
	mini_snprintf(SPIMessage, 30, "SLOT %c \n%s", boardNumber + 'A', GUI_LANG_GetText(127));
	CreateMessage(SPIMessage, GUI_YELLOW, GUI_BLACK);
}

static void SendConfigurationCardErrorMessage(uint8_t boardNumber)
{
	char SPIMessage[30];
	mini_snprintf(SPIMessage, 30, "SLOT %c \n%s", boardNumber + 'A', GUI_LANG_GetText(128));
	CreateMessage(SPIMessage, GUI_YELLOW, GUI_BLACK);
}

static uint8_t IOBoards_WriteCalibrationToBoard(CALIBRATION_StructTypeDef * calibrationData, uint8_t boardNumber, uint8_t InputMode)
{
	uint8_t error = 0;
	uint32_t* pKalibracja = (uint32_t *) calibrationData;
	uint32_t* pSPI5Buffer = (uint32_t *) &SPITxBuffer[2];

	for (uint8_t ikal = 0; ikal < 14; ikal++)
	{
		*pSPI5Buffer = *pKalibracja;
		pSPI5Buffer++;
		pKalibracja++;
	}

	IOBoards_ChipSelect(boardNumber);
	error = IOBoards_WriteCalibrationData(InputMode);
	IOBoards_DisableChipSelect();
	return error;
}

static uint8_t IOBoards_WriteCalibrationData(uint8_t InputMode)
{
	if (IOBoards_TransmitCommand(ADDRCMD_WR_CAL_POINTS, InputMode) == ERROR)
		return ERROR;
	DelayNOP(100000);

	if (ERROR == IOBoards_TransmitData(ADDRCMD_WR_CAL_POINTS, InputMode, IOBoards_SetNumberOfBytesToSPICommunication(
	ADDRCMD_WR_CAL_POINTS)))
		return ERROR;

	HAL_Delay(50);

	if (ERROR == IOBoards_ReciveResponse(ADDRCMD_WR_CAL_POINTS, 3))
		return ERROR;
	else if (SPIRxBuffer[2] == 1)
		return SUCCESS;
	else
		return ERROR;
}

void IOBoards_ReadCalibrationFromBoard(CALIBRATION_StructTypeDef * calibrationData, uint8_t boardNumber, uint8_t InputMode)
{
	IOBoards_ChipSelect(boardNumber);
	IOBoards_ReadData(ADDRCMD_RD_CAL_POINTS, InputMode);
	IOBoards_DisableChipSelect();

	calibrationData->CalYear = *(unsigned char *) (SPIRxBuffer + 4);
	calibrationData->CalMonth = *(unsigned char *) (SPIRxBuffer + 5);
	calibrationData->CalDay = *(unsigned char *) (SPIRxBuffer + 6);
	calibrationData->CalHour = *(unsigned char *) (SPIRxBuffer + 7);
	calibrationData->CalMin = *(unsigned char *) (SPIRxBuffer + 8);
	calibrationData->CalSec = *(unsigned char *) (SPIRxBuffer + 9);

	for (uint8_t nr_kanalu = 0; nr_kanalu < 6; nr_kanalu++)
	{
		calibrationData->Coefficient[nr_kanalu].A = *(float *) (SPIRxBuffer + 10 + 8 * nr_kanalu);
		calibrationData->Coefficient[nr_kanalu].B = *(float *) (SPIRxBuffer + 10 + 8 * nr_kanalu + 4);
	}
}

void HART_GetLongAddr(uint8_t boardNumber, uint8_t shortAddr, uint8_t *bufferAddrRev)
{
	uint8_t state;

	IOBoards_ChipSelect(boardNumber);
	state = IOBoards_ReadData(ADDRCMD_RD_HART_LONG_ADDR, shortAddr);
	IOBoards_DisableChipSelect();

	if (SUCCESS == state)
	{
		*(bufferAddrRev + 0) = SPIRxBuffer[5];
		*(bufferAddrRev + 1) = SPIRxBuffer[6];
		*(bufferAddrRev + 2) = SPIRxBuffer[7];
		*(bufferAddrRev + 3) = SPIRxBuffer[8];
		*(bufferAddrRev + 4) = SPIRxBuffer[9];
		*(bufferAddrRev + 5) = SPIRxBuffer[3];
	}
}

void HART_ServiceReadValue(char *CalValue, uint8_t boardNumber)
{
	uint8_t state;

	IOBoards_ChipSelect(boardNumber);
	state = IOBoards_ReadData(ADDRCMD_RD_HART_FRAME, 0);
	IOBoards_DisableChipSelect();

	if (SUCCESS == state)
	{
		for (int i = 0; i < 240; ++i)
			CalValue[i] = SPIRxBuffer[2 + i];
	}
}

void HART_SendCmdResistorOnOff(uint8_t boardNumber, uint8_t resistorOnOff)
{
	IOBoards_ChipSelect(boardNumber);
	SPITxBuffer[2] = resistorOnOff;
	IOBoards_WriteData(ADDRCMD_WR_HART_CMD_RESISTOR, 0);
	IOBoards_DisableChipSelect();
}

void HART_ChangeAddress(uint8_t boardNumber, uint64_t addr, uint8_t newAddr)
{
	IOBoards_ChipSelect(boardNumber);
	SPITxBuffer[2] = addr >> 32;
	SPITxBuffer[3] = addr >> 24;
	SPITxBuffer[4] = addr >> 16;
	SPITxBuffer[5] = addr >> 8;
	SPITxBuffer[6] = addr >> 0;
	SPITxBuffer[7] = newAddr;
	IOBoards_WriteData(ADDRCMD_WR_HART_CMD_CHANGE_ADDR, 0);
	IOBoards_DisableChipSelect();
}

void HART_SendRequestReadID(uint8_t boardNumber, uint8_t shortAddr)
{
	IOBoards_ChipSelect(boardNumber);
	SPITxBuffer[2] = shortAddr;
	IOBoards_WriteData(ADDRCMD_WR_CMD_ID, 0);
	IOBoards_DisableChipSelect();
}

void Calibration_Write(CALIBRATION_StructTypeDef * calibrationData, uint8_t BoardNumber, uint8_t InputMode)
{
	RTC_TimeTypeDef CalibrationTime;
	RTC_DateTypeDef CalibrationDate;

	RTC_GetTimeAndDate(&CalibrationTime, &CalibrationDate);

	localCalibration.CalDay = CalibrationDate.Date;
	localCalibration.CalMonth = CalibrationDate.Month;
	localCalibration.CalYear = CalibrationDate.Year;
	localCalibration.CalHour = CalibrationTime.Hours;
	localCalibration.CalMin = CalibrationTime.Minutes;
	localCalibration.CalSec = CalibrationTime.Seconds;

	for (uint8_t nr_kanalu = 0; nr_kanalu < 6; nr_kanalu++)
	{
		localCalibration.Coefficient[nr_kanalu].A = calibrationData->Coefficient[nr_kanalu].A;
		localCalibration.Coefficient[nr_kanalu].B = calibrationData->Coefficient[nr_kanalu].B;
	}
	IOBoards_WriteCalibrationToBoard(&localCalibration, BoardNumber, InputMode);
}

static float IOBoards_ReadValue(uint8_t functionCode, uint8_t IONumber, uint8_t boardNumber)
{
	IOBoards_ChipSelect(boardNumber);
	IOBoards_ReadData(functionCode, 0);
	IOBoards_DisableChipSelect();
	float value = *(float *) (SPIRxBuffer + 3 + 5 * IONumber);
	return value;
}

void Calibration_ReadValues(float *CalValue, float *RawValue, uint8_t IONumber, uint8_t boardNumber)
{
	*CalValue = IOBoards_ReadValue(ADDRCMD_RD_CALIBR_VAL, IONumber, boardNumber);
	*RawValue = IOBoards_ReadValue(ADDRCMD_RD_RAW_VAL, IONumber, boardNumber);
}

static void IOBoards_WriteInitToHartBoard(const EXPANSION_CARD * expansionCard)
{
	int sizeFrame;
	sizeFrame = 45;

	if (expansionCard->status == SPI_BOARD_OK)
	{
		memset((uint8_t *) &SPITxBuffer[2], 0, CMD_LENGTH + sizeFrame);
		for (uint8_t i = 0; i < sizeFrame; i++)
			SPITxBuffer[2 + i] = 0;
		IOBoards_WriteData(ADDRCMD_WR_VALUES_HART, 0);
	}
}

void HART_Init2SendCommandGetLongAddr(uint8_t boardNumber, uint8_t InputsMode)
{
	EXPANSION_CARD CalibarationSettingForCard = ExpansionCards[boardNumber];

	IOBoards_ChipSelect(boardNumber);
	IOBoards_WriteInitToHartBoard(&CalibarationSettingForCard);
	IOBoards_DisableChipSelect();

}

void SetBoardToCalibartionMode(uint8_t boardNumber, uint8_t InputsMode)
{
	EXPANSION_CARD CalibarationSettingForCard = ExpansionCards[boardNumber];

	CalibarationSettingForCard.settings.IOcard.channels[0].type = InputsMode;
	CalibarationSettingForCard.settings.IOcard.channels[1].type = InputsMode;
	CalibarationSettingForCard.settings.IOcard.channels[2].type = InputsMode;

	if (CalibarationSettingForCard.type == SPI_CARD_IN3 || CalibarationSettingForCard.type == SPI_CARD_IN3RTD)
	{
		CalibarationSettingForCard.settings.IOcard.channels[3].type = 0;
		CalibarationSettingForCard.settings.IOcard.channels[4].type = 0;
		CalibarationSettingForCard.settings.IOcard.channels[5].type = 0;
	}
	else
	{
		CalibarationSettingForCard.settings.IOcard.channels[3].type = InputsMode;
		CalibarationSettingForCard.settings.IOcard.channels[4].type = InputsMode;
		CalibarationSettingForCard.settings.IOcard.channels[5].type = InputsMode;
	}

	IOBoards_ChipSelect(boardNumber);
	IOBoards_WriteSettingsToInputBoard(&CalibarationSettingForCard);
	IOBoards_DisableChipSelect();
}

uint8_t GetActiveCalibrationPointsMode(uint8_t boardNumber, uint8_t inputsMode)
{
	if (GetBoardType(boardNumber) == SPI_CARD_IN3)
	{
		switch (inputsMode)
		{
		case 3:
		case 4:
			return 1;
			break;
		case 5:
			return 2;
		case 6:
		case 7:
		case 8:
			return 3;
		default:
			return 0;
		}
	}
	else
		return 0;
}

void IOBoards_ChipSelect(uint8_t boardNumber)
{
	IOBoards_DisableChipSelect();
	DelayNOP(5000);
	IOBoards_SetChipSelect(boardNumber);
	DelayNOP(5000);
	IOBoards_EnableChipSelect();
	DelayNOP(100000);
}

void IOBoards_Initalize(void)
{
	MX_SPI5_Init();
	HAL_GPIO_WritePin(DEMUX_E_GPIO_Port, DEMUX_E_Pin, GPIO_PIN_SET);
}

void IOBoards_SetChipSelectDemux(GPIO_PinState demuxA2, GPIO_PinState demuxA1, GPIO_PinState demuxA0)
{
	HAL_GPIO_WritePin(DEMUX_A2_GPIO_Port, DEMUX_A2_Pin, demuxA2);
	HAL_GPIO_WritePin(DEMUX_A1_GPIO_Port, DEMUX_A1_Pin, demuxA1);
	HAL_GPIO_WritePin(DEMUX_A0_GPIO_Port, DEMUX_A0_Pin, demuxA0);
}

void IOBoards_SetChipSelect(uint8_t boardNumber)
{
	switch (boardNumber)
	{
	case BOARD_A:
		IOBoards_SetChipSelectDemux(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET);
		break;
	case BOARD_B:
		IOBoards_SetChipSelectDemux(GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET);
		break;
	case BOARD_C:
		IOBoards_SetChipSelectDemux(GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET);
		break;
	case BOARD_D:
		IOBoards_SetChipSelectDemux(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET);
		break;
	case BOARD_E:
		IOBoards_SetChipSelectDemux(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET);
		break;
	case BOARD_F:
		IOBoards_SetChipSelectDemux(GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET);
		break;
	case BOARD_G:
		IOBoards_SetChipSelectDemux(GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);
		break;
	default:
		IOBoards_SetChipSelectDemux(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET);
		break;
	}
}

void IOBoards_DisableChipSelect(void)
{
	HAL_GPIO_WritePin(DEMUX_E_GPIO_Port, DEMUX_E_Pin, GPIO_PIN_SET);
}

void IOBoards_EnableChipSelect(void)
{
	HAL_GPIO_WritePin(DEMUX_E_GPIO_Port, DEMUX_E_Pin, GPIO_PIN_RESET);
}

void IOBoards_PowerOn(uint8_t boardNumber)
{
	IOBOARD_PWR_ON(boardNumber);
}

void IOBoards_PowerOff(uint8_t boardNumber)
{
	IOBOARD_PWR_OFF(boardNumber);
}

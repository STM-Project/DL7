/*
 * channels_typedef.h
 *
 *  Created on: 18.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_UTILITIES_CHANNELS_TYPEDEF_H_
#define GUI_UTILITIES_CHANNELS_TYPEDEF_H_

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "historicalData.h"

#define NUMBER_OF_CHANNELS 100

#define	FORMULA_USER_SIZE 300 //manipulujac tymi wartosciami nalezy zadbac o odpowiednia wielkosc bufora w parameters.c!
#define FORMULA_RPN_SIZE 450
#define FORMULA_MAX_CHAR 200

#define UNIT_SIZE 31

typedef struct{
	uint8_t type; 	// 0 - Off, 1 - Mesurment, 2 - Computed 3 - ModbusTCP 4 - Demo
	uint8_t board; 	// A-G board number
	uint8_t number;	// 0-9 input number
}CH_SOURCE;

typedef struct {
	int type; //0 ->wyłaczony, 1->Niekasowalny, 2 ->Kasowalny, 3->Dzienny, 4 ->tygodniowy, 5 ->miesięczny
	int ToArchive; //0->brak archiwizacji, 1->archiwizacja
	double value;
	uint8_t period; // ustawiany na podstawie parametrów z listy {1, 60, 3600}
	uint8_t multipler; // ustawiany na podstawie parametrów z listy {0.001, 1, 1000}
	char resolution; //0 ->0., 1->0.0, 2 ->0.00, 3->0.000, 4 ->0.0000
	char unit[UNIT_SIZE];
	uint8_t sendEmail;
}TOTALIZER;

typedef struct {
	uint8_t type; //0 ->wyłaczony, 1->Dolny, 2 ->Górny
	uint8_t mode; //0 -> Alarm, 1->Sterowanie
	uint8_t state; //0 -> wyłączony, 1 -> właczony niepotwierdzony, 2 -> włączony potwierdzony
	uint8_t AckAlarm; //0 -> niepotwierdzoany, 1->potwierdzony (ten stan jest włączny odrazu dla wyjśc w trybie Sterowanie

	float level;
	float hysteresis;
	CH_SOURCE output;
	uint8_t color;			//0->brak zamiany,  1->zielony, 2 ->żółty 3->czerwony
	uint8_t ChangeArchivizationFrequency;
	uint8_t logEvent;		// 0-> alarm 	1-> sterowanie
	uint8_t sendEmail;
}ALARM;

typedef struct {
	char userFormula[FORMULA_USER_SIZE];
	char RPNFormula[FORMULA_RPN_SIZE]; // potrzebuje wiecej, poniewaz tu dochodza jeszcze spacje
}COMPUTE_FORMULA;

typedef struct {
	CH_SOURCE source;
	char description[51];
	float value;
	char resolution; //0 ->0., 1->0.0, 2 ->0.00, 3->0.000, 4 ->0.0000
	char unit[UNIT_SIZE];

	COMPUTE_FORMULA formula;

	TOTALIZER Tot1;
	TOTALIZER Tot2;
	float min;
	float max;
	HIST_DATA *histData;
	float graph_max_value;
	float graph_min_value;
	ALARM alarm[2];

	uint8_t firstIterration;
	int ToArchive;  //0->brak archiwizacji, 1->archiwizacja

	uint8_t CharacteristicType;
	uint8_t UserCharacteristicType;
	float LowSignalValue;
	float LowScaleValue;
	float HighSignalValue;
	float HighScaleValue;

	uint8_t filterType;

	uint8_t failureMode; //0 -> symbol awarii w zależności od typu błedu, 1-> stała wartość pobrana z zmiennej FailureValue
	uint8_t failureState; //aktualny stan błedu 1->brak błędu, inna wartośc zgodnie z ustawieniami.
	float failureValue; //tylko dla trybu 1

	uint32_t Color;

	RTC_TimeTypeDef ResetTime;
	RTC_DateTypeDef ResetDate;
}CHANNEL;

#endif /* GUI_UTILITIES_CHANNELS_TYPEDEF_H_ */

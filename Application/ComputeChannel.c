/*
 * ComputeChannel.c
 *
 *  Created on: 07.08.2017
 *      Author: MagdalenaRyb
 */
//http://www.includehelp.com/c/infix-to-postfix-conversion-using-stack-with-c-program.aspx
#include "ComputeChannel.h"
#include "channels_typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "mini-printf.h"
#include "PopUpMessageDLG.h"

#define TOKEN_NUMBER 14

static void putOnStack(char item);
static char takeFromStack(void);
static void putOnStackNumber(float item);
static float takeFromStackNumber(void);

char stack[FORMULA_RPN_SIZE];
float stackNumber[FORMULA_RPN_SIZE];
int top = -1;
int topNumber = -1;
uint8_t stackUnderflow=0;
uint8_t stackOverflow=0;

typedef enum { false, true } bool;

struct tokenMap {
	char token;
	int index;
	bool firstToken;
};

struct tokenMap map[TOKEN_NUMBER] = {
		{'F', 0, true},
		{'(', 1, true},
		{')', 2, false},
		{'S', 3, true},
		{'Q', 4, false},
		{'T', 5, false},
		{'D', 6, false},
		{'M', 7, false},
		{'-', 8, true},
		{'+', 9, false},
		{'.', 10, false},
		{'#', 11, true},
		{'^', 12,false},
		{'\0', 13, false},
};

bool legalSequences [TOKEN_NUMBER][TOKEN_NUMBER] = {
				/*	digit		(				)				S				Q				T				D				M				-				+				.				#				^			\0  */
/* dig */	{	true, 	true, 	false, 	true, 	false,	false,	true,		true,		true,		true,		true,		true,		true,		false},
/*  (  */	{	false, 	true, 	false, 	true, 	false,	false,	true,		true,		true,		true,		false,	false,	true,		false},
/*  )  */	{	true, 	false, 	true, 	false, 	true,		true,		false,	false,	false,	false,	false,	false,	false,	true},
/*  S  */	{	false, 	true, 	false, 	false, 	false,	false,	true,		true,		true,		true,		false,	false,	false,	false},
/*  Q  */	{	true, 	false, 	true, 	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	true},
/*  T  */	{	true, 	false, 	true, 	false,	false,	false, 	false,	false,	false,	false,	false,	false,	false,	true},
/*  D  */ {	true, 	false, 	true, 	false,	true,		true,		false,	false,	false,	false,	false,	false,	false,	false},
/*  M  */ {	true, 	false, 	true, 	false,	true,		true,		false,	false,	false,	false,	false,	false,	false,	false},
/*  -  */ {	true, 	true, 	true, 	false,	true,		true,		true,		true,		true,		true,		false,	false,	false,	false},
/*  +  */ {	true, 	false, 	true, 	false,	true,		true,		false,	false, 	false,	false,	false,	false,	false,	false},
/*  .  */	{	true, 	false, 	false,	false,	false,	false,	false,	false, 	false,	false,	false,	false,	false,	false},
/*  #  */	{	false,	true, 	false,	true,		false,	false,	true,		true,		true,		true,		false,	false,	true,		false},
/*	^  */ {	true, 	false, 	true, 	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false},
/* \0  */ {	true, 	false, 	true,		false,	true,		true,		false,	false,	false,	false,	false,	false,	false,	false},
};


static int isOperator(char *symbol)
{
	if(*symbol == '+' || *symbol =='-' || *symbol == 'M' || *symbol == 'D' || *symbol == 'Q' || *symbol == 'T' || *symbol == 'S' || *symbol == '^' || *symbol == 'N')
		return 1;
	else
		return 0;
}

static int isSpecialCase(char symbol)
{
	if(symbol == '#' || symbol == '.' || symbol == '(' || symbol == ')')
		return 1;
	else
		return 0;
}

static uint8_t getIndex(char token) {
	if(isdigit(token))
		token = 'F';
	for (int i=0; i<TOKEN_NUMBER; i++){
		if(map[i].token == token)
			return map[i].index;
	}
	return TOKEN_NUMBER;
}

uint8_t CheckFormulaCorrectness(char * formula) {
	/* WAŻNE! Formula podana jako argument funkcji musi byc po konwersji funkcja convertSpecialCase */
	uint8_t i = 0;
	uint8_t index0, index1;
	int bracket = 0;
	char miniStack[20];
	if (!(map[getIndex(*formula)].firstToken))
		return 1;
	do {
		/* sprawdzenie ilosci nawiasow i odpowiedniego domkniecia */
		if (*(formula+i) == '(')
			bracket++;
		else if (*(formula+i) == ')') {
			bracket--;
			if (bracket<0)
				return i;
		}
		/* sprawdzenie odpowiedniej sekwencji znakow wyrazenia infix */
		index0 = getIndex(*(formula+i));
		index1 = getIndex(*(formula+i+1));
		if (index0 >= TOKEN_NUMBER || index1>= TOKEN_NUMBER)
			return 1;
		if(!legalSequences[index1][index0])
			return 1;
		/* sprawdzenie, warunek maksymalnej ilosc minusow (2) jest spelniony oraz czy dwa minusy nie wystepuja przed pierwiastkowaniem albo nawiasem */
		if(*(formula+i)=='-' && *(formula+i+1)=='-' && (*(formula+i+2)=='-' || *(formula+i+2)=='(' || *(formula+i+2)=='S'))
			return 1;
		/* sprawdzenie, czy nie zostal wprowadzony kanal z poza zakresu i czy jest l. calkowita*/
		else if (*(formula+i) == '#') {
			if(isdigit(*(formula+i+1))) {
				for (int j=0; j<20 && (isdigit(*(formula+i+j+1)) || *(formula+i+j+1)=='.'); j++) {
					miniStack[j] = *(formula+i+j+1);
					miniStack[j+1] = '\0';
				}
				if(strchr(miniStack, '.') || atoi(miniStack)<1 || atoi(miniStack)>NUMBER_OF_CHANNELS)
					return 1;
			}
		}
		i++;
	} while (*(formula+i) != '\0');
	if (bracket!=0)
		return 1;
	return 0;
}

void convertSpecialCase(char* Dst, char* Src) {
	int i=0;
	int j=0;
	while(*(Src+i) != '\0') {
		if(isdigit(*(Src+i)) || isOperator((Src+i)) || isSpecialCase(*(Src+i)))
			*(Dst+j++) = *(Src+i++);
		else if(*(Src+i) == 195) {
			if(*(Src+i+1) == 151)
				*(Dst+j++) = 'M';
			else if(*(Src+i+1) == 183)
				*(Dst+j++) = 'D';
			i += 2;
		}
		else if(*(Src+i) == 194) {
			if(*(Src+i+1) == 178)
				*(Dst+j++) = 'Q';
			else if(*(Src+i+1) == 179)
				*(Dst+j++) = 'T';
			i += 2;
		}
		else if(*(Src+i) == 226 && *(Src+i+1)==136 && *(Src+i+2)==154) {
			*(Dst+j++) = 'S';
			i += 3;
		}
		else
			i++;
	}
	*(Dst+j) = '\0';
}

static void putOnStack(char item)
{
	if(top >= FORMULA_RPN_SIZE-1)
		stackOverflow = 1;
	else
		stack[++top] = item;
}

static char takeFromStack(void)
{
	char item ;
	if(top <0)
	{
		stackUnderflow = 1;
		return '\0';
	}
	else
	{
		item = stack[top--];
		return(item);
	}
}

static void putOnStackNumber(float item)
{
	if(topNumber >= FORMULA_RPN_SIZE-1)
		stackOverflow = 1;
	else {
		topNumber += 1;
		stackNumber[topNumber] = item;
	}
}

static float takeFromStackNumber(void)
{
	float item ;

	if(topNumber < 0)
	{
		stackUnderflow = 1;
		return 0;
	}
	else
	{
		item = stackNumber[topNumber--];
		return(item);
	}
}

static int precedence(char symbol)
{
	if(symbol == 'Q' || symbol == 'T' || symbol == 'S'  || symbol == '^') /* second power, third power, square root, power*/
		return 4;
	else if(symbol == 'N') /* negaytive */
		return 3;
	else if(symbol == 'M' || symbol == 'D' ) /* multiplication, division */
		return 2;

	else if(symbol == '+' || symbol == '-')
		return 1;

	else
		return 0;
}

static int addSpace(char *expression)
{
	if (*(expression-1) != ' ') {
		*expression = ' ';
		return 1;
	}
	return 0;
}

uint8_t StriToRPN(char *postfix_exp, char *infix_exp)
{
	stackUnderflow=0;
	stackOverflow=0;
	int i, j;
	char item;
	char x;

	putOnStack('(');
	strcat(infix_exp,")");

	i=0;
	j=0;
	item=infix_exp[i];
	if (item == '\0')
		return 0;
	while(item != '\0')
	{
		if(item == '(')
			putOnStack(item);
		else if( isdigit(item) || (isalpha(item) && !isOperator(&item)) || item=='.' || item=='#')
			postfix_exp[j++] = item;
		else if(isOperator(&item))
		{
			j += addSpace(&postfix_exp[j]);
			if (item=='-' && ( i==0 || infix_exp[i-1] == '(' || infix_exp[i-1] == 'M' || infix_exp[i-1] == 'D' || infix_exp[i-1] == '+' || infix_exp[i-1] =='-'))
			{
				item = 'N'; /* badam szczegolny przypadek ujemnej liczby */
			}
			x = takeFromStack();
			while (isOperator(&x) && precedence(x) >= precedence(item))
			{
				postfix_exp[j++] = x;
				j += addSpace(&postfix_exp[j]);
				x = takeFromStack();
			}
			putOnStack(x);
			putOnStack(item);
		}
		else if(item == ')')
		{
			x = takeFromStack();
			while(x != '(')
			{
				j += addSpace(&postfix_exp[j]);
				postfix_exp[j++] = x;
				x = takeFromStack();
			}
		}
		else
			break;
		item = infix_exp[++i];
	}
	postfix_exp[j] = '\0';
	if (stackOverflow || stackUnderflow)
		return 1;
	else
		return 0;
}

float computeRPN(CHANNEL *Channels, int channelNumber)
{
	char *readpointer = NULL;
	char *item = NULL;
	float x=0, y=0;
	uint8_t selection=0;;
	char formulaRPN[FORMULA_RPN_SIZE]={0};

	stackUnderflow = 0;
	stackOverflow = 0;
	top = -1;
	topNumber = -1;

	/* potrzebuje skopiowac argument do zmiennej lokalnej, poniewaz pracujac na argumencie przez komende strtok_r psuje sie formule w strukturze kanalu */
	strncpy(formulaRPN, Channels[channelNumber].formula.RPNFormula,FORMULA_RPN_SIZE);
	item=strtok_r(formulaRPN, " ", &readpointer);
	while(item != '\0')
	{
		if(isOperator(item) == 1)
		{
			x = takeFromStackNumber();
			switch(*item)
			{
			case 'N':
				putOnStackNumber(-x);
				break;
			case 'Q':
				putOnStackNumber(pow(x, 2));
				break;
			case 'T':
				putOnStackNumber(pow(x,3));
				break;
			case 'S':
				putOnStackNumber(sqrt(x));
				break;
			case '^':
				y = takeFromStackNumber();
				putOnStackNumber(pow(y,x));
				break;
			case '+':
				y = takeFromStackNumber();
				putOnStackNumber(x + y);
				break;
			case '-':
				y = takeFromStackNumber();
				putOnStackNumber(y - x);
				break;
			case 'M':
				y = takeFromStackNumber();
				putOnStackNumber(x * y);
				break;
			case 'D':
				y = takeFromStackNumber();
				putOnStackNumber(y / x);
				break;
			}
		}
		else if(*item=='#') {
			selection = atoi((item+1))-1;
			if (Channels[selection].failureState == 1 || Channels[selection].failureMode == 1)
				putOnStackNumber(Channels[selection].value);
			else
			{
				Channels[channelNumber].failureState = Channels[selection].failureState;
				return 0;
			}
		}
		else
			putOnStackNumber(atof(item));

		item= strtok_r(NULL, " ", &readpointer);
	}
	if (stackOverflow || stackUnderflow) {
		Channels[channelNumber].failureState = 4;
		return 0;
	}
	else
		return takeFromStackNumber();
}

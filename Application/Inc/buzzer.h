#ifndef __BUZZER_H
#define __BUZZER_H

#include "gpio.h"

void BUZZER_Init(void);
void BUZZER_Beep(void);
void BUZZER_BeepOffCounter(void);
void TestBUZZER(void);

void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Toggle(void);

#endif /*__BUZZER_H */


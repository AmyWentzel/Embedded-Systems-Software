#include "stm32f303xe.h"
#include "uart.h"

#ifndef LED_H
#define LED_H



void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
//returns 0 if not pressed
//returns 1 if pressed

#endif

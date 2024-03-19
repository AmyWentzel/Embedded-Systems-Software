#include "stm32f303xe.h"

#ifndef PB_H
#define PB_H



void pb_Init(void);

uint8_t pb_PressCheck(void);
//returns 0 if not pressed
//returns 1 if pressed

//uint8_t is an 8-bit unsigned integer (1 byte)


#endif
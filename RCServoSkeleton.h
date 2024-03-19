#include "stm32f303xe.h"
#include "utility.h"
#include "SysClock.h"
#include "uart.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>


#ifndef RCSERVOSKELETON_H
#define RCSERVOSKELETON_H



#define BAUD_RATE 9600
#define MAX_UART_BUFSIZ 120

void RCServo_Init(void);
uint16_t RCServo_SetAngle( int16_t angle );


#endif
#include "SysClock.h"




#ifndef _STM32F303RE_UART_E

#define _STM32F303RE_UART_E
#define BAUD_RATE 9600
#define MAX_UART_BUFSIZ 120


void UART2_Init(void);
void UARTputc( char cx );
void UARTputs( char *str );
char UARTgetc( void );
char UARTgetcNB( void );
void UARTprintf( char *fmt, ...) ;
#endif
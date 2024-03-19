/// header

#ifndef ___UART___
#define ___UART___

void UART2_init(void);

#define BAUD_RATE 9600
#define MAX_BUFSIZ 150

void UARTputc(char thisChar);
void UARTputs(char* thisChar);
char UARTgetc(void);
void UARTprintf(char* message, ...);
char UARTgetcNB(void);
short UARTgetiNB(void);
short UARTgeti(void);

#endif

#include "UART.h"
#include "stm32f303xe.h"
#include "stdio.h"
#include "stdarg.h"

void UART2_init(void){
	
	//Part 1: clock source Config	

	//enable USART2 Clock to enable the subsystem
	
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	 
	// SElect USART2 clock source
	// 00; pclk (peripheral crystal clock
	// 01; SYSTEM CLOCK
	// 10; HST16 clock source
	// 11: LSE Cock source
	
	RCC->CFGR3 &= RCC_CFGR3_USART2SW_1; // reset bit 1a
	RCC->CFGR3 |= RCC_CFGR3_USART2SW_0; // set bit 0
	
	
	//part 2: GPIO congig to enable PA2 and PA3 for UART2 AF
	
	//ENABLE GPIO Port A clock source
	 RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	//Set the MODER for PA2 and PA3 to AF (0b 10)
	// Clear
	GPIOA->MODER &= ~(0x03UL << (2*2));
	GPIOA->MODER &= ~(0x03UL << (2*3));
	
	GPIOA->MODER |= (0x02UL << (2*2));
	GPIOA->MODER |= (0x02UL << (2*3));
	
	// Configure PA.5 output type to push pull
	
	GPIOA->ODR |= 1UL << 5;
	
	// Select USART2 as the alternative function
	
	GPIOA->AFR[0] &= ~(0x0FUL << 2*4);
	GPIOA->AFR[0] &= ~(0x0FUL  << 3*4);
	
	
	GPIOA->AFR[0] |= (0x07UL << 2*4);
	GPIOA->AFR[0] |= (0x07UL << 3*4);
	
	//set the output speed (OSPEED) register to low speed
	//00- low 01- MED, 10- fast, 11- HIGH,
	//OSPEEDR
	
	GPIOA ->OSPEEDR &= ~(0x03UL << (2*2));
	GPIOA ->OSPEEDR &= ~(0x03UL << (3*2)); 	
	
	//Set PUPDR of PA2 and PA3 to No Pull
	
	
	GPIOA -> PUPDR &= ~(0x3UL <<(3*2));
	GPIOA -> PUPDR &= ~(0x3UL <<(2*2));
	
	
	//Set OTYPE to push-pull for PA2 and PA3
	
	GPIOA -> OTYPER &= ~(0x1UL << (1));
	GPIOA -> OTYPER &= ~(0x1UL << (2));
	
	
	
	
	//Part 3: configure the UART Subsystem for 9600 baud comm
	
	// disable UART2 (write - into US of USART2-> CR1)
	
		USART2 -> CR1 &= ~USART_CR1_UE;
	
	
	// set word length (write 0 to M1 and M0 of USART2->CR1
		USART2 -> CR1 &= ~USART_CR1_M;
		
		
	// set oversampling rate to 16x (write 0 into OVER8 of USART2->CR1)
	
	USART2->CR1 &= ~ USART_CR1_OVER8;
		
	// set stop bit config to 1 stop bit (write 00 into USART2->CR2)
	USART2 ->CR2 &= ~USART_CR2_STOP;
	
	// set the Baud rate register
	//BRR_Value = SystemClock / BaudRate
	// SystemCoreClock (72MHz) defined in SysClock.h
	// BaudRate already defined in UART.h
	// therefore
	// Write SystemCoreClock /BAUD_RATE into USART2->BRR
	SystemCoreClockUpdate();
	USART2->BRR = SystemCoreClock/ BAUD_RATE;
	
	// turn on transmitter and reveiver blocks by writing 1 into RE and TE of USART2->CR1
	USART2 -> CR1 &= ~USART_CR1_TE;
	USART2 -> CR1 &= ~USART_CR1_RE;
	
	
	USART2 -> CR1 |= USART_CR1_TE;
	USART2 -> CR1 |= USART_CR1_RE;
	
	// RE-Enable USART2 by writing 1 into UE or USART->CR1
	USART2 -> CR1 |= USART_CR1_UE;
	
	
	//Wait for REACK and TEACK to turn 1 on USART->ISR
	while((USART2->ISR & USART_ISR_TXE) == 0);
	while((USART2->ISR & USART_ISR_TXE) == 0); 
}

//sends a character over the Rx channel
void UARTputc(char thisChar){
	while((USART2->ISR &USART_ISR_TXE) == 0);
	USART2 ->TDR = (uint8_t) thisChar;
}

//Sends a string over the Rx channel
void UARTputs(char* thisChar){
	while(*thisChar){
		UARTputc(*thisChar++);
		
	}	
}


// This is a character input
char UARTgetc(void)
{
	while((USART2->ISR & USART_ISR_RXNE) == 0);
	return(char)(USART2->RDR);
}


// This is a character input that doesn't stop if there is no input ready
char UARTgetcNB(void)
{
	if(!((USART2->ISR & USART_ISR_RXNE) == 0)){
	return(char)(USART2->RDR);
	}
	return '\0';
}


//This works as a printf but will transimit from the board to tera term
void UARTprintf(char* message, ...)
{
	va_list args;
	char buf[MAX_BUFSIZ];
	va_start (args, message);
	vsnprintf( buf, MAX_BUFSIZ, message, args);
	va_end(args);
	UARTputs(buf);
}


short UARTgeti(void){
	short inputI = UARTgetc()-48;
	if(inputI<0 || inputI >=10){
		UARTprintf("Error, input not a digit");
		return(-1000);
	}
	else{
		return inputI;
	}
}
 

short UARTgetiNB(void){
	short inputI = UARTgetcNB()-48;
	if(inputI<0 || inputI >=10){
		return(-1);
	}
	else{return inputI;}
}

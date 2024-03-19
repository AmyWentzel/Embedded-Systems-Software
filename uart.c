/*************************************************************************
FILE NAME: uart.c
DESCRIPTION: Initialize, GetChar(), PutCHar(), PutString(), and printf() 
		functions for UART compatibility on Nucleo STM32F303RE board.
AUTHOR: Scott Chen, Amy Wentzell
DATE: January 22, 2024

*************************************************************************/





#include "stm32f303xe.h"
#include "uart.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>




void UART2_Init(void)
{
	
	///// 
	/// INIT PART I: Clock Source Configuration
	/////
	
	//Enable USART2 clock to enable it
	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	
	//Select USART2 clock source
	//  00: PCLK (peripheral crystal clock, not available)
	//  01: System block (SYSCLK)
	//  10: HSI16 clock source (16 MHz high-speed internal RC oscillator, faster startup but less accurate)
	//  11: LSE clock source (32.7 kHz clock, low-speed)
	
	//  We want SYSCLK
	RCC -> CFGR3 &= ~RCC_CFGR3_USART2SW_1;  // clear the MSB
	RCC -> CFGR3 |= RCC_CFGR3_USART2SW_0;   // set the LSB
	// There are better ways to set this.  We will talk about it when we have time.
	
	
	///// 
	/// INIT PART II: Enable GPIO
	/////
	
	// Enable the GPIO clock to enable it. (Where is USART2 located?)
	// We will use Port A (most commonly used)
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;  //same line as that in LED
	
	
	// Then, Configure the desired pins - we will use PA2 and PA3 (common choice)
	//    PA.2 => USART2 TX (on AF7)
	//    PA.3 => USART2 RX (on AF7)
	// Thus, we need to set up 1) Alternative Function, 2) Speed (Low Speed), 3) Push-Pull Mode, and 4) Pull-Up
	
	// Alternative Function
	GPIOA -> MODER &= ~( 0x03UL << (2*2) ); // Clear out GPIOA Pin 2 Mode bits
	GPIOA -> MODER &= ~( 0x03UL << (2*3) ); // Clear out GPIOA Pin 3 Mode bits
	
	GPIOA -> MODER |= ( 0x02UL << (2*2) ); // Set GPIOA Pin 2 Mode bits to '10' for AF
	GPIOA -> MODER |= ( 0x02UL << (2*3) ); // Set GPIOA Pin 3 Mode bits to '10' for AF
	// Recall: 00 - input, 01 - output, 10 - AF, 11 - analog
	
	GPIOA -> AFR[0] |= ( 0x07UL << (4*2) ); // Set PA2 to AF7 (each AFR is 4 bit, AFR[0] is AFRL)
	
	//GPIOA -> AFR[0] = (0x7UL &= ~GPIO_AFRL_AFRL7_Msk); 
	//Attempt at using stm32f303xe.h implementation (unsuccessful)
	
	GPIOA -> AFR[0] |= ( 0x07UL << (4*3) ); // Set PA3 to AF7 (each AFR is 4 bit, AFR[0] is AFRL)
	//  There's a better way to do it.  Read into stm32f303xe.h!!
	
	// Speed Configuration : Low Speed
	// 00 - Low, 01 - Med, 10 - Fast, 11 - High
	GPIOA -> OSPEEDR &= ~( 0x3UL << (2*2) );  //Low speed for PA2, why?
	GPIOA -> OSPEEDR &= ~( 0x3UL << (2*3) );  //Low speed for PA3, why?
	
	// Pull-Up Connfiguration : No Pull
	// 00 - No Pull, 01 - Pull-Up, 10 - Pull-Down, 11 - Reserved (not used for now)	
	GPIOA -> PUPDR &= ~ ( 0x3UL << (2*2)); // No pull for PA2
	GPIOA -> PUPDR &= ~ ( 0x3UL << (2*3)); // No pull for PA3
	
	// Output Type Configuration : Push-Pull
	// 0 - Push-pull, 1 - Open Drain
	GPIOA -> OTYPER &= ~ ( 0x1UL << 2 ); // Push-pull for PA2
	GPIOA -> OTYPER &= ~ ( 0x1UL << 3 ); // Push-pull for PA3
	
	// The above configuration settings can be greatly simplified - how?
	
	
	
	///// 
	/// INIT PART III: Configure the USART Comm Mode
	/////
	
	//Default Setting (we will use this)
	// No hardware flow control, 8 bit data bits, no parity, 1 start bit, 1 stop bit
	
	// III.1 Disable USART
	USART2 -> CR1 &= ~USART_CR1_UE; // ref manual + stm32f303xe.h
	
	// III.2 Configure Word Length (M0 and M1 combined)
	//  WATCH OUT!!  M0 and M1 are not located adjacent to each other.  M0 is on bit 12, and M1 is on bit 28
	//  Good News!   stm32f303xe.h has a mask called "USART_CR1_M" that masks M0 and M1 in one shot
	//               we will use this for our convenience
	//  Mode: 00 - 8-bit length, 01 - 9-bit lenght, 10 - 7 bit
	USART2 -> CR1 &= ~USART_CR1_M; 
	
	// III.3 Sampling Rate (set to 16x, DO NOT go with 8x - well, not gonna hurt badly, but less reliable, so why??)
	// Sampling Rate: 0 - oversample by 16x, 1 - oversample by 8x (just don't, because largely pointless)
	USART2 -> CR1 &= ~USART_CR1_OVER8;
	
	// III.4 Stop Bit Configuration (we will use 1 stop bit)
	// This setting is on CR2. 
	//  00 - 1 bit, 01 - 0.5 bit (what?), 10 - 2 bit, 11 - 1.5 bit (WHAT???)
	USART2 -> CR2 &= ~USART_CR2_STOP;
	
	// III.5 Configure BAUD Rate (BAUD_RATE = 9600, #define at the beginning)
	//   APB System Clock Rate is 72 MHz 
	//     (system_stm32f3xx.c has some definitions, but confusing...Set to 80MHz first, but changed to 72 MHz by HSI later)
	// To hit 9600 BAUD, we need to set the BAUD clock divider by:
	//   72 MHz / BAUD_RATE  		(for 16x oversampling)
	//   72 MHz / 2 * BAUD_RATE	(for 8x oversapling) 
	// Thus, with our settings, we should set USARTDIV to:
	//   USARTDIV = 72,000,000 / 9600 = 7500
	// Note: BBR is 16-bit long, so consider the design constraints!
	USART2 -> BRR = SystemCoreClock / BAUD_RATE;
	
	// III.6 Enable Transmitter and Receiver
	USART2 -> CR1 |= USART_CR1_RE; //Enable Receiver
	USART2 -> CR1 |= USART_CR1_TE; //Enable Transmitter
	
	/// III.7 Re-enable USART
	USART2 -> CR1 |= USART_CR1_UE; // ref manual + stm32f303xe.h
	
	// III.8 Make sure USART is ready to respond
	// Zhu: After re-enabling USART with new settings: 
	//    REACK will take some time to be automatically set to zero when receiver is ready to receive
	//    TEACK should also be set to zero automatically after reset, so to get ready to transmit.  
	// Both flags are available in USART interrupt status register (ISR)
	while ( (USART2 -> ISR & USART_ISR_TEACK) == 0 );  // Wait till Transmitter is ready to go
	while ( (USART2 -> ISR & USART_ISR_REACK) == 0 );  // Wait till Receiver is ready to go
	
	// Finally, we've got USART2 ready for use!
}



void UARTputc( char cx )
{
	
	while (!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = (uint8_t)cx;
}

void UARTputs( char *str )
{
	while( *str )
	UARTputc( *str++ );
}


char UARTgetcNB( void ) // Non-blocking
{
	if (!((USART2->ISR & USART_ISR_RXNE) == 0))
		return(char)(USART2->RDR);
	else
		return ('\0');
}



void UARTprintf( char *fmt, ...) 
{
		va_list args;
		// need to guarantee that buffer big enough for any string
		char buffer[MAX_UART_BUFSIZ];
		va_start( args, fmt );
		vsnprintf( buffer, MAX_UART_BUFSIZ, fmt, args );
		va_end( args );
		UARTputs( buffer );
}


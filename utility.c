/*************************************************************************
FILE NAME: main.c
DESCRIPTION: Use UART compatible functions and stepper functions
		to mimic the control of the stepper driver.
AUTHOR: Scott Chen, Amy Wentzell
DATE: January 22, 2024

*************************************************************************/

#include "utility.h"



void Delay_ms(uint32_t msec) {
	SysTick->CTRL = 0;
	// Disable Timer + SysTick interrupt + default AHB/8 clock + clear CountFlag (how?)
	SysTick->LOAD = ((SystemCoreClock /8) / 1000UL) * msec; // Set reload register (like BRR)
	SysTick->VAL = 0; // Reset the SysTick counter value
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk; // Start SysTick. Use AHBclock/8
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // Wait for timeout
	SysTick->CTRL = 0; // Disable SysTick until next time
}

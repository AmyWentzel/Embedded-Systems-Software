/*************************************************************************
FILE NAME: pb.c
DESCRIPTION: Push Button Init and PressCheck Functions
AUTHOR: Scott Chen, Amy Wentzell
DATE: February 13, 2024

*************************************************************************/


#include "pb.h"

void pb_Init(void)
{
	//1. Turn on GPIOC
	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	
	//2. Set GPIOC PC13 to input and no pull
	GPIOC -> MODER &= ~(0x3UL << (2*13));
	GPIOC -> PUPDR &= ~(0x1UL << (2*13));
	//replace with macros
}

uint8_t pb_PressCheck(void)
{
	//Check PC13 on IDR...
	// If HIGH, return 0 (off/notpressed)
	if ((GPIOC -> IDR &= GPIO_IDR_13) == 1)
		return(0);
	// Else if LOW, return 1 (on/pressed)
	else
		return(1);
	
}
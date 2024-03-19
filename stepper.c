/*************************************************************************
FILE NAME: stepper.c
DESCRIPTION: Stepper initializing and stepper pattern update.
AUTHOR: Scott Chen, Amy Wentzell
DATE: January 22, 2024

*************************************************************************/

#include "stepper.h"

static unsigned int SEFSpattern[4] = {1, 4, 2, 8};
static unsigned int DEFSpattern[4] = {5, 6, 10, 9};
static unsigned int HSpattern[8] = {1, 5, 4, 6, 2, 10, 8, 9};
extern int step = 0;

void Stepper_Init()
{
	//Enable GPIO Port C Clock
	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	
	//Setup MODER to OUTPUT for PC0-PC3
	//A. CLEAR
	GPIOC -> MODER &= ~( 0x03UL << (2*0) ); // Clear out GPIOC Pin 0 Mode bits
	GPIOC -> MODER &= ~( 0x03UL << (2*1) ); // Clear out GPIOC Pin 1 Mode bits
	GPIOC -> MODER &= ~( 0x03UL << (2*2) ); // Clear out GPIOC Pin 2 Mode bits
	GPIOC -> MODER &= ~( 0x03UL << (2*3) ); // Clear out GPIOC Pin 3 Mode bits
	//B. SET
	GPIOC -> MODER |= ( 0x01UL << (2*0) ); // Set GPIOC Pin 0 Mode bits to '01' for Output
	GPIOC -> MODER |= ( 0x01UL << (2*1) ); // Set GPIOC Pin 1 Mode bits to '01' for Output
	GPIOC -> MODER |= ( 0x01UL << (2*2) ); // Set GPIOC Pin 2 Mode bits to '01' for Output
	GPIOC -> MODER |= ( 0x01UL << (2*3) ); // Set GPIOC Pin 3 Mode bits to '01' for Output
	
	//PUPDR to NOPULL for PC0-PC3
	//A. CLEAR
	GPIOC -> PUPDR &= ~ ( 0x3UL << (2*0)); // No pull for PC0 
	GPIOC -> PUPDR &= ~ ( 0x3UL << (2*1)); // No pull for PC1
	GPIOC -> PUPDR &= ~ ( 0x3UL << (2*2)); // No pull for PC2
	GPIOC -> PUPDR &= ~ ( 0x3UL << (2*3)); // No pull for PC3
	
	//OTYPER to PUSHPULL for PC0-PC3
	GPIO_OTYPER_SET(C, 0 , OTYPER_PP); //Push-pull for PC0
	GPIO_OTYPER_SET(C, 1 , OTYPER_PP); //Push-pull for PC1
	GPIO_OTYPER_SET(C, 2 , OTYPER_PP); //Push-pull for PC2
	GPIO_OTYPER_SET(C, 3 , OTYPER_PP); //Push-pull for PC3
	
}

void Update_Stepper_Pattern(int stepType)
{
	GPIOC -> ODR &= ~(0x0F); // CLEAR the least significant 4 bits
	
	if (stepType == 0)
	{
			GPIOC -> ODR |= SEFSpattern[step]; //Set the bits
			//UARTprintf("%d\n\n", GPIOC -> ODR);
			step++;
			if (step > 3)
				step = 0;
	}
	else if (stepType == 1)
	{
			GPIOC -> ODR |= DEFSpattern[step]; //Set the bits
			//UARTprintf("%d\n\n", GPIOC -> ODR);
			step++;
			if (step > 3)
				step = 0;
	}
	else
	{
			GPIOC -> ODR |= HSpattern[step]; //Set the bits
			//UARTprintf("%d\n\n", GPIOC -> ODR);
			step++;
			if (step > 7)
				step = 0;
	}
	
	
}
/*************************************************************************
FILE NAME: LED.c
DESCRIPTION: LED Init, On, Off, and Toggle functions
AUTHOR: Scott Chen, Amy Wentzell
DATE: February 13, 2024

*************************************************************************/


#include "LED.h"

void LED_Init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~(3UL<<(2*5)); // Clear bits 10 and 11 for Pin
	GPIOA->MODER |= 1UL<<(2*5); 
	GPIOA->OTYPER &= ~(1UL<<5); //Clear Bit 5
	GPIOA->ODR |= 1UL << 5; // Set bit 5
	


}

void LED_On()
{
	GPIOB->ODR |= 1UL << 5; // Output 1 to turn on red LED
}	


void LED_Off()
{
	GPIOB->ODR &= ~(1UL << 5); // Output 0 to turn on red LED
}

void LED_Toggle()
{
	GPIOB->ODR ^= (1UL << 5); // Output 0 if 1, 1 if 0
}
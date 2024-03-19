/*************************************************************************
FILE NAME: RCServoSkeleton.c
DESCRIPTION: Initialize servo, and set the Servo angle position.
AUTHOR: Scott Chen, Amy Wentzell
DATE: February 6, 2024

*************************************************************************/





#include "RCServoSkeleton.h"



void RCServo_Init(void)
{
	// Set up PB15 to Output, AF1, PP, no Pull
	// 1. Enable clock to Port B
		RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;  //enable GPIO Port B
	
	// 2. Set the MODER to AF - PB15
	FORCE_BITS(GPIOx(B)->MODER, 0x03UL << (2*15), (0x02UL << (2*15))); // PB15
	// 3. Set the AFR..([0] or [1]?) to AF1 for PB15
	FORCE_BITS(GPIOx(B) -> AFR[1], 0xFUL << (4*7), 0x01UL << (4*7)); // AFR[1] is PB8 - PB15
	// 4. Set the OTYPER to Push-Pull - PB15
	CLEAR_BITS(GPIOx(B)-> OTYPER, 0x01UL << 15 );//PB15
	// 5. Set the PUPDR to no pull
	CLEAR_BITS(GPIOx(B) -> PUPDR, 0x03UL << (2*15)); //PB15
	// Your Task - Use your MACRO in place of the bit-operation statements from the previous labs.
	
	
	
	// Configure TIM15
	// 1. Turn on TIM15 by feeding in the APB clock.  (RCC_APB2ENR_TIM15EN)
	RCC -> APB2ENR |= RCC_APB2ENR_TIM15EN;
	
	// 2. Program the prescaler (PSC) to ensure TIM15 counts at 1us
	//		Timer Period = (Prescaler - 1) / SystemClockFreq
	//      aka.  Timer Period = System Clock Period * (Prescaler - 1)
	//    Then, what is PSC with Timer Period = 1us? ***73
	TIM15->PSC |= ((SystemCoreClock / 1000000) - 1) ;
	
	// 3. Set TIM15 to Upcounting (no need to do it, because TIM15 only knows upcounting...) ***OPTIONAL***
	// 4. Set the ARR to 20000 us period
	//      Repeating Counter Period = ARR + 1
	//    Then, what is ARR? ***19999us
	TIM15->ARR &= ~(0xFFFFUL);
	TIM15->ARR |= 19999UL;
	
	// 5. Enable TIM15 ARR Preload (ARPE flag on CR1)
	TIM15->CR1 |= (1UL << (1*7));
	// 6. Enable TIM15 main output, so to make it avaiable to the PWM OC (MOE flag on BDTR)
	TIM15->BDTR |= (1UL << (1*15));
	
	
	// Configure CH2 of TIM15 for PWM OC mode
	// 1. Select PWM mode 1 (low-count mode) for TIM15 CH2 (OC2M flags - 2 bits - on CCMR1)
	//    ** PAY VERY CLOSE ATTENTION ON THIS ITEM **
	TIM15->CCMR1 &= ~(0x03UL << (1*12)); //CLEAR
	TIM15->CCMR1 |= 0x06UL << (1*12); //SET
	
	// 2. Enable the Output Compare Preload on CH2 (OC2PE flag on CCMR1)
	TIM15->CCMR1 &= ~(0x01UL << (1*11)); //CLEAR
	TIM15->CCMR1 |= 0x01UL << (1*11); //SET
	
	// 3. Enable output channel 2 (CH2 with no negate) - (CC2E flag on CCER)
	TIM15->CCER &= ~(0x01UL << (1*4)); //CLEAR
	TIM15->CCER |= 0x01UL << (1*4); //SET
	
	// 4. Make CH2 Active High (no negate) - (CC2P flag on CCER)
	TIM15->CCER &= ~(0x01UL << (1*5)); //CLEAR
	
	// 5. Set CH2 PWM initial on-time to 0, so PWM will not output anything before preload is done (writing 0 into CCR2)
	TIM15->CCR2 &= (0UL); // Set all to zero
	
	
	// Set TIM15 off!
	// 1. Force and Update Event to ensure all preload operations are done in sync! (UG flag on EGR)
	TIM15->EGR |= 0x01UL;
	
	
	// 2. Enable TIM15 Counting! (CEN flag on CR1)
	TIM15->CR1 |= 0x01UL;
	
	// If everything done correctly, you should be able to see on PB15 a PWM waveform
	//
	// There are 3 waveforms to capture
	//  1. Neutral Position (Pulse Width = 1500 us)
	//  2. -45 Deg Position (Pulse Width = 1050 us)
	//  3. +45 Deg Position (Pulse Width = 1950 us)
}


uint16_t RCServo_SetAngle( int16_t angle )
{
	int PulseWidth;
	// 1. Convert the target angle to the corresponding target Pulse Width
	//Output = Input x Gain - Offset
	// Gain = (90 - (-90)) / (2400 - 600)us = 180/1800us = 0.1 degree/us or 10 us/degree
	// Offset = -90 or -1500us
	PulseWidth = 10*(angle) + 1500;
	
	// 2. Check whether the PW has exceeded the mechanical (+45 ~ -45 degrees) & motor limit (+/- 90 degrees)
	//    and of course cap the target PW at the limits!
	if (PulseWidth < 1050)
		PulseWidth = 1050;
	else if (PulseWidth > 1950)
		PulseWidth = 1950;
	
	// 3. Write the new target PW into TIM15 CCR2
	TIM15->CCR2 &= ~(0xFFFFUL);
	TIM15->CCR2 |= PulseWidth;
	
	// 4. return the calculated PW for printout in main()
	return(PulseWidth);
}
/*************************************************************************
FILE NAME: USS.c
DESCRIPTION: Initialize TIM16, initialize TIM3, Read US, 
					USSToggleSensorEnable, handle interrupt functions for 
					Ultrasonic Sensor (US.
AUTHOR: Scott Chen, Jude Thibeault, Amy Wentzell
DATE: March 19, 2024

*************************************************************************/

#include "UART.h"
#include "stm32f303xe.h"
#include "stdio.h"
#include "stdarg.h"
#include "Utility.h"
#include "USS.h"

static int lastDistance;

void USS_Timer16_Init(void){
	
	// PART 1 - Configure Timer 16 CH1 as Trigger-Generation Timer
	
	//  I.1 Configure GPIO Pin
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //Enable clock to GPIOA
	
	FORCE_BIT(GPIOA->AFR[1], 0xFUL << 4*4, 1UL << 4*4); // Clear AF12, Set PA12 to AF12 (High)
	
	FORCE_BIT(GPIOA->MODER, 0x3UL << 12*2, MODER_AF << 12*2); //Clear MODER, Set MODER of PA12 to Alternate Function

	FORCE_BIT(GPIOA->PUPDR, 0x3UL << 12*2, PUPDR_OFF << 12*2); //Clear PUPDR, set PUPDR of PA12 to No Pull
	
	FORCE_BIT(GPIOA->OTYPER, 0x1UL << 12, OTYPER_PP << 12); //Clear and set OTYPER to Push-Pull
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //Enable Clock to TIM16
	
	
	
	// I.2 Configure TIM16
	
	TIM16->PSC = (SystemCoreClock/1000000) -1; // Set TIM16 Prescaler
	
	TIM16->ARR = 99999;  // Set TIM16 ARR
	
	SET_BIT(TIM16->CR1, TIM_CR1_ARPE); // Set CR1 to Auto Reload Preload Enable
	
	SET_BIT(TIM16->BDTR, TIM_BDTR_MOE); // Set BDTR to Main Output Enable
	
	SET_BIT(TIM16->CCMR1, TIM_CCMR1_OC1M); // Set CCMR1 to PWM (OC1M)
	
	SET_BIT(TIM16->CCMR1, TIM_CCMR1_OC1PE); // Set CCMR1 to Output Compare Preload on CH1 
	
	SET_BIT(TIM16->CCER, TIM_CCER_CC1E); // Set CCER to Regular Output Channel
	
	SET_BIT(TIM16->CCER, TIM_CCER_CC1P); // Set CCER to Active High
	
	TIM16->CCR1 |= 9; // Repeating Counter Period = ARR + 1, CCR1 = 10 - 1, CCR1 = 9us
	
	
	// I.4 Configure TIM16 CH1 for PWM - **REPEATING MODE**
		// 		a) Force an update event to preload all the registers by setting (UG) in EGR
		//    **b) !! DO NOT !! enable TIM16 here.
		//         ** We will start the timer from the mainline function call!
	SET_BIT(TIM16->EGR, TIM_EGR_UG);
	
	
}	

void USS_Timer3_Init(void)
{
	

	// Part III - Configure Timer 3 CH2 as Edge-Measurement Timer
	
	
	//  III.1 Configure GPIO Pin
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable Clock to GPIOC
	
	FORCE_BIT(GPIOC->AFR[0], 0xFUL << 7*4, 2UL << 7*4); // Clear AF7, Set PC7 to AF7 (Low)
	
	FORCE_BIT(GPIOA->MODER, GPIO_MODER_MODER7, GPIO_MODER_MODER7_1); //Clear MODER, Set MODER of PC7 to Alternate Function

	FORCE_BIT(GPIOA->PUPDR, 0x3UL << 7*2, PUPDR_OFF << 7*2); //Clear PUPDR, set PUPDR of PC7 to No Pull
	
	FORCE_BIT(GPIOA->OTYPER, 0x1UL << 12, OTYPER_PP << 12); //Clear and set OTYPER to Push-Pull
	
	
	
	//  III.2 Configure TIM3 Timer Input Channel 2 (TI2)
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //Enable Clock to TIM3
	
	TIM3->PSC |= (SystemCoreClock/1000000) -1; // Set PSC of TIM3
	
	CLEAR_BIT(TIM3->CR1, TIM_CR1_DIR);	
	
	TIM3->ARR |= 0xFFFF; // Set ARR of TIM3
	
	
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2F); // Clear all bits of IC2F, bits 0 - 3
	
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2PSC); // Clear IC2PSC, set PSC to zero, bits 0 - 1
	
	//  III.3 Configure TIM3 CCR1 for PW measurement on TI2
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_1; // Set CC1S in CCMR1 to selection 2
	
	SET_BIT(TIM3->CCER, TIM_CCER_CC1P); // Set TIM3 CCR1 to capture TI2 falling edge
	CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1NP);


	//  III.4 Configure TIM3 to Slave Reset on TI2 Rising Edge
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_TS); // Set Filtered Input Timer 2 (TI2) to trigger RESET  
	SET_BIT(TIM3->SMCR, TIM_SMCR_TS_1); // Set (TS[3:0]) in SMCR to 6
	SET_BIT(TIM3->SMCR, TIM_SMCR_TS_2);
	
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_0); // Set RESET slave mode on TI2 Rising Edge		//0 LSB
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_1);																							//0
	SET_BIT(TIM3->SMCR, TIM_SMCR_SMS_2); // Write 4 into (SMS[2:0]) in SMCR							//1
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_3);																							//0 MSB
	
	
	//  III.5 Enable Counter Capture
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E); // Enable Counter Capture by setting (CC1E and CC2E) in CCER
	
	// 	III.6 ** ENABLE TIM3 INTERRUPT on TI2 Falling Edge **
	SET_BIT(TIM3->DIER, TIM_DIER_CC1IE); // Enable TIM3 TI2 to generate interrupt request by setting (CC1IE) in DIER
	
	NVIC_SetPriority(TIM3_IRQn, 12); // Set TIM3 IRQ priority to something high under 10, Set to 9

	NVIC_EnableIRQ(TIM3_IRQn); // Enable TIM3 IRQ in NVIC
	
	SET_BIT(TIM3->CR1, TIM_CR1_CEN); // Enable TIM3 main counter by setting (CEN) in CR1
	
}

// Part II  Interface Function - Start Automatic Triggering
void USSToggleSensorEnable(void){
	TIM16->CR1 ^= TIM_CR1_CEN; // Enable the TIM16 by setting CEN in CR1
}

// Part IV - Echo Pulse Width Measurement Interface Functions
		// READ_SENSOR function - Call this from mainline to get distance measurements whenever!
int USSGetDistance(void){
	
	return TIM3->CCR1 /58;
}

// Check whether (CC1IF) in SR is set
void EXIT15_10IRQHandler(void){
	UARTprintf("Here");
	UARTgetc();
	if((TIM3->SR & TIM_SR_CC1IF) !=0){
		lastDistance = TIM3->CCR1;
		TIM3->SR &= ~TIM_SR_CC1IF; // this might be bad and dumb;
	}
	
}

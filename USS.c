#include "UART.h"
#include "stm32f303xe.h"
#include "stdio.h"
#include "stdarg.h"
#include "Utility.h"
#include "USS.h"

static int lastDistance;

void USSInit(void){
	
	//Trigger working in GPIOA pin 12
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->AFR[1] &= ~(0xFUL<< 4*4);
	GPIOA->AFR[1] |= (1UL << 4*4);
	
	GPIOA->MODER &= ~(3UL<< 12*2);
	GPIOA->MODER |= (MODER_AF<< 12*2);
	
	GPIOA->PUPDR &= (3UL << 12*2); 
	GPIOA->PUPDR |= (PUPDR_OFF << 12*2);
	
	GPIOA->OTYPER &= ~(1UL << 12);
	GPIOA->OTYPER |= (OTYPER_PP <<12);
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
	
	TIM16->PSC = (SystemCoreClock/1000000) -1;
	TIM16->ARR = 9999;
	
	SET_BIT(TIM16->CR1, TIM_CR1_ARPE);
	
	SET_BIT(TIM16->BDTR, TIM_BDTR_MOE);
	
	
	SET_BIT(TIM16->CCMR1, TIM_CCMR1_OC1M);
	
	SET_BIT(TIM16->CCMR1, TIM_CCMR1_OC1PE);
	
	SET_BIT(TIM16->CCER, TIM_CCER_CC1E);
	
	CLEAR_BIT(TIM16->CCER, TIM_CCER_CC1P);
	
	TIM16->CCR1 |= 9;
	
	SET_BIT(TIM16->EGR, TIM_EGR_UG);
	
	
	
	
	// TIM 3 is in GPIOC pin7
	
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	
	GPIOC->MODER &= ~(3UL << 7*2);
	GPIOC->MODER |= (MODER_AF << 7*2);
	
	GPIOC->PUPDR &= ~(3UL << 7*2);
	GPIOC->PUPDR |= (PUPDR_OFF<< 7*2);
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	TIM3->PSC |= (SystemCoreClock/1000000) -1;
	TIM3->ARR |= 0xFFFF;
	
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2F_0);
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2F_1);
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2F_2);
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2F_3);
	
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2PSC_0);
	CLEAR_BIT(TIM3->CCMR1,TIM_CCMR1_IC2PSC_1);
	
	
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_1;
	
	SET_BIT(TIM3->CCER, TIM_CCER_CC1P);
	CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1NP);

	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_TS_0);
	SET_BIT(TIM3->SMCR, TIM_SMCR_TS_1);
	SET_BIT(TIM3->SMCR, TIM_SMCR_TS_2);
	
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_0);
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_1);
	SET_BIT(TIM3->SMCR, TIM_SMCR_SMS_2);
	CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_3);
	
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);
	SET_BIT(TIM3->CCER, TIM_CCER_CC2E);
	
	SET_BIT(TIM3->DIER, TIM_DIER_CC1IE);
	NVIC_SetPriority(TIM3_IRQn, 10);
	NVIC_EnableIRQ(TIM3_IRQn);
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
	
}

void USSToggleSensorEnable(void){
	TIM16->CR1 ^= TIM_CR1_CEN;
}

int USSGetDistance(void){
	return lastDistance *170;
}

void EXIT15_10IRQHandler(void){
	if((TIM3->SR & TIM_SR_CC1IF) !=0){
		lastDistance = TIM3->CCR1;
		TIM3->SR &= ~TIM_SR_CC1IF; // this might be bad and dumb;
	}
	
}

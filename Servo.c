#include "Utility.h"
#include "Servo.h"
#include "system_stm32f3xx.h"
#include "SysClock.h"
#include "UART.h"


//required global varables for the stepper
static int targetAngle = 1500;
static int incrementNum = 10;

void servoInit(void){
	//Initilized the control registers 
	RCC->AHBENR = RCC_AHBENR_GPIOBEN;
	
	GPIOB->MODER &= ~(3UL << (2*15));
	GPIOB->MODER |= (2UL << (2*15));
	
	GPIOB->AFR[1] &= ~(15UL << (4*7));
	GPIOB->AFR[1] |= (1UL << (4*7));
	

	GPIOB->OTYPER &= ~(1UL << 15);
	

	GPIOB->PUPDR  &= ~(3UL << (2*15));
	
	
	
	//Enable timer 15
	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
	
	//Set the timer to be in micro seconds then set the reset to be at 20 miliseconds
	TIM15->PSC = (SystemCoreClock/1000000) -1;
	TIM15->ARR = 19999;
	
	
	//Set timer 15 to be a PWM output
	SET_BIT(TIM15->CR1, TIM_CR1_ARPE);
	SET_BIT(TIM15->BDTR, TIM_BDTR_MOE);
	
	CLEAR_BIT(TIM15->CCMR1, TIM_CCMR1_OC2M);
	
	
	TIM15->CCMR1 |= (1UL << 13);
	TIM15->CCMR1 |= (1UL << 14);
	
	SET_BIT(TIM15->CCMR1, TIM_CCMR1_OC2PE);
	
	SET_BIT(TIM15->CCER, TIM_CCER_CC2E);
	
	
	CLEAR_BIT(TIM15->CCER, TIM_CCER_CC2P);
	
	//Set the output to 0 degrees
	TIM15->CCR2 = 1500;
	
	SET_BIT(TIM15->EGR, TIM_EGR_UG);
	SET_BIT(TIM15->CR1, TIM_CR1_CEN); 
}

//called from main and changes the target angle of the PWM
void RCServoSetAngle( int16_t angle){
	if(angle <= 45 && angle >= -45){
		targetAngle = angle *10 + 1500;
	}
}


//incremtents the angle of the servo motor
void incrementAngle(void){
	//calculates the difference between the current angle and target angle
	int diff = targetAngle - TIM15->CCR2;

	//checks that the increment rate isn't 0, if it is 0 then it will just jump to the target angle
	if(incrementNum > 0){
		//checks to make sure that we won't jump over the target angle,
		//then if we will increments to the target angle instead of the increment rate
		if((diff*isNegative(diff)) < incrementNum){
			TIM15->CCR2 += diff;
		}
		else{
		//increments the angle of the PWM towards the target angle
		TIM15->CCR2 += incrementNum *isNegative(diff);
		}
	}
	else{TIM15->CCR2 = targetAngle;}
}	

//called from main to get a read out of the PWM
int ServoUpdateStatus(void){
	UARTprintf("Target angle:\t\t%d\nCurrent angle:\t\t%d\nIncrementing by:\t%d\n\n\n", ((targetAngle-1500)/10), (((int)TIM15->CCR2-1500)/10),   (incrementNum));
	return(targetAngle);
}

//Calleded from name to cange the target output
void updateIncrementNum(int newNum){
	if(newNum >=0){
		incrementNum = newNum;
	}
}


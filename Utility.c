#include "Utility.h"
#include "UART.h"
#include "SysClock.h"
#include "stm32f303xe.h"



//Lags the program for a given amount of time
void delay(uint32_t time) {
	SysTick->CTRL = 0;
	SysTick->LOAD = ((SystemCoreClock /8) / 1000UL) * time;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
		SysTick->CTRL = 0;
}


short isNegative(int testNum){
	if(testNum > 0){
		return(1);
	}
	if(testNum < 0){
		return -1;
	}
	return 0;
} 
	

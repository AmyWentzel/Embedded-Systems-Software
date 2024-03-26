#include "Utility.h"
#include "UART.h"
#include "SysClock.h"
#include "USS.h"
#include "stm32f303xe.h"

int main(void){
	System_Clock_Init();
	SystemCoreClockUpdate();
	UART2_init();
	UARTprintf("Welcome to Lab 5: Ultrasonic Sensor \n\n\n");
	USS_Timer16_Init();
	USS_Timer3_Init();
	USSToggleSensorEnable();
	
	int DistanceReading = 0;
	
	for(;;)
	{
		DistanceReading = USSGetDistance();
		UARTprintf(" Distance = %d.\n\n", DistanceReading);
		delay(100);
	}
}

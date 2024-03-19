// Drive Motor Module Source File

#include "DriveMotor.h"
#include "utility.h"

// Drive Motor Configuration Parameters
// - Motor Speed Control Pins:
//    Left Motor	PC10
//    Right Motor   PC11
// - Motor Direction Control Pins:
//    Left Motor Forward (A)	PC12
//    Left Motor Reverse (B)    PC13
//    Right Motor Forward (A)   PC8
//    Right Motor Reverse (B)   PC9
//
// - Direction Control Truth Table
//     STOP   FWD	RWD   UNDEFINED
// (A)  0      1     0      1     
// (B)  0      0     1      1

void DriveMotor_Init(void)
{
	// Enable GPIO Clock
	
	// Configure Motor Control Pins 
	//  For Direction Control, just ordinary GPIO configurations
	//    MODE = Output
	//    Output Type = Push Pull
	//    Pull-up / Pull-down = No Pull
	//    Initial Output Value should be set to 0 (STOP by default)
	
	//  For Speed Control, connect TIM8 channels to PC10 and PC11 with the following configurations
	//	  MODE = Alternative Function 4
	//    Output Type = Push Pull
	//    Pull-up / Pull-down = No Pull
	
	//  PC10 will be connected to TIM8 channel 1N (complemented output)
	//  PC11 will be connected to TIM8 channel 2N (complemented output)
	
	
	
	// Next, Configure Timer 8 for CH1N and CH2N - Mostly like Servo with 2 exceptions 
	// 1. Turn o Timer 8
	
	// 2. Program Timer 8 prescaler to count in microseconds
		//You need -1 to allow the timer start sequence (mandatory)
		
	// 3. Set TIM8 to upcounting mode (MANDATORY!! because TIM8 default mode can be something else)
		// [DIFFERENT!] This is different from TIM15 in Servo Lab because TIM15 only supports upcounting
	
	// 4. Set the TIM8 reload value to 1000 us period (Remember -1 because it's repeating)
			
	// 5. Enable TIM8 ARR preload
		
	// 6. Set TIM8 main output enabled
	
	
		
	
	// Configure CH1N (Complementary) of TIM8 for Left Wheel PWM output compare mode
	
	// 1. Select PWM mode 1 for TIM8 channel 1N
		// Set OC1M bits to 0110 (i.e. set bit 1 and 2) in CCMR1
		// Read manual again to confirm
	
	// 2. Enable output compare preload on channel 1 (i.e. update counter register only on timer enabled (UE) )
	    // Set OC1PE bit in CCMR1
	
	// 3. Enable the COMPLEMENTARY output channel (CH1N)
		// Set CC1NE bit in CCER
		// [DIFFERENT!] This is defferent because PC10 and PC11 only has AF options of TIM8 CH1N/CH2N
		// Read Manual to confirm
	
	// 4. Make CH1N active HI (standard polarity)
		// Clear CC1NP bit in CCER
		// [DIFFERENT!] Also because we are working with completentary output
	
	// 5. Set the CH1N initial PWM ON-time to 0 by clearing CCR1, so that PWM will not be outputted until the first 1ms elapsed

	
	
	// Configure CH2N (Complementary) of TIM8 for Right Wheel PWM output compare mode
	
	// 1. Select PWM mode 1 for TIM8 channel 2N
		// Set OC2M to 0110 in CCMR1 - same as CH1N configurations
		// Read Manual to confirm
	
	// 2. Enable output compare preload on channel 2 (i.e. update counter register only on timer enabled (UE) )
		// Set OC2PE to 1 in CCMR1 - same as CH1N configurations
	
	// 3. Enable the COMPLEMENTARY output channel (CH2N)
		// Set CC2NE in CCER - same as CH1N
	
	// 4. Make CH2N active HI (standard polarity)
		// Clear CC2NP in CCER - same as CH1N
	
	// 5. Set the CH2N initial PWM ON-time to 0 by clearing CCR1, so that PWM will not be outputted until the first 1ms elapsed

	
	
	// Finally, Start TIM8 CH1N and CH2N Outputs
	
	// 1. Force an update event to preload all the registers
		// Set UG to 1 in TIM8 EGR
	
	// 2. Enable TIM15 to start counting
		// Set CEN in TIM8 CR1
		SET_BITS( TIM8 -> CR1, TIM_CR1_CEN );
	
}


void SetMotorSpeed( uint8_t motor, uint16_t dutyCycle )
{
	// This part is the same as Servo
	//		Write different values into CCR1/CCR2 to control the motor speed using dutyCycle
	//      Make sure you cap the DutyCycle to 100% / 0% so to not cause odd system behaviours
}	




void SetMotorDir( uint8_t motor, uint8_t dir )
{
	// - Direction Control Truth Table
	//     STOP   FWD	RWD   UNDEFINED
	// (A)  0      1     0      1     
	// (B)  0      0     1      1

	// Deploy a switch structure to set the corresponding GPIO pins according to the direction input
}





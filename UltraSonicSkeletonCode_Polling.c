// Ultrasonic Sensor Polling Mode - Bonus Lab

// In this lab, we will design a simple ultrasonic sensor control module that allows you to:
//  1. Call the Ultrasonic Distance Measurement function on demand, and
//  2. Read the Measurement results some time later.
// This is a preparation lab for your full Lab 5 - Ultrasonic Sensor in Interrupt Mode

// As described in class, an Ultrasonic Sensor will require two timers:
//  1. Trigger-Generation Timer (OCM on PWM mode, 10us PW)
//      Recommended Pin - PA.12 TIM 16 - Output CH1
//
//  2. Echo Measurement Timer (ICM on Slave Mode - Rising Edge Reset)
//      Recommended Pin - PC.7 TIM 3 - Input CH2

// For Trigger Generation Timer, we will further set it to ONE-SHOT MODE, so that we can trigger the sensor on demand.
//  i.e. Only trigger the sensor if we want a measurement
// 
// In One-Shot mode, the timer will generate a pulse after enabled, and immediately disable itself automatically after the pulse is generated.





//  Part I - Configure Timer 16 CH1 as Trigger-Generation Timer

//  I.1 Configure GPIO Pin
//		Enable GPIOA, set PA12 to AF - TIM16 CH1, Push-Pull, and no pull-up or pull-down.
	
//  I.2 Configure TIM16
//      a) Turn on clock for TIM16
//      b) Set the PSC so TIM16 counts in us unit
//      c) Set the TIM16 counting direction to upcounting
//      d) Set the ARR to 10us pulse 
//      e) Enable ARR preload (ARPE) in CR1
//      f) Set main output enabled (MOE) in BDTR

//  I.3 Configure TIM16 Channel 1 for OCM with PWM
// 		a) Set TIM16 CH1 to be PWM mode 1 through (OC1M) in CCMR2
//      b) Enable the Output Compare Preload on CH1 (we will update CCR1 only upon UE) through setting (OC1PE) on CCMR1
//      c) Enable the Regular Ouptut Channel for CH1 by setting (CC1E) in CCER
//      d) Make CH1 active HI through CLEARING (CC1P) in CCER
//      e) Set CH1 initial output waveform on-time to 1 in CCR1.
//         - NOTE: this setting is different from periodic PWM in Lab 3.
//                 in one-shot mode, you need to start CCR1 with init value of 1 to make sure the timer works correctly. 
//  			   ( is there a better way to do it? )

//  I.4 Configure TIM16 CH1 for PWM - One Shot
//      a) Set TIM16 CH1 for one-shot mode by setting (OPM) in CR1
// 		b) Force an update event to preload all the registers by setting (UG) in EGR
//      c) !! DO NOT !! enable TIM16 here.
 //        Because we are making an on-demand Trigger, so we only will enable it when the program asks for it



// Part II  Interface Function - ON-DEMAND TRIGGER Function
//   Main program can call this function to generate a single PULSE to trigger the Ultrasonic Sensor
//   a) Enable the TIM16 by setting CEN in CR1



// Part III - Configure Timer 3 CH2 as Edge-Measurement Timer

//  III.1 Configure GPIO Pin
//		Enable GPIOC, set PC7 to AF for TIM3 CH2, no pull-up / pull-down.
//      ** read the ref manual to confirm the AF selection for TIM3 Input Channel 2 (TI2 == TIM3 Input CH2)

//  III.2 Configure TIM3 Timer Input Channel 2 (TI2)
//  	a) Turn on clock for TIM3
//      b) Set the PSC so TIM3 counts in us
//      c) Set TIM3 counting direction to upcounting
//      d) Set ARR to MAXIMUM possible value (i.e. 0xFFFF)
//          so that if we capture a reall large PW measurement, we know it's garbage.
//      e) Set TIM3 TI2 with No Input Filtering to start with, by CLEARING all 4 bits of (IC2F[3:0]) in CCMR1
//			** read the manual!!! (you may want to increase the filtering in the future, and you need to know how to set it)
//      f) Set TI2 Prescaler to ZERO to capture every valid transition event.  This can be done by CLEARING both bits in
//         (IC2PSC[1:0]) in CCMR1
//          ** read the reference manual for more detail
//          Simple Ver.: Prescaler to 1 => only use EVERY OTHER event as valid event


//  III.3 Configure TIM3 CCR1 for PW measurement on TI2
//     	a) Make sure TIM3 TI2 is connected internally to CCR1, so Echo signal falling edge from PC7 can be measured
//         and captured by the timer.
//         This is done by setting (CC1S) in CCMR1 to selection 2
//          ** read the manual to see what is the correct value to write into CC1S
//    	b) Set TIM3 CCR1 to capture TI2 falling edge by setting (CC1P) and clearing (CC1NP) in CCER


//  III.4 Configure TIM3 to Slave Reset on TI2 Rising Edge
//      a) Select Filtered Input Timer 2 (TI2) as the trigger source of the Timer Reset
// 		   Done by setting (TS[3:0]) in SMCR to 6
//          ** read the manual to confirm the configuration value.
//   	b) Select RESET slave mode on TI2 Rising Edge, done by writing 4 into (SMS[2:0]) in SMCR
//  	    ** read the manual to confrim the configuration value.


//  III.5 Enable Counters
// 		a) Enable CCR1 by setting (CC1E) in CCER
//  	b) Enable TIM3 main counter by setting (CEN) in CR1



// Part IV - Echo Pulse Width Measurement Interface Functions
//		a) ECHO_RECEIVED function
//			check whether ECHO has been received - use polling mode
//   		
//			Check whether (CC1IF) in SR is set
//				If yes, ECHO falling edge has been detected, and the counter value is recorded in CCR.
//  			If no, ECHO has not been detected yet.

// 		b) READ_SENSOR function
//  		read the valid CCR1 value from TIM3 to calculate the detected distance.
//				You can return the CCR1 value and let the main program do the calculation, OR
//				You can do the count-to-distance calculation and return the calculated value back to the main program.




//  If you've configured everything above correctly, you should be able to do this in the main control loop:

//  1. Whenever a key is pressed to invoke on distance measurement, fire the ON-DEMAND TRIGGER Function
//  2. Periodically check whether ECHO_RECEIVED has returned true.  If not, do other stuff.
//  3. As soon as ECHO_RECEIVED has returned true, call READ_SENSOR to acquire the measured distance.
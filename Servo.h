#include "system_stm32f3xx.h"
#include "SysClock.h"
#ifndef SERVO_HEADER
#define SERVO_HEADER


void servoInit(void);
void RCServoSetAngle( int16_t angle);
void incrementAngle(void);
int ServoUpdateStatus(void);
void updateIncrementNum(int newNum);



#endif

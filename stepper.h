#include "stm32f303xe.h"
#include "utility.h"
#include "SysClock.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>


#ifndef STEPPER_H

#define STEPPER_H

void Stepper_Init(void);
void Update_Stepper_Pattern(int stepType);



#endif
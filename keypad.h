
#include "uart.h"
#include "pb.h"
#include "utility.h"

#ifndef KEYPAD_H
#define KEYPAD_H



void Keypad_Init(void);

char scanKey(void);

void debounce(void);
//wait the bounce out
//OR count the 1s and 0s
#endif
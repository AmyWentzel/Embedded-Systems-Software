#include "stm32f303xe.h"
#ifndef UTILITY_H
#define UTILITY_H

//We need some macros
#define SET_BITS(port,mask)((port) |= (mask))
// for example... GPIOB -> ODR |= 0x8000;
// is now: SET_BITS(GPIOB->ODR, 0x8000) ***macros are like functions


#define GPIOx(port) GPIO ## port
// GPIOx(B) will be expanded into GPIOB as part of the code


//it would be nice if I could ...
// SET_BITS(B, 0DR, 0x8000) ... this is much better
#define SET_BITS_P(port, reg, mask) ( (GPIOx(port)->(reg)) |= (mask) )
// SET_BITS_P(B, 0DR, 0x8000)
// will be expanded into (GPIOB -> ODR |= 0x8000)


#define CLEAR_BITS(port,mask) ( (port) &= (~(mask)) )

#define FLIP_BITS(port, mask) ( (port) ^= (mask) )

//to avoid writing GPIO ->ODR
#define CLEAR_BITS_P(port, reg, mask) ( (GPIOx(port)->(reg)) &= (~(mask)) )
#define FLIP_BITS_P(port, reg, mask) ( (GPIOx(port)->(reg)) ^= (mask) )


#define FORCE_BITS(port, mask, value) \
( (port) = ( ( (port) & (~(mask)) ) | ( (value) & (mask) ) ) )


#define OTYPER_PP 0UL
#define OTYPER_0D 1UL
// want to do this:
// SET_BITS_P(B, OTYPER, OTYPER_PP << OTYPER_Bit?_Mask)



#define GPIO_OTYPER_SET(port, pin, type) \
FORCE_BITS(GPIOx(port)->OTYPER, (1UL << ( (pin)* 1)), ((type) << ( (pin)* 1)) )
//if this can be built correctly, then we can do this:
// GPIO_OTYPER_SET(B, 2, OTYPER_PP)

// 	MODER, OTYPER, PUPDR, OSPEEDR, ENABLE_CLOCK


void Delay_ms(uint32_t msec);

#endif
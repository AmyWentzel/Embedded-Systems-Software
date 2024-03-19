#include "stm32f303xe.h"
#include "SysClock.h"
#include "stdio.h"
#include "stdarg.h"


#ifndef UTILITYH
#define UTILITYH



#define SET_BIT(port,mask) ((port) |= (mask))


#define GPIOx(port) GPIO ## port
#define SET_BIT_PORT(port, reg, mask) ((GPIOx(port)->(reg)) |= (mask))
#define CLEAR_BIT_PORT(port,reg , mask) ((GPIOx(port))->reg &= (mask))


#define CLEAR_BIT(port, mask) ((port) &= (~(mask)))
#define FLIP_BIT(port,mask)((port) ^= (mask))

#define FORCE_BIT(port,mask, value)  ((port) = (port & (~(mask))) | ((value)&(mask)))

#define OTYPER_PP 0UL
#define OTYPER_OD 1UL

#define GPIO_OTYPE_SET(port,pin,type) (FORCE_BIT(GPIOx(port)->OTYPER, (1UL << ((pin)*1)), ((type) << ((pin)*1)) ))

#define MODER_IN 0UL 
#define MODER_GPO 1UL 
#define MODER_AF 2UL 
#define MODER_AM 3UL 

#define GPIO_MODER_SET(port, pin, type) (FORCE_BIT(GPIOx(port)->MODER, (3UL << ((pin)*2)), ((type) << ((pin)*2))))

#define PUPDR_OFF 0UL
#define PUPDR_PU 1UL
#define PUPDR_PD 2UL

#define GPIO_PUPDR_SET(port, pin, type) (FORCE_BIT(GPIOx(port)->PUPDR, (3UL << ((pin)*2)), ((type) << ((pin)*2))))


void delay(uint32_t time);

short isNegative(int testNum);
#endif

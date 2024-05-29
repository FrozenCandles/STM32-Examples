#ifndef __SYSTICK_DELAY_H__
#define __SYSTICK_DELAY_H__

#include <stm32f10x_conf.h>
#include <core_cm3.h>

#include "main.h"


#define SysTick_Disable() \
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk)
#define SysTick_Enable() \
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk)

#define SysTick_TickInt_Disable() \
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk)
#define SysTick_TickInt_Enable() \
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk)

#define SysTick_CountFlag_Read() \
    READ_BIT(SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk)




void systick_delay_us(unsigned int us);

void systick_delay_ms(unsigned int ms);

void systick_delay_s(unsigned int s);


#endif //__SYSTICK_DELAY_H__

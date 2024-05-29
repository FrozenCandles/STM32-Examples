
#ifndef __MAIN_H__
#define __MAIN_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <iso646.h>

#if !defined(STM32F10X_LD_VL) && \
    !defined(STM32F10X_LD) && \
    !defined(STM32F10X_MD_VL) && \
    !defined(STM32F10X_MD) && \
    !defined(STM32F10X_HD) && \
    !defined(STM32F10X_XL) && \
    !defined(STM32F10X_CL)
#   define STM32F10X_HD
#   define __ARM_CC
#endif

#include <stm32f10x.h>
#include <stm32f10x_conf.h>

#include "sys/chrono.h"


static void SoftDelay(volatile uint32_t Count) {
    while (Count--);  // Do nothing
}

#define DELAY_SHORT  0x166666
#define DELAY_MEDIUM 0x2FFFFF
#define DELAY_LONG   0x5FFFFF

#define nothing {}
#define ever (;;)
#define loop for ever
#define until while
#define Initial {0}

#define USE_PREPROCESSOR
// #define USE_BITFIELD
#define USE_LIB


#endif // __MAIN_H__

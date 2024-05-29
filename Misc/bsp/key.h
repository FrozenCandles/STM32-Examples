#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__




enum KeyState {
    KEY_ON = 1,
    KEY_OFF = !KEY_ON,
} KeyStateType;

#ifdef USE_PREPROCESSOR
#
#   include "preprocessor.h"
#   define KEY1 PA, 0
#   define KEY2 PC, 13
#
#   define Key_Init(Key) GPIO_INIT_E(Key, IPU, 50MHz)
#   define Key_Pressed(Key) (GPIO_READ_E(Key) == KEY_ON)
#   define Key_Released(Key) (GPIO_READ_E(Key) == KEY_OFF)
#
#elif defined(USE_BITFIELD)
#
#   include "sys/bit_fields.h"
#   define KEY1 PAin(0)
#   define KEY2 PCin(13)
#
#   define Key_Pressed(Key) (Key == KEY_ON)
#   define Key_Released(Key) (Key == KEY_OFF)
#
#endif


#endif /* __BSP_KEY_H__ */

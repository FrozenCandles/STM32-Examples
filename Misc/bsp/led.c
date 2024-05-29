
#include "led.h"


#if !defined(USE_PREPROCESSOR) && !defined(USE_BITFIELD)

LED_InstanceType LED_R = GPIO_Pin_5;
LED_InstanceType LED_G = GPIO_Pin_0;
LED_InstanceType LED_B = GPIO_Pin_1;

void LED_Init(LED_InstanceType LED_Instance) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Pin = LED_Instance
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void LED_On(LED_InstanceType LED_Instance) {
    GPIO_ResetBits(GPIOB, LED_Instance);
}

void LED_Off(LED_InstanceType LED_Instance) {
    GPIO_SetBits(GPIOB, LED_Instance);
}

void LED_Toggle(LED_InstanceType LED_Instance) {
    GPIO_WriteBit(GPIOB, LED_Instance,
                  (BitAction) !GPIO_ReadOutputDataBit(GPIOB, LED_Instance));

}

void LED_Status(LED_InstanceType LED_Instance) {
    GPIO_ReadInputDataBit(GPIOB, LED_Instance);

}

void LED_Turn(LED_InstanceType LED_Instance, BitAction NewStatus) {
    GPIO_WriteBit(GPIOB, LED_Instance, NewStatus);
}

#endif

/**
 * @brief ch01.2 使用 GPIO 闪烁 LED 灯
 * 
 * @note
 * 三个 LED ，负极分别接到 PB0 PB1 PB5 ，均为低电平点亮
*/

#include "main.h"


int main(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
            .GPIO_Mode = GPIO_Mode_Out_PP,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5);

    loop {

        GPIO_WriteBit(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5,
                      (BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0));

        SoftDelay(DELAY_MEDIUM);
    }

}

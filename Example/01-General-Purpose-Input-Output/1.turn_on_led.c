/**
 * @brief ch01.1 使用 GPIO 输出点亮 LED 灯
 * 
 * @note
 * LED 负极接到 PB0 ，低电平点亮
*/

#include "main.h"


int main(void) {

    // 1. 打开 GPIO 外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 2. 初始化 GPIO 外设
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Pin = GPIO_Pin_0
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. 调用相关函数设置 GPIO 输出值
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);

    loop {

    }
}

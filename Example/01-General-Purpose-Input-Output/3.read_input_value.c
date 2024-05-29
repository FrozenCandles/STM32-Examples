/**
 * @brief ch01.3 使用 GPIO 读入按键的值，控制 LED 灯亮暗
 * 
 * @note
 * 按键接 PA0 引脚，默认状态为低电平，按下后为高电平，使用并联电容实现硬件滤波，不需软件滤波
*/

#include "main.h"


int main(void) {

    // 1. 打开 GPIO 外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 2. 初始化 GPIO
    GPIO_InitTypeDef GPIO_InitStructure = {
            .GPIO_Mode = GPIO_Mode_Out_PP,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5);

    loop {
        // 3. 调用相关函数
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == SET) {
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == SET);
            GPIO_WriteBit(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5,
                          (BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0));
        }

        SoftDelay(DELAY_SHORT);
    }

}

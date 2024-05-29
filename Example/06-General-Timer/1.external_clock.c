/**
 * @brief ch06.1 通过按键/光敏传感器等作为外部时钟，驱动定时器实现计数功能
 * 
 * @note
 * 每隔一两秒，程序将当前的计数值输送给 PC
 * 
*/

#include "main.h"
#include "bsp/usart.h"


static void TIM_Config(void) {

    // 1. 初始化 ETR 对应的 GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = GPIO_Pin_0,
        .GPIO_Mode = GPIO_Mode_IPD,
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. 配置 TIM2 输入时钟为外部时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_ETRClockMode2Config(TIM2, 
                            TIM_ExtTRGPSC_OFF,
                            TIM_ExtTRGPolarity_Inverted,
                            0x03);

    // 3. 初始化定时器的时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
        .TIM_Prescaler = 1 - 1,  // 对外部计数没必要分频
        .TIM_Period = 1000 - 1,  // 一轮计数的个数。这里不需要进中断，可以设置一个比较大的值
        .TIM_ClockDivision = TIM_CKD_DIV1,
        .TIM_CounterMode = TIM_CounterMode_Up,
    };
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 4. 启用定时器
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}

int main(void) {

    USART1_Config(115200);
    TIM_Config();

    loop {
        uint16_t CurrentCount = TIM_GetCounter(TIM2);
        printf("Current Count: %d", CurrentCount);
        delay(2, s);
    }
}

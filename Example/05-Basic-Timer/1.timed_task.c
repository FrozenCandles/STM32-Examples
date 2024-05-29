/**
 * @brief ch05.1 使用基本定时器执行定时任务
 * 
 * @note
 * 只有大容量的设备才有 TIM6 。中小容量的设备可以改用 TIM3 观察
*/

#include "main.h"
#include "bsp/led.h"


static void TIM_Config(void) {

    // 1. 打开定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    // 2. 初始化定时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
        .TIM_Prescaler = /* SystemCoreClock / 1000000 - 1 */ 72 - 1,
        .TIM_Period = 1000 - 1,
    };
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    // 3. 开中断
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    // 4. 启用定时器
    TIM_Cmd(TIM6, ENABLE);
}

static void InterruptConig(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = TIM6_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&NVIC_InitStructure);
}


uint32_t TimerCount = 0;

void TIM6_IRQHandler(void) {
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        TimerCount++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}


int main(void) {

    LED_Init(LED_R);

    InterruptConig();
    TIM_Config();

    loop {
        if (TimerCount == 1000) {
            TimerCount = 0;
            LED_Toggle(LED_R);
        }
    }

}


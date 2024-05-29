/**
 * @brief ch06.2 将一个定时器的输出事件作为另一个定时器的输入时钟（内部触发），实现定时器的级联
 * 
 * @note
 * 现象为 1 分 20 秒的超长定时任务
*/

#include "main.h"
#include "bsp/led.h"


static void TIM_Config(void) {

    // 1. 打开定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

    TIM_InternalClockConfig(TIM2);  /* 配置定时器时钟为内部时钟 */
    TIM_InternalClockConfig(TIM3);  /* 可省略，默认就为内部时钟 */

    // 2. 初始化定时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
        .TIM_Prescaler = 720 - 1,
        .TIM_Period = 1000,         /* 每 10ms 产生一次更新事件 */
    };
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseStructure.TIM_Period = 100 * 80; /* 每 1min 20s 产生一次更新事件 */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 3. 主从模式配置
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update); // 使能 TIM2 触发输出
    TIM_ITRxExternalClockConfig(TIM3, TIM_TS_ITR1);       // 配置 TIM3 为使用ITR1 作为内部触发的从模式
    /* 这个函数等价于以下两个调用：
     *  TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);
     *  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1);
     */

    // 4. 启用定时器
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

static void InterruptConfig(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = TIM3_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {

        LED_Toggle(LED_G);

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

int main(void) {

    LED_Init(LED_G);
    LED_Off(LED_G);

    TIM_Config();
    InterruptConfig();

    loop;
}

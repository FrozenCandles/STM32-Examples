/**
 * @brief ch02.1 使用外部中断的形式判断按键状态，并控制 LED 灯亮暗
 * 
 * @note
 * 按键接 PA0
*/

#include "main.h"
#include "bsp/led.h"


static void EXTI_Config(void) {
    // 1. 初始化 GPIO ，注意打开 AFIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_IN_FLOATING,
        .GPIO_Pin = GPIO_Pin_0
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. 初始化 EXTI
    EXTI_InitTypeDef EXTI_InitStructure = {
        .EXTI_Line = EXTI_Line0,             // 0 号输入线
        .EXTI_Mode = EXTI_Mode_Interrupt,    // 中断模式
        .EXTI_Trigger = EXTI_Trigger_Rising, // 上升沿触发
        .EXTI_LineCmd = ENABLE               // 启用
    };
    EXTI_Init(&EXTI_InitStructure);

    // 3. 将 EXTI Line0 输入线与 GPIOA Pin0 相关联 
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
}

static void InterruptConfig(void) {
    // 4. 在 NVIC 中安排 EXTI0 中断源
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 优先级分组

    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = EXTI0_IRQn,              // 中断源
        .NVIC_IRQChannelPreemptionPriority = 1,     // 中断源的主优先级
        .NVIC_IRQChannelSubPriority = 0,            // 中断源的子优先级
        .NVIC_IRQChannelCmd = ENABLE                // 开中断
    };
    NVIC_Init(&NVIC_InitStructure);
}

// 5. 编写中断服务函数
void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {  // 判断是否为对应中断

        LED_Toggle(LED_G);

        EXTI_ClearITPendingBit(EXTI_Line0);     // 清除对应中断的标志位，从而退出中断
    }
}

int main(void) {

    LED_Init(LED_G);
    LED_Off(LED_G);

    InterruptConfig();
    EXTI_Config();

    // 6. 啥也不管，中断会自己发生
    loop;
}

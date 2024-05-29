/**
 * @brief ch04.1 熟悉时钟配置过程，并尝试修改关键参数
 * 
 * @note
 * 外部晶振为 8MHz ，分别设置运行频率为 HSE x15 x9 x3、HSI ，并通过 LED 灯切换速度观察程序运行速度
*/

#include "main.h"
#include "bsp/led.h"


static void UpdateSysClock(uint32_t RCC_PLLMul) {

    // 0. 如果配置过 RCC ，就需要恢复初始状态
    RCC_DeInit();

    // 1. 使能 HSE
    RCC_HSEConfig(RCC_HSE_ON);

    // 2. 等待 HSE 启动，并预设错误处理
    if (RCC_WaitForHSEStartUp() == SUCCESS) {

        // 3. 配置 PLL
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul);    // 时钟来源 HSE ，倍频
        RCC_PLLCmd(ENABLE);                                   // 开启 PLL
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  // 等待 PLL 稳定

        // 4. 设置系统时钟 SYSCLK 
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // 切换为 PLL 时钟
        while (RCC_GetSYSCLKSource() != 0x08);      // 确保 PLL 时钟被选为系统时钟

        // 5. 设置外设时钟
        RCC_HCLKConfig(RCC_SYSCLK_Div1);  // AHB 1 分频，HCLK = SYSCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);   // APB2 1 分频，PCLK2 = HCLK
        RCC_PCLK1Config(RCC_HCLK_Div2);   // APB1 2 分频，PCLK1 = HCLK/2
    }
    else { 
        // HSE 开启失败
        // 此时 HSI 仍为系统时钟
        while (1);
    }
}

static void HSI_Enable(void) {
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_OFF);                          // 关闭 HSE
    RCC_HSICmd(ENABLE);                                  // 启动 HSI
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET); // 等待 HSI 准备就绪
}

int main(void) {

    LED_Init(LED_B);
    LED_Off(LED_B);


    UpdateSysClock(RCC_PLLMul_15);
    for (int i = 0; i < 6; i++) {
        SoftDelay(DELAY_MEDIUM);
        LED_Toggle(LED_B);
    }

    UpdateSysClock(RCC_PLLMul_9);
    for (int i = 0; i < 6; i++) {
        SoftDelay(DELAY_MEDIUM);
        LED_Toggle(LED_B);
    }

    UpdateSysClock(RCC_PLLMul_3);
    for (int i = 0; i < 6; i++) {
        SoftDelay(DELAY_MEDIUM);
        LED_Toggle(LED_B);
    }

    HSI_Enable();
    for (int i = 0; i < 6; i++) {
        SoftDelay(DELAY_MEDIUM);
        LED_Toggle(LED_B);
    }

    loop;
}

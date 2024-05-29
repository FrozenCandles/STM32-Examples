/**
 * @brief ch04.4 使用 MCO 对外输出时钟
 * 
 * @note
 * 将 PA8 接示波器即可观察到现象
*/

#include "main.h"


static void UpdateSysClock(uint32_t RCC_PLLMul);

static void MCO_Config(void) {

    // 1. 初始化 PA8 ，注意模式选择为复用推挽输出
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_AF_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_8
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 2. 配置 MCO 时钟为 SYSCLK
    RCC_MCOConfig(RCC_MCO_SYSCLK);
}

int main(void) {

    MCO_Config();

    loop;
}

static void UpdateSysClock(uint32_t RCC_PLLMul) {

    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);

    if (RCC_WaitForHSEStartUp() == SUCCESS) {

        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul);
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while (RCC_GetSYSCLKSource() != 0x08);

        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
    }
    else { 
        while (1);
    }
}
/**
 * @brief ch03.1 使用串口向 PC 发送消息
 * 
 * @note
 * 将 PA9(Tx) 接 PC 端的 Rx
 * 在 PC 中，使用任意串口调试助手就可以观察到串口发送的消息
*/

#include "main.h"


static void USART_Config(void) {

    // 1. 初始化负责输出的 GPIO 为复用推挽输出模式
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_9,
        .GPIO_Mode = GPIO_Mode_AF_PP
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. 初始化 USART
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef USART_InitStructure = {
        .USART_BaudRate = 115200,                 // 波特率 115200
        .USART_WordLength = USART_WordLength_8b,  // 字长 8 bit
        .USART_StopBits = USART_StopBits_1,       // 1 位停止位
        .USART_Parity = USART_Parity_No,          // 无校验位
        .USART_Mode = USART_Mode_Tx,              // 仅发送
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    };                                            // 不需要硬件流控制
    USART_Init(USART1, &USART_InitStructure);

    // 3. 使能 USART
    USART_Cmd(USART1, ENABLE);
}

char Message[] = "Hello STM32";

int main(void) {

    USART_Config();

    volatile uint16_t _ = USART1->SR;
    for (uint32_t i = 0; i < sizeof(Message); i++) {
        // 4. 发送数据
        USART_SendData(USART1, Message[i]);
        // 5. 等待发送结束
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }

    loop;
}

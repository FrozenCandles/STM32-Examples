/**
 * @brief ch03.2 使用轮询方式，从串口接收来自 PC 的消息，并原封不动的返回
 * 
 * @note
 * 将 PA9(Tx) 接 PC 端的 Rx ，PA10(Rx) 接串口的 Tx
 * 在 PC 中，使用任意串口调试助手主动发送信息，就可以观察到串口返回的消息
*/

#include "main.h"


static void USART_Config(void) {

    // 1. 初始化负责输出的 GPIO ，发送端为复用推挽输出模式，接收端为浮空输入模式
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz,
    }; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. 初始化并使能 USART
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef USART_InitStructure = {
        .USART_BaudRate = 115200,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,         // 收发都要
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    };
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

int main(void) {

    USART_Config();

    loop {
        // 3. 使用对应函数收发数据
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
            char Data = (char) USART_ReceiveData(USART1);
            volatile uint16_t _ = USART1->SR;
            USART_SendData(USART1, Data);
            while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);

            //USART_ClearFlag(USART1, USART_FLAG_RXNE);
        }
    }
}



#include "usart.h"


static void USART1_GPIO_Init(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   // Tx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  // Rx
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART1_Config(uint32_t BaudRate) {

    USART1_GPIO_Init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef USART_InitStructure = {
        .USART_BaudRate = BaudRate,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    };
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

void USART_SendChar(USART_TypeDef* USARTx, char ch) {
    volatile uint16_t _ = USART1->SR;
    USART_SendData(USARTx, ch);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

void USART_SendString(USART_TypeDef* USARTx, const char* String) {
    unsigned int i = 0;
    do {
        USART_SendChar(USARTx, *(String + i));
        i++;
    } while (*(String + i) != '\0');
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

void USART_SendBytes(USART_TypeDef* USARTx, uint8_t* Bytes, uint16_t Size) {
    for (uint16_t i = 0; i < Size; i++)
        USART_SendChar(USARTx, Bytes[i]);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

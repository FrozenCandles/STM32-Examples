
#include <stdio.h>

#include "main.h"

void USART1_Config(uint32_t BaudRate);

#define USART1_SendChar(ch) USART_SendChar(USART1, ch)

#define USART1_SendString(str) USART_SendString(USART1, str)

void USART_SendChar(USART_TypeDef* USARTx, char ch);

void USART_SendString(USART_TypeDef* USARTx, const char* str);

void USART_SendBytes(USART_TypeDef* USARTx, uint8_t* Bytes, uint16_t Size);

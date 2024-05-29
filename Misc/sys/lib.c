
#include "main.h"
#include "bsp/usart.h"


#ifdef USE_LIB
int fputc(int ch, FILE *f) {
    volatile uint16_t _ = USART1->SR;
    USART_SendChar(USART1, (char)ch);
    return ch;
}

int fgetc(FILE *f) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (int)USART_ReceiveData(USART1);
}

#endif


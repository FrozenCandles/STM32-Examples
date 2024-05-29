/**
 * @brief ch03.3 PC 通过串口控制 MCU
 * 
 * @note
 * PC 发送 `LED-[RGB]-(ON|OFF)` ，MCU 视情况控制 LED 并返回 
 * + `LED-[RGB] is (on|off)`
 * + `LED-[RGB] turned (on|off)`
 * + `Error: no such LED`
 * + `Error: Invalid status`
 * + `Error: Invalid command`
 * 注意：本节将使用 Keil 提供的 MicroLib ，利用 `stdio.h` 提供的标准输入/输出功能
 *       在编译前，需要将 `Mics/main.h` 中的宏 `USE_LIB` 取消定义，
 *       或删除本节定义的 `fputc()` 及 `fgetc()` 函数，以免产生重复定义的问题
*/

#include "main.h"
#include "bsp/led.h"


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

    // 3. 配置中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

static char* ToLowerCase(char* string) {
    for (char *p = string; *p; ++p) 
        *p = *p >= 'A' && *p <= 'Z' ? *p | 0x60 : *p;
    return string;
}

char CommandBuffer[50];
void ParseCommand(void);

int main(void) {

    LED_Init(LED_R);
    LED_Init(LED_G);
    LED_Init(LED_B);
    USART_Config();

    puts("System initialized");

    loop {
        fgets(CommandBuffer, 49, stdin);
        ParseCommand();
    }
}

void ParseCommand(void) {

    char LED_Selected;
    char LED_NewStatus[10];
    bool LED_StatusSelected;
    ToLowerCase(CommandBuffer);

    if (sscanf(CommandBuffer, "led-%c-%s", &LED_Selected, LED_NewStatus) == 2) {
        if (strcmp(LED_NewStatus, "on") == 0)
            LED_StatusSelected = 0;
        else if (strcmp(LED_NewStatus, "off") == 0)
            LED_StatusSelected = 1;
        else {
            puts("Error: Invalid status");
            return;
        }

        switch (LED_Selected) {
            case 'r':
                if (LED_Status(LED_R) == LED_StatusSelected)
                    printf("LED R is %s\n", LED_NewStatus);
                else {
                    LED_Turn(LED_R, LED_StatusSelected);
                    printf("LED R turned %s\n", LED_NewStatus);
                }
                return;
            case 'g':
                if (LED_Status(LED_G) == LED_StatusSelected)
                    printf("LED G is %s\n", LED_NewStatus);
                else {
                    LED_Turn(LED_G, LED_StatusSelected);
                    printf("LED G turned %s\n", LED_NewStatus);
                }
                return;
            case 'b':
                if (LED_Status(LED_B) == LED_StatusSelected)
                    printf("LED B is %s\n", LED_NewStatus);
                else {
                    LED_Turn(LED_B, LED_StatusSelected);
                    printf("LED B turned %s\n", LED_NewStatus);
                }
                return;
            default:
                puts("Error: no such LED");
                return;
        }
    }
    else {
        puts("Error: Invalid command");
    }
}

#if 0

int fputc(int ch, FILE *f) {
    USART_SendChar(USART1, (char)ch);
    return ch;
}

int fgetc(FILE *f) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != SET);
    return (int)USART_ReceiveData(USART1);
}

#endif

/**
 * @brief ch03.5 综合以上串口技术，利用串口1接收 PC 的消息，然后转发给串口 3 ，并将串口 3 的消息再转发回串口 1
 * 
 * @note
 * 本实验通信的设备是 ESP8266 ，它与 STM32 的引脚接法为：
 * PB10 <-> RXD 
 * PB11 <-> TXD
 * PB8  <-> CH-PD 
 * PB9  <-> RST
 * 
 * 运行本程序后，在 PC 端使用串口调试助手依次发送以下信息：
 * （每条信息都需要包括换行符）
 * （上电后可能会看到一些乱码，但不重要）

AT

ATE0

AT+CWMODE=2

AT+CIPAP="192.168.10.1"

AT+CWSAP="ESP8266","abc123456",4,4

AT+CIPMODE=0

AT+CIPMUX=1

AT+CIPSERVER=1,8266

 * 然后，使用任意能连接 WIFI 的设备，就可以看到 WIFI 列表多出了一个名为
 * "ESP8266"的 WIFI ，输入密码 "abc123456" 即可连接上该 WIFI 。
 * 
 * 连接上该 WIFI 后，运行本目录下的 `send_demo.py` 脚本，就可以在 PC 端的
 * 串口调试助手内看到通过 WIFI 发送的消息了。
 * 
 * 注：本实验只有在局域网内有效。
*/

#include "main.h"


#include "preprocessor.h"
#define ESP8266_RST    PB, 9
#define ESP8266_CH_PD  PB, 8

#define SerialBufferDataLength 512
typedef struct SerialBuffer {
    char Data[SerialBufferDataLength];
    uint16_t Length;
} SerialBufferType;

SerialBufferType DebugSerialBuffer = Initial;
SerialBufferType ESP8266SerialBuffer = Initial;

static void USART_Config(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = { .GPIO_Speed = GPIO_Speed_50MHz };
    // USART1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // USART3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    USART_InitTypeDef USART_InitStructure = {
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None
    };
    USART_InitStructure.USART_BaudRate = 115200;
    USART_Init(USART1, &USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

static void InterruptConfig(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
}

void ESP8266_Reset(void) {
    GPIO_RESET(ESP8266_RST);    // 复位 I/O 拉低
    delay(500, ms);             // 短暂延时
    GPIO_SET(ESP8266_RST);      // 复位 I/O 拉高
}

void ESP8266_Init(void) {
    GPIO_INIT(ESP8266_RST, Out_PP, 50MHz);
    GPIO_INIT(ESP8266_CH_PD, Out_PP, 50MHz);
    GPIO_SET(ESP8266_RST);
    GPIO_SET(ESP8266_CH_PD);
    delay(100, ms);
}

static void RedirectMessage(void) {
    unsigned int i = 0;
    do {
        volatile uint16_t _ = USART3->SR;
        USART_SendData(USART3, DebugSerialBuffer.Data[i]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        i++;
    } while (DebugSerialBuffer.Data[i] != '\0');
}

void USART1_IRQHandler(void) {
    // 接收到单个字符
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {		
		char Data = USART_ReceiveData(USART1);
        DebugSerialBuffer.Data[DebugSerialBuffer.Length++] = Data;
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
    // 数据帧接收完毕
    if (USART_GetITStatus(USART1, USART_IT_IDLE) == SET) {
        DebugSerialBuffer.Data[DebugSerialBuffer.Length] = '\0';

        DebugSerialBuffer.Length = 0;
        fputs("STM32 接收到了 PC 数据：", stdout);
        puts(DebugSerialBuffer.Data);
        RedirectMessage();
        // 由软件序列清除中断标志位 先读 USART_SR 然后读 USART_DR
        volatile uint16_t temp;
        temp = USART1->SR;
        temp = USART1->DR;
    }
}

void USART3_IRQHandler(void) {
    // 接收到单个字符
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {		
		char Data = USART_ReceiveData(USART3);
        ESP8266SerialBuffer.Data[ESP8266SerialBuffer.Length++] = Data;
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	}
    // 数据帧接收完毕
    if (USART_GetITStatus(USART3, USART_IT_IDLE) == SET) {
        ESP8266SerialBuffer.Data[ESP8266SerialBuffer.Length] = '\0';

        ESP8266SerialBuffer.Length = 0;
        fputs("STM32 接收到了 ESP8266 数据：", stdout);
        puts(ESP8266SerialBuffer.Data);

        // 由软件序列清除中断标志位 先读 USART_SR 然后读 USART_DR
        volatile uint16_t temp;
        temp = USART3->SR;
        temp = USART3->DR;
    }
}

int main(void) {
    ESP8266_Init();
    ESP8266_Reset();
    USART_Config();
    InterruptConfig();

    loop;
}

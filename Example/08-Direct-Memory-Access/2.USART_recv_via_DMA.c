/**
 * @brief ch08.2 通过 DMA 接收串口输入的内容，然后通过软件发回
 *
*/

#include "main.h"
#include "bsp/usart.h"


#define ReceiveBufferSize 512
uint8_t ReceiveBuffer[ReceiveBufferSize];

void DMA_Config(void) {

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef DMA_InitStructure = {
        // 数据的源和目标
        .DMA_PeripheralBaseAddr = (uint32_t) &(USART1->DR), // 外设地址：串口数据寄存器地址
        .DMA_MemoryBaseAddr = (uint32_t)ReceiveBuffer,      // 内存地址：缓存区地址
        .DMA_DIR = DMA_DIR_PeripheralSRC,                   // 方向：从外设到内存
        // 数据的量
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte, // 传输单位为字节
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
        .DMA_BufferSize = ReceiveBufferSize,                  // 传输大小为缓存大小
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,       // 外设地址不增
        .DMA_MemoryInc = DMA_MemoryInc_Enable,                // 内存地址自增
        .DMA_Mode = DMA_Mode_Circular,                        // 循环传输模式
        // 通道的其它参数
        .DMA_Priority = DMA_Priority_Medium,  // 优先级：中
        .DMA_M2M = DMA_M2M_Disable,           // 禁止内存到内存的传输
    };
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    DMA_Cmd (DMA1_Channel5, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

void InterruptConfig(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

void USART1_IRQHandler(void) {

    if (USART_GetITStatus(USART1, USART_IT_IDLE) == SET) {

        // 先关闭 DMA
        DMA_Cmd(DMA1_Channel5, DISABLE);

        // 再获取计数器剩余值
        uint16_t CurrCount = DMA_GetCurrDataCounter(DMA1_Channel5);

        // 向 PC 返回数据（接收数据数量 = 计数器实际递减的值）
        USART_SendBytes(USART1, ReceiveBuffer, ReceiveBufferSize - CurrCount);

        // 重新设置传输的数据数量
        DMA_SetCurrDataCounter(DMA1_Channel5, ReceiveBufferSize);

        // 开启DMA传输
        DMA_Cmd(DMA1_Channel5, ENABLE);

        USART_ClearFlag(USART1, USART_FLAG_IDLE);
        // 注：清串口空闲中断位还要读数据寄存器
        USART_ReceiveData(USART1);
    }
}

int main(void) {

    USART1_Config(115200);
    InterruptConfig();
    DMA_Config();

    loop;
}


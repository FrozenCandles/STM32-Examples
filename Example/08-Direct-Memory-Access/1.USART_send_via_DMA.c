/**
 * @brief ch08.1 通过 DMA 使串口输出内容，CPU 一旁摸鱼
 * 
*/

#include "main.h"
#include "bsp/usart.h"


char SendBuffer[] = "你说得对，但是意法半导体的STM32F1系列主流MCU满足了工业、医疗和消费类市场的各种应用需求。凭借该产品系列，意法半导体在全球Arm® Cortex™-M内核微控制器领域处于领先地位，同时树立了嵌入式应用历史上的里程碑。该系列在集成一流的外设和低功耗、低电压运行等基础上实现了高性能，同时还以可接受的价格实现了高集成度，并具有简单的架构和简便易用的工具。\n该系列包含五个产品线，它们的引脚、外设和软件均兼容。\n* 超值型STM32F100 - 24 MHz CPU，具有电机控制和CEC功能\n* STM32F101 - 36 MHz CPU，具有高达1MB的Flash\n* STM32F102 – 48 MHz CPU具备USB FS\n* STM32F103 - 72 MHz，具有高达1MB的Flash、电机控制、USB和CAN\n* STM32F105/107 - 72 MHz CPU，具有以太网MAC、CAN和USB 2.0 OTG";

void DMA_Config(void) {

    // 1. 打开 DMA 时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 2. 初始化 DMA
    DMA_InitTypeDef DMA_InitStructure = {
        // 数据的源和目标
        .DMA_PeripheralBaseAddr = (uint32_t) &(USART1->DR),  // 外设地址为串口的数据寄存器地址
        .DMA_MemoryBaseAddr = (uint32_t) SendBuffer,         // 内存地址为数据缓冲区的地址
        .DMA_DIR = DMA_DIR_PeripheralDST,                    // 设置传输方向为存储器到外设
        // 数据的量
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte, // 外设数据单位	
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,         // 内存数据单位
        .DMA_BufferSize = sizeof(SendBuffer),                  // 数据个数
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,        // 外设地址不增
        .DMA_MemoryInc = DMA_MemoryInc_Enable,                 // 内存地址自增
        .DMA_Mode = DMA_Mode_Normal,                           // 传输一次即可
        // 通道的其它参数
        .DMA_Priority = DMA_Priority_Medium,  // 优先级：中	
        .DMA_M2M = DMA_M2M_Disable            // 不需要内存到内存的传输
    };   
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    // 3. 使能 DMA
    DMA_Cmd(DMA1_Channel4, ENABLE);

    // 4. 使能 USART 的 DMA 触发
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

int main(void) {

    USART1_Config(115200);
    DMA_Config();

    loop;  // CPU 摸鱼
}

/**
 * @brief 通过 TIM6 控制的 DMA 使 DAC 输出正弦波，CPU 全程看戏
 * 
*/

#include "main.h"


// 正弦表，一个周期点的个数由生成脚本确定
uint16_t SinWaveTable[] = {
#   include "sin_wave.txt"
};

#define SinWaveTableLength (sizeof(SinWaveTable) / sizeof(*SinWaveTable))

// DAC DHR12RD 寄存器，12位、右对齐、双通道
#define DAC_DHR12RD_ADDRESS    (DAC_BASE + 0x08) 

static void TIM_Config(void) {
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
        .TIM_Period = 20 - 1,
        .TIM_Prescaler = 0,
    };
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    /* 配置TIM2触发源 */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    TIM_Cmd(TIM6, ENABLE);
}


static void DAC_Config(void) {

    // 初始化 GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = GPIO_Pin_4,
        .GPIO_Mode = GPIO_Mode_AIN
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化 DAC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    DAC_InitTypeDef DAC_InitStructure = {
        .DAC_Trigger = DAC_Trigger_T6_TRGO,             // 使用TIM2作为触发源
        .DAC_WaveGeneration = DAC_WaveGeneration_None,  // 不使用波形发生器
        .DAC_OutputBuffer = DAC_OutputBuffer_Disable,   // 不使用DAC输出缓冲
    };  
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // 使能 DAC 的 DMA 请求
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

static void DMA_Config(void) {

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    DMA_InitTypeDef DMA_InitStructure = {
        .DMA_PeripheralBaseAddr = DAC_DHR12RD_ADDRESS,  //外设数据地址
        .DMA_MemoryBaseAddr = (uint32_t) SinWaveTable ,  //内存数据地址
        .DMA_DIR = DMA_DIR_PeripheralDST,//数据传输方向内存至外设
        .DMA_BufferSize = SinWaveTableLength,  //缓存大小
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,  //外设数据地址固定    
        .DMA_MemoryInc = DMA_MemoryInc_Enable,  //内存数据地址自增
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,// 外设数据以字为单位
        .DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord,  //内存数据以字为单位    
        .DMA_Mode = DMA_Mode_Circular,  //循环模式
        .DMA_Priority = DMA_Priority_High,  //高DMA通道优先级
        .DMA_M2M = DMA_M2M_Disable  // 非内存至内存模式    
    };
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);

    DMA_Cmd(DMA2_Channel3, ENABLE);
}

int main(void) {
    TIM_Config();
    DMA_Config();
    DAC_Config();

    loop;  // 看戏
}

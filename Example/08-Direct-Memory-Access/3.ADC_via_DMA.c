/**
 * @brief ch08.3 通过 DMA 搬运 ADC 采集的多通道数据
 * 
 * @note
 * 这是多通道 ADC 采集的标准做法
 * 
*/

#include "main.h"
#include "bsp/usart.h"


uint16_t ADC_Values[4];

static void ADC_Config(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_AIN,
        .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure = {
        .ADC_Mode = ADC_Mode_Independent,
        .ADC_ScanConvMode = ENABLE,
        .ADC_ContinuousConvMode = ENABLE,
        .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
        .ADC_DataAlign = ADC_DataAlign_Right,
        .ADC_NbrOfChannel = 4
    };
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_41Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_41Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 3, ADC_SampleTime_41Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 4, ADC_SampleTime_41Cycles5);
    
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

static void DMA_Config(void) {

    // 1. 打开时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 2. 初始化 DMA
    DMA_InitTypeDef DMA_InitStructure = {
        // 数据的源和目标
        .DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR),  // 外设地址为 ADC 的数据寄存器地址
        .DMA_MemoryBaseAddr = (uint32_t) ADC_Values,       // 内存地址为数据缓冲区的地址
        .DMA_DIR = DMA_DIR_PeripheralSRC,                  // 设置传输方向为外设到存储器
        // 数据的量
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord, // 外设数据单位
        .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,         // 内存数据单位
        .DMA_BufferSize = 4,                                       // 数据个数
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,            // 外设地址不增
        .DMA_MemoryInc = DMA_MemoryInc_Enable,                     // 内存地址自增
        .DMA_Mode = DMA_Mode_Circular,                             // 需要循环传输
        // 通道的其它参数
        .DMA_Priority = DMA_Priority_High,    // 优先级：高
        .DMA_M2M = DMA_M2M_Disable            // 不需要内存到内存的传输
    };
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // 3. 使能 DMA 通道
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 4. 使能 ADC 的 DMA 功能
    ADC_DMACmd(ADC1, ENABLE);
}

static inline float ToLocalVoltage(uint16_t Value) {
    return (float) Value / 4096 * 3.3;
}

int main(void) {

    USART1_Config(115200);
    puts("System initialized");

    ADC_Config();
    DMA_Config();

    loop {
        printf("CH0 value = %.4f V.\n", ToLocalVoltage(ADC_Values[0]));
        printf("CH1 value = %.4f V.\n", ToLocalVoltage(ADC_Values[1]));
        printf("CH2 value = %.4f V.\n", ToLocalVoltage(ADC_Values[2]));
        printf("CH3 value = %.4f V.\n", ToLocalVoltage(ADC_Values[3]));

		SoftDelay(DELAY_LONG);
    }
}

/**
 * @brief ch08.2 通过中断的方式采集模拟输入量
 * 
*/

#include "main.h"
#include "bsp/usart.h"


static void ADC_Config(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_AIN,
        .GPIO_Pin = GPIO_Pin_1
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure = {
        .ADC_Mode = ADC_Mode_Independent,
        .ADC_ScanConvMode = DISABLE,
        .ADC_ContinuousConvMode = ENABLE,  // 连续转换模式
        .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
        .ADC_DataAlign = ADC_DataAlign_Right,
        .ADC_NbrOfChannel = 1
    };
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

volatile uint16_t ADC_Value;

static void InterruptConfig(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = ADC1_2_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    };
    NVIC_Init(&NVIC_InitStructure);
    
    // 启用 ADC 转换中断
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

// 在中断处理函数中完成 ADC 读取
void ADC1_2_IRQHandler(void) {
    // 检查转换完成中断
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
        // 读取转换结果
        ADC_Value = ADC_GetConversionValue(ADC1);
        // 清除转换完成中断标志
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}

static inline float ToLocalVoltage(uint16_t RawValue) {
    return ((float)(RawValue * 3.3 / 4095));
}

int main(void) {

    USART1_Config(115200);
    
    ADC_Config();
    InterruptConfig();

    puts("System initialized");

    loop {

        float AnalogInput = ToLocalVoltage(ADC_Value);

        printf("Analog input %.4f\n", AnalogInput);

        SoftDelay(DELAY_MEDIUM);
    }
}


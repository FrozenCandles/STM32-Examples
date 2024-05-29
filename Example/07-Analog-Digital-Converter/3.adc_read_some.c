/**
 * @brief ch08.3 通过注入序列采集两个外部通道和两个内部通道的模拟量
 * 
 * @note
 * 采集以下通道的数据：
 * 1. 通道 10 ：对应 PC0 ，接外部可变电阻
 * 2. 通道 11 ：对应 PC1 ，接 Vcc
 * 3. 通道 16 ：接内部 Vrefint
 * 4. 通道 17 ：接内部温度传感器
 * 
*/

#include "main.h"
#include "bsp/usart.h"


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
        .ADC_ScanConvMode = ENABLE,        // 多通道扫描模式
        .ADC_ContinuousConvMode = ENABLE,  // 连续转换模式
        .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
        .ADC_DataAlign = ADC_DataAlign_Right,
            // 不使用规则通道
    };
    ADC_Init(ADC1, &ADC_InitStructure);

    // 配置注入通道模式
    ADC_InjectedSequencerLengthConfig(ADC1, 4);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);
    ADC_AutoInjectedConvCmd(ADC1, ENABLE);

    // 配置 ADC 各通道
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_16, 3, ADC_SampleTime_55Cycles5);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_17, 4, ADC_SampleTime_55Cycles5);

    // 读取内部温度传感器和 Vrefint 的值，需要先使能这两个通道
    ADC_TempSensorVrefintCmd(ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t ADC_Values[4];

static void InterruptConfig(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = ADC1_2_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    };
    NVIC_Init(&NVIC_InitStructure);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

// ADC 转换结束产生中断，在中断服务程序中读取转换值
// 在转换完成中断中读取转换结果
void ADC1_2_IRQHandler(void) {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
        // 读取转换结果
        ADC_Values[0] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_Values[1] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
        ADC_Values[2] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3);
        ADC_Values[3] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4);

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

        float AnalogInput = ToLocalVoltage(ADC_Values[0]);
        float VoltageInput = ToLocalVoltage(ADC_Values[1]);
        float InternalTemperature = ToLocalVoltage(ADC_Values[2]);
        float InternalRefVoltage = ToLocalVoltage(ADC_Values[3]);

        printf("Analog input %.4f\n", AnalogInput);
        printf("Input voltage %.4f\n", VoltageInput);
        printf("Internal temperature %.4f\n", InternalTemperature);
        printf("Internal reference voltage %.4f\n", InternalRefVoltage);

        SoftDelay(DELAY_LONG);
    }
}


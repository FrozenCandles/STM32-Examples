/**
 * @brief ch08.1 通过 ADC 采集模拟输入量
 * 
 * @note
 * 采集通道 11 ：对应 PC1 ，接外部可变电阻，使得输入电压范围在 0 到 3.3V 之间
 * 
*/

#include "main.h"
#include "bsp/usart.h"


static void ADC_Config(void) {

    // 1. 打开 ADC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // 2. 配置 GPIO
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_AIN,
        .GPIO_Pin = GPIO_Pin_1
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. 配置 ADC
    ADC_InitTypeDef ADC_InitStructure = {
        .ADC_Mode = ADC_Mode_Independent,
        .ADC_ScanConvMode = DISABLE,
        .ADC_ContinuousConvMode = DISABLE,
        .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
        .ADC_DataAlign = ADC_DataAlign_Right,
        .ADC_NbrOfChannel = 1
    };
    ADC_Init(ADC1, &ADC_InitStructure);

    // 4. 配置 ADC 通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);

    // 5. 启用 ADC
    ADC_Cmd(ADC1, ENABLE);

    // 6. 校准 ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

static inline float ToLocalVoltage(uint16_t RawValue) {
    return ((float)(RawValue * 3.3 / 4095));
}

int main(void) {

    USART1_Config(115200);
    ADC_Config();

    puts("System initialized");

    loop {

        // 由于没有采用外部触发，所以使用软件触发ADC转换
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        // 等待本次转换结束
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
        // 读取本次转换值
        uint16_t ADC_Value = ADC_GetConversionValue(ADC1);

        float AnalogInput = ToLocalVoltage(ADC_Value);

        printf("Analog input %.4f\n", AnalogInput);

        SoftDelay(DELAY_MEDIUM);
    }
}


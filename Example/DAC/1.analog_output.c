/**
 * @brief 通过 DAC 输出正弦波
 * 
 * @note
 * 由 PA4 输出，对应 DAC 通道 1 ，可通过示波器观察现象
*/

#include "main.h"


// 正弦表，一个周期点的个数由生成脚本确定
const uint16_t SinWaveTable[] = {
#   include "sin_wave.txt"
};

static void DAC_Config(void) {

    // 1. 初始化 GPIO 为模拟输入模式
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = GPIO_Pin_4,
        .GPIO_Mode = GPIO_Mode_AIN
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. 初始化 DAC 通道 1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    DAC_InitTypeDef DAC_InitStructure = {
        .DAC_Trigger = DAC_Trigger_None,                // 不使用触发源
        .DAC_WaveGeneration = DAC_WaveGeneration_None,  // 不使用波形发生器
        .DAC_OutputBuffer = DAC_OutputBuffer_Disable,   // 不使用 DAC 输出缓冲
    };
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // 3. 使能 DAC 通道 1
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // 4. 初始化 DAC 的输出值
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

int main(void) {

    DAC_Config();
    
    loop {
        for (int i = 0; i < sizeof(SinWaveTable) / sizeof(SinWaveTable[0]); i++) {
            DAC_SetChannel1Data(DAC_Align_12b_R, SinWaveTable[i]);
            delay(10, ms);
        }
    }
}

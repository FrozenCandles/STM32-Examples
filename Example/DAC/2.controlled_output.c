/**
 * @brief 通过按键控制 DAC 输出的幅度
 * 
 * @note
 * 两个按键，按键 1 每按一下 DAC 通道的值下降 10% ，
 * 按键 2 每按一下 DAC 通道的值上升 10%
 * 升降到最大/小值后不动
*/

#include "main.h"
#include "bsp/key.h"


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

uint16_t DAC_Value = 0;

int main(void) {

    Key_Init(KEY1);
    Key_Init(KEY2);
    DAC_Config();

    loop {
        if (Key_Pressed(KEY1)) {
            if (DAC_Value >= 409)
                DAC_Value -= 409;
            DAC_SetChannel1Data(DAC_Align_12b_R, DAC_Value);
            while (Key_Pressed(KEY1));
        }
        if (Key_Pressed(KEY2)) {
            if (DAC_Value <= 4096 - 409)
                DAC_Value += 409;
            DAC_SetChannel1Data(DAC_Align_12b_R, DAC_Value);
            while (Key_Pressed(KEY2));
        }
    }
}

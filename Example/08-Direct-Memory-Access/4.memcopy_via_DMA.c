/**
 * @brief ch08.4 通过 DMA 复制内存数据
 *
 * @note
 * 复制成功亮绿灯，复制失败亮红灯
 *
*/

#include "main.h"
#include "bsp/led.h"

const uint32_t Data[]= {
#   include "data.txt"
};

uint32_t Buffer[sizeof(Data) / sizeof(Data[0])];

void DMA_memcpy(void* restrict Dst, const void* restrict Src, size_t Size) {

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure = {
        // 数据的源和目标
        .DMA_PeripheralBaseAddr = (uint32_t)Src, // 源数据地址
        .DMA_MemoryBaseAddr = (uint32_t)Dst,     // 目标地址
        .DMA_DIR = DMA_DIR_PeripheralSRC,        // 方向：外设(其实也是存储器)到存储器
        // 数据的量
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte, // 外设数据单位
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,         // 内存数据单位
        .DMA_BufferSize = Size,                                // 数据个数
        .DMA_PeripheralInc = DMA_PeripheralInc_Enable,         // 外设地址递增
        .DMA_MemoryInc = DMA_MemoryInc_Enable,                 // 内存地址递增
        .DMA_Mode = DMA_Mode_Normal,                           // 一次传输
        // 通道的其它参数
        .DMA_Priority = DMA_Priority_Low,   // 优先级：低
        .DMA_M2M = DMA_M2M_Enable,          // 使能存储器到存储器的传输
    };
    // 当使用存储器到存储器模式时候，通道可以随便选，没有硬性的规定
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    DMA_ClearFlag(DMA1_FLAG_TC6);
    DMA_Cmd(DMA1_Channel6, ENABLE);

    // 等待 DMA 传输完成
    while (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);

}


int main(void) {

    LED_Init(LED_R);
    LED_Init(LED_G);
    LED_Off(LED_R);
    LED_Off(LED_G);

    delay(2, s);
    // 复制前，确认两者不一样
    if (memcmp(Buffer, Data, sizeof(Data)) == 0) {
        LED_On(LED_G); LED_Off(LED_R);
    }
    else {
        LED_On(LED_R); LED_Off(LED_G);
    }

    delay(2, s);
    DMA_memcpy(Buffer, Data, sizeof(Data));
    // 复制完，判断两者应该一样
    if (memcmp(Buffer, Data, sizeof(Data)) == 0) {
        LED_On(LED_G); LED_Off(LED_R);
    }
    else {
        LED_On(LED_R); LED_Off(LED_G);
    }

    delay(2, s);
    // 偷偷修改结果，现在两者应该又不一样了
    Buffer[0] = UINT32_MAX;
    if (memcmp(Buffer, Data, sizeof(Data)) == 0) {
        LED_On(LED_G); LED_Off(LED_R);
    }
    else {
        LED_On(LED_R); LED_Off(LED_G);
    }

    loop;
}

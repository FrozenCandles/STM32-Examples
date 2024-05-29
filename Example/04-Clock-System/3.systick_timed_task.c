/**
 * @brief ch04.3 使用 SysTick 中断执行定时任务：LED 灯亮暗
 * 
 * @note
 * 使用本文件时，需要暂时删除 `stm32f10x_it.c` 中 `SysTick_Handler` 的符号定义
*/

#include "main.h"
#include "bsp/led.h"


volatile uint32_t SysTick_Count;

void SysTick_Init(void) {
    // 1. 初始化定时器，使其定时周期为 1/10 秒
    if (SysTick_Config(SystemCoreClock / 10)) {
        // ...
        while (1);
    }
    // 2. 确定 10 次中断执行一个任务
    SysTick_Count = 10;
}

// 3. 在 SysTick 中断服务函数内执行定时任务
void SysTick_Handler(void) {
    if (SysTick_Count != 0)
        SysTick_Count--;
    else {
        LED_Toggle(LED_B);
        SysTick_Count = 10;
    }
}

int main(void) {

    LED_Init(LED_B);
    LED_Off(LED_B);

    SysTick_Init();

    // 4. 啥也不管，任务会自己执行
    loop;
}


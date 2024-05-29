

#include "systick_delay.h"


void systick_delay_us(unsigned int us) {
    SysTick_Config(72);
    for (int i = 0; i < us; i++) {
        // 定时器检查第 16 位是否至 1  如果为 1 说明时间到了退出循环
        while (!SysTick_CountFlag_Read()); 
    }
    SysTick_Disable();
}

void systick_delay_ms(unsigned int ms) {
    SysTick_Config(72000);
    for (int i = 0; i < ms; i++) {
        while (!SysTick_CountFlag_Read());
    }
    SysTick_Disable();
}

void systick_delay_s(unsigned int s) {
    SysTick_Config(72000);
    // 延时 s 时 int 的最大比 72000000 小所以在 for 中乘 1000 效果时一样的
    for (int i = 0; i < s * 1000; i++) {
        while (!SysTick_CountFlag_Read());
    }
    SysTick_Disable();
}

#if 0

void SysTick_Delay(uint32_t ms) {
    // 1. 初始化定时器的值，使其定时周期为 1/1000 秒
    SysTick->VAL = 0x00;
    SysTick->LOAD = SystemCoreClock / 1000;
    // 2. 开始定时
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    for (uint32_t i = 0; i < ms; i++) {
        uint32_t status;
        // 3. 定时器检查 COUNTFLAG 位是否置 1 ，同时 ENABLE 位也为 1；如果为 1 说明一轮定时结束
        do {
            status = SysTick->CTRL;
        } while (READ_BIT(status, SysTick_CTRL_ENABLE_Msk) &&
                 !READ_BIT(status, SysTick_CTRL_COUNTFLAG_Msk));
        // 等待时间到达，看CTRL的第16位（COUNTFLAG）是否为1，看STRL的第0位（）是否为1
    }
    // 关闭计数器 
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
} 
#endif

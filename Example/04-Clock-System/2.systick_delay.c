/**
 * @brief ch04.2 使用 SysTick 实现精确延时
 * 
*/

#include "main.h"
#include "bsp/led.h"


void SysTick_Delay(uint32_t ms) {
    // 1. 初始化定时器的值，使其定时周期为 1/1000 秒
    SysTick->VAL = 0x00;
    SysTick->LOAD = SystemCoreClock / 1000;
    // 2. 开始定时
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    for (uint32_t i = 0; i < ms; i++) {
        // 3. 定时器检查 COUNTFLAG 位是否置 1 ；如果为 1 说明定时结束
        while (READ_BIT(SysTick->CTRL, SysTick_CTRL_COUNTFLAG_Msk) != Bit_SET); 
    }
    // 4. 关闭计数器 
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}


int main(void) {

    LED_Init(LED_B);
    LED_Off(LED_B);

    loop {
        SysTick_Delay(1000);
        LED_Toggle(LED_B);
    }
}


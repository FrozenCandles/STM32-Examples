/**
 * @brief ch05.2 使用基本定时器计算 HC-SR04 传感器的距离
 * 
 * @note
 * 只有大容量的设备才有 TIM6 。中小容量的设备可以改用 TIM3 观察
*/

#include "main.h"
#include "bsp/usart.h"


volatile uint32_t TimerCount = 0;

static void TIM_Config(void) {
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {
        .TIM_Prescaler = /* SystemCoreClock / 1000000 - 1 */ 72 - 1,  // 定时单位为 us
        .TIM_Period = 10 - 1,  //  一次计个 10 us
    };
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    // 清除计数器更新标志位，防止立即发生中断
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);

    TIM_Cmd(TIM6, ENABLE);
}

/**
 * @brief 根据计数值计算物体距离
 * @param Count 计数值
 * @return 距离(单位：m)
 */
static inline float CalcDistanceFromCounter(uint32_t Count) {
    return ((float)Count / 100000 /* Time */ * 340 /* Speed of Sound */) / 2;
}

void InterruptConig(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = TIM6_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE
    };
    NVIC_Init(&NVIC_InitStructure);
}

void TIM6_IRQHandler(void) {
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        TimerCount++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

#include "preprocessor.h"
#define HCSR04_TRIG  PB, 8
#define HCSR04_ECHO  PB, 9

int main(void) {

    USART1_Config(115200);

    GPIO_INIT(HCSR04_TRIG, Out_PP, 50MHz);
    GPIO_INIT(HCSR04_ECHO, IPD, 50MHz);

    InterruptConig();
    TIM_Config();

    uint32_t IntervalCount;

    loop {
        GPIO_SET(HCSR04_TRIG);
        delay_us(15);
        GPIO_RESET(HCSR04_TRIG);

        while (GPIO_READ(HCSR04_ECHO) == 0);
        TimerCount = 0;
        while (GPIO_READ(HCSR04_ECHO) == 1);
        IntervalCount = TimerCount;
        if (TimerCount >= 380)
            puts("maximum distance exceeded");
        else
            printf("object distance: %.4f m.\n", CalcDistanceFromCounter(IntervalCount));

        delay(1, s);
    }
}

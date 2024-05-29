/**
 * @brief ch01.4 使用位带操作 GPIO ，实现按键控制 LED 灯亮暗
 * 
*/

#include "main.h"


#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x02000000 + ((addr & 0x00FFFFFF) << 5) + (bitnum << 2))

#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
// 把位带别名区地址转换成指针
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOA_ODR_Addr (GPIOA_BASE+12)  // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE+12)  // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE+12)  // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE+12)  // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE+12)  // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE+12)  // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE+12)  // 0x40011E0C
#define GPIOA_IDR_Addr (GPIOA_BASE+8)   // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE+8)   // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE+8)   // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE+8)   // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE+8)   // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE+8)   // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE+8)   // 0x40011E08

#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)  // 输出
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)  // 输入
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)  // 输出
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n)  // 输入
#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)  // 输出
#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n)  // 输出
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n)  // 输入
#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n)  // 输出
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n)  // 输入
#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n)  // 输出
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n)  // 输入
#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n)  // 输出
#define PGin(n)  BIT_ADDR(GPIOG_IDR_Addr, n)  // 输入


int main(void) {

    // 1. 初始化 GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
            .GPIO_Mode = GPIO_Mode_Out_PP,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. 定义 GPIO 输入输出位在位带中的地址
    #define LED_R PBout(0)
    #define LED_G PBout(1)
    #define LED_B PBout(5)
    #define KEY   PAin(0)

    // 3. 通过直接操作位带实现读写 GPIO
    LED_R = LED_G = LED_B = 0;

    loop {

        if (KEY == SET) {
            while (KEY == SET);
            LED_R = LED_G = LED_B = !LED_R;
        }

        SoftDelay(DELAY_SHORT);
    }

}

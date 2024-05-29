/**
 * @brief LED 控制相关函数
 * 
 * @note
 * 这里有 3 种控制 LED（或其它类似的输入输入端口）的方式
 * 
 * 1. 使用预处理器
 *    优点：只要定义引脚号，初始化等各种操作都可以由预处理器自动完成，
 *          且几乎没有性能损失
 *    缺点：预处理器在某些时候不能正常工作，且不易调试
 * 2. 使用位域
 *    优点：定义的 LED 实例可以像变量一样直接操作
 *    缺点：需要自行提供一个初始化函数
 * 3. 直接使用标准库
 *    优点：定义的 LED 实例支持 `LED_R | LED_G` 的串联形式
 *    缺点：如果使用串联形式，这些实例需要挂载在同一个 GPIO 外设上；
 *          不同 GPIO 外设上的实例不能使用这些函数
*/

#include "main.h"


#if defined(USE_PREPROCESSOR)
#
#   include "preprocessor.h"
#   define LED_R PB, 5
#   define LED_G PB, 0
#   define LED_B PB, 1
# 
#   define LED_Init(LED_Instance) GPIO_INIT_E(LED_Instance, Out_PP, 50MHz);
#   define LED_On GPIO_RESET
#   define LED_Off GPIO_SET
#   define LED_Toggle GPIO_TOGGLE
#   define LED_Status GPIO_READ_OUT
#   define LED_Turn GPIO_WRITE
#
#elif defined(USE_BITFIELD)
#
#   include "sys/bit_fields.h"
#   define LED_R PBout(5)
#   define LED_G PBout(0)
#   define LED_B PBout(1)
#
#   define LED_On(LED_Instance) LED_Instance = 0
#   define LED_Off(LED_Instance) LED_Instance = 1
#   define LED_Toggle(LED_Instance) LED_Instance = ~LED_Instance
#   define LED_Status(LED_Instance) (LED_Instance)
#   define LED_Turn(LED_Instance, NewStatus) LED_Instance = NewStatus
#
#else

    typedef uint16_t LED_InstanceType;
    extern LED_InstanceType LED_R;
    extern LED_InstanceType LED_G;
    extern LED_InstanceType LED_B;

    void LED_Init(LED_InstanceType LED_Instance);
    void LED_On(LED_InstanceType LED_Instance);
    void LED_Off(LED_InstanceType LED_Instance);
    void LED_Toggle(LED_InstanceType LED_Instance);
    bool LED_Status(LED_InstanceType LED_Instance);
    void LED_Turn(LED_InstanceType LED_Instance, BitAction NewStatus);

#endif

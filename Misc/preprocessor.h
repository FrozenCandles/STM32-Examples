
#ifndef __PREPROCESS_H__
#define __PREPROCESS_H__


#define PP_ARG_1(_1, ...) _1
#define PP_ARG_2(_1, _2, ...) _2

#define PP_CAT(A, B) A ## B
#define PP_CATI(A, B) PP_CAT(A, B)

#define PP_EXPAND(...) __VA_ARGS__
#define PP_MAC(Mac) Mac


#define _GPIO_PA_PORT GPIOA
#define _GPIO_PB_PORT GPIOB
#define _GPIO_PC_PORT GPIOC

#define _GPIO_0_PIN GPIO_Pin_0
#define _GPIO_1_PIN GPIO_Pin_1
#define _GPIO_2_PIN GPIO_Pin_2
#define _GPIO_3_PIN GPIO_Pin_3
#define _GPIO_4_PIN GPIO_Pin_4
#define _GPIO_5_PIN GPIO_Pin_5
#define _GPIO_6_PIN GPIO_Pin_6
#define _GPIO_7_PIN GPIO_Pin_7
#define _GPIO_8_PIN GPIO_Pin_8
#define _GPIO_9_PIN GPIO_Pin_9
#define _GPIO_10_PIN GPIO_Pin_10
#define _GPIO_11_PIN GPIO_Pin_11
#define _GPIO_12_PIN GPIO_Pin_12
#define _GPIO_13_PIN GPIO_Pin_13
#define _GPIO_14_PIN GPIO_Pin_14
#define _GPIO_15_PIN GPIO_Pin_15

#define GPIO_PORT(PortPin) PP_CATI(PP_CATI(_GPIO_, GPIO_PORT_NAME(PP_EXPAND(PortPin))), _PORT)
#define GPIO_PIN(PortPin) PP_CATI(PP_CATI(_GPIO_, GPIO_PIN_NAME(PP_EXPAND(PortPin))), _PIN)

#define GPIO_PORT_E(Port, Pin) PP_CATI(PP_CATI(_GPIO_, Port), _PORT)
#define GPIO_PIN_E(Port, Pin) PP_CATI(PP_CATI(_GPIO_, Pin), _PIN)

#define GPIO_PORT_NAME(PortPin) PP_ARG_1(PortPin)
#define GPIO_PIN_NAME(PortPin) PP_ARG_2(PortPin)

#define _GPIO_PORT_NAME_E(Port, Pin) Port
#define _GPIO_PIN_NAME_E(Port, Pin) Pin

#define GPIO_CLKCMD(PortPin) PP_CATI(PP_CATI(_GPIO_, PP_ARG_1(PortPin)), _CLKCMD)
#define GPIO_CLK(PortPin) PP_CATI(PP_CATI(_GPIO_, PP_ARG_1(PortPin)), _CLK)

#define GPIO_CLKCMD_E(Port, Pin) PP_CATI(PP_CATI(_GPIO_, Port), _CLKCMD)
#define GPIO_CLK_E(Port, Pin) PP_CATI(PP_CATI(_GPIO_, Port), _CLK)

#define _GPIO_PA_CLKCMD RCC_APB2PeriphClockCmd
#define _GPIO_PB_CLKCMD RCC_APB2PeriphClockCmd
#define _GPIO_PC_CLKCMD RCC_APB2PeriphClockCmd

#define _GPIO_PA_CLK RCC_APB2Periph_GPIOA
#define _GPIO_PB_CLK RCC_APB2Periph_GPIOB
#define _GPIO_PC_CLK RCC_APB2Periph_GPIOC

#define _GPIO_PA_CLK RCC_APB2Periph_GPIOA
#define _GPIO_PB_CLK RCC_APB2Periph_GPIOB
#define _GPIO_PC_CLK RCC_APB2Periph_GPIOC

#define GPIO_SET(PortPin) \
    GPIO_SetBits(GPIO_PORT_E(PortPin), \
                 GPIO_PIN_E(PortPin))

#define GPIO_RESET(PortPin) \
    GPIO_ResetBits(GPIO_PORT_E(PortPin), \
                   GPIO_PIN_E(PortPin))

#define GPIO_WRITE(PortPin, Bit) \
    GPIO_WriteBit(GPIO_PORT_E(PortPin), \
                 GPIO_PIN_E(PortPin), \
                  (BitAction)(Bit))

#define GPIO_TOGGLE(PortPin) \
    GPIO_WriteBit(GPIO_PORT_E(PortPin), \
                 GPIO_PIN_E(PortPin), \
                  (BitAction)((1 - GPIO_READ_E(PortPin))))

#define GPIO_READ(PortPin) \
    GPIO_ReadInputDataBit(GPIO_PORT_E(PortPin), \
                 GPIO_PIN_E(PortPin))

#define GPIO_READ_OUT(PortPin) \
    GPIO_ReadOutputDataBit(GPIO_PORT_E(PortPin), \
                 GPIO_PIN_E(PortPin))

#define GPIO_READ_E(Port, Pin) \
    GPIO_ReadInputDataBit(GPIO_PORT_E(Port, Pin), \
                 GPIO_PIN_E(Port, Pin))

#define GPIO_EN_CLOCK(PortPin) \
    GPIO_CLKCMD(GPIO_CLK(PortPin), ENABLE)

#define GPIO_INIT(PortPin, Mode, Speed) do { \
    GPIO_CLKCMD_E(PortPin)(GPIO_CLK_E(PortPin), ENABLE); \
    GPIO_InitTypeDef GPIO_InitStructure = { \
        .GPIO_Pin = GPIO_PIN_E(PortPin),   \
        .GPIO_Mode = GPIO_Mode_ ## Mode,  \
        .GPIO_Speed = GPIO_Speed_ ## Speed  \
    };  \
    GPIO_Init(GPIO_PORT_E(PortPin), &GPIO_InitStructure);    \
    } while (0)

#define GPIO_INIT_E(Port, Pin, Mode, Speed) do { \
    GPIO_CLKCMD_E(Port, Pin)(GPIO_CLK_E(Port, Pin), ENABLE); \
    GPIO_InitTypeDef GPIO_InitStructure = { \
        .GPIO_Pin = GPIO_PIN_E(Port, Pin),   \
        .GPIO_Mode = GPIO_Mode_ ## Mode,  \
        .GPIO_Speed = GPIO_Speed_ ## Speed  \
    };  \
    GPIO_Init(GPIO_PORT_E(Port, Pin), &GPIO_InitStructure);    \
    } while (0)



#endif /* __PREPROCESS_H__ */

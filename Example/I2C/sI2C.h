/**
 * @brief 软件 I2C 控制相关代码
 */
#ifndef __S_I2C_H__
#define __S_I2C_H__


#include "main.h"
#include "sys/bit_fields.h"


#define SCL PBout(6)
#define SDAout PBout(7)
#define SDAin PBin(7)

static inline void SDA_InMode(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_IPU,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_7
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static inline void SDA_OutMode(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_7
    };
   GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void sI2C_Config(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7,
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


static void sI2C_Start(void) {
    SDA_OutMode();
    SDAout = 1;
    SCL = 1;
    delay_us(10);
    SDAout = 0;
    delay_us(10);
}

static void sI2C_Stop(void) {
    SDA_OutMode();
    SDAout = 0;
    delay_us(10);
    SCL = 1;
    delay_us(10);
    SDAout = 1;
    delay_us(10);
}

/**
 * @return 响应状态
 *  @retval 1 未响应
 *  @retval 0 响应
*/
static uint8_t sI2C_WaitAck(void) {
    SDA_InMode();
    SDAout = 1;
    delay_us(5);
    SCL = 1;
    delay_us(5);

    uint8_t Timeout = 0;
    while (SDAin == 1) {
        Timeout++;
        delay_us(10);
        if (Timeout > 10) {
            sI2C_Stop();
            return 1;
        }
    }
    SCL = 0;
    delay_us(5);
    return 0;
}

static void sI2C_Ack(void) {
    SCL = 0;
    SDA_OutMode();
    SDAout = 0;
    delay_us(5);
    SCL = 1;
    delay_us(5);
    SCL = 0;
}

static void sI2C_NAck(void) {
    SCL = 0;
    SDA_OutMode();
    SDAout = 1;
    delay_us(5);
    SCL = 1;
    delay_us(5);
    SCL = 0;
}

static void sI2C_SendByte(uint8_t SendData) {
    SDA_OutMode();
    SCL = 0;
    for (uint8_t Bit = 0; Bit < 8; Bit++) {
        
        if ((SendData & 0x80) == 0x80)
            SDAout = 1;
        else
            SDAout = 0;

        SCL = 1;
        delay_us(5);  // 当前数据可靠的，趁这时赶紧读取

        SCL = 0;
        delay_us(5);  // 现在在准备数据，不要读
        SendData <<= 1;

    }
    SCL = 1;  // 释放总线
}

static uint8_t sI2C_RecvByte(void) {
    uint8_t RecvData = 0;
    SDA_InMode();
    SCL = 0;  // 先准备数据
    for (uint8_t i = 0; i < 8; i++) {
        RecvData <<= 1;
        SCL = 1;  // 准备好读了
        delay_us(5);
        if (SDAin == 1) {
            RecvData++;
        }
        SCL = 0;  // 读完了，继续准备
        delay_us(5);
    }
    return RecvData;
}

#endif /* __S_I2C_H__ */

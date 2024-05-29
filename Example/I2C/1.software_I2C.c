/**
 * @brief 通过软件控制 I2C ，随便抽两个 IO 控制 AT24C02
 * 
 * @note
 * STM32 生成若干随机数，通过串口告诉 PC 同时存入 At24C02 ；
 * 然后使 STM32 断电重启，再读取这些随机数
 * 
*/

#include "main.h"
#include "sI2C.h"
#include "bsp/usart.h"
#include "bsp/adc.h"



typedef enum {
    AT24C02_Success,
    AT24C02_DeviceNotFound,
    AT24C02_InvalidCommand,
    AT24C02_ExecutionFailed,
} AT24C02_StatusType;


AT24C02_StatusType AT24C02_WriteByte(uint8_t Address, uint8_t Data) {
    uint8_t ack = 0;
    
    sI2C_Start();

    // 设备写地址 0b10100000 = 0xA0
    sI2C_SendByte(0xA0);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_DeviceNotFound;
     
    // 要访问的数据存储地址
    sI2C_SendByte(Address);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_InvalidCommand;

    sI2C_SendByte(Data);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_ExecutionFailed;

    sI2C_Stop();
 
    return AT24C02_Success;
}

AT24C02_StatusType AT24C02_Write(uint8_t Address, uint8_t* Data, uint8_t Size) {
    uint8_t ack = 0;
    uint8_t* p = Data;
    
    sI2C_Start();

    // 设备写地址 0b10100000 = 0xA0
    sI2C_SendByte(0xA0);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_DeviceNotFound;
     
    // 要访问的数据存储地址
    sI2C_SendByte(Address);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_InvalidCommand;

    while (Size--) {
        // 写入数据
        sI2C_SendByte(*p);
        // 等待应答
        ack = sI2C_WaitAck();
        if (ack == 1)
            return AT24C02_ExecutionFailed;
        p++;
    }

    sI2C_Stop();
 
    return AT24C02_Success;
}

AT24C02_StatusType AT24C02_ReadByte(uint8_t Address, uint8_t* Data) {
    uint8_t ack = 0;
    
    // 开始信号
    sI2C_Start();
    // 设备写访问地址 0xA0
    sI2C_SendByte(0xA0);
     
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_DeviceNotFound;
     
    // 要访问的数据存储地址
    sI2C_SendByte(Address);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_InvalidCommand;
 
    // 再次发送开始信号
    sI2C_Start();
     
    // 设备读访问地址 0xA1
    sI2C_SendByte(0xA1);
     
    //等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_InvalidCommand;

    // 接收数据
    *Data = sI2C_RecvByte();     
    // 发送非应答，并终止读取
    sI2C_NAck();
    sI2C_Stop();

    return AT24C02_Success;
}


AT24C02_StatusType AT24C02_Read(uint8_t Address, uint8_t* Buffer, uint8_t Size) {

    uint8_t ack = 0;
    uint8_t *p = Buffer;
    
    // 开始信号
    sI2C_Start();
    // 设备写访问地址 0xA0
    sI2C_SendByte(0xA0);
     
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_DeviceNotFound;
     
    // 要访问的数据存储地址
    sI2C_SendByte(Address);
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_InvalidCommand;
 
    // 再次发送开始信号
    sI2C_Start();
     
    // 设备读访问地址 0xA1
    sI2C_SendByte(0xA1);
     
    // 等待应答
    ack = sI2C_WaitAck();
    if (ack == 1)
        return AT24C02_ExecutionFailed;
    // 连续接收数据
    Size = Size - 1;
    while (Size--) {
        *p++ = sI2C_RecvByte();
        // 发送应答
        sI2C_Ack();
    }
     
    // 接收最后一个字节
    *p = sI2C_RecvByte();
     
    // 发送不应答
    sI2C_NAck();
    sI2C_Stop();
     
    return AT24C02_Success;  
}

int main(void) {

    USART1_Config(115200);
    sI2C_Config();
    ADC_SimpleConfig();

    AT24C02_StatusType Status;

    int random_values[2];
    size_t size = sizeof(random_values);
    uint8_t random_bytes[size];

    // static_assert(sizeof(random_values) <= 8, "写入的数据不能超过一页");

    #define StoredAddress 3

    // 上电后先读取前人留下的数值
    Status = AT24C02_Read(StoredAddress, random_bytes, size);
    if (Status != AT24C02_Success)
        printf("Error reading AT24C02 data (code: %d)\n", Status);
    else {
        puts("前一次存入的数值为：");
        for (size_t i = 0; i < size; i++)
            printf("%02x ", random_bytes[i]);
        putchar('\n');
    }

    // 然后存入新的值
    srand(ADC_ReadOne(ADC_Channel_TempSensor));  /* 针对随机数不够随机的方法：
                                                  * 读温度传感器的模拟量作为种子
                                                  * 一般 ADC 的最后几位就很随机 */
    random_values[0] = rand();
    random_values[1] = rand();
    memcpy(random_bytes, random_values, size);
    Status = AT24C02_Write(StoredAddress, random_bytes, size);
    if (Status != AT24C02_Success)
        printf("Error writing AT24C02 data (code: %d)\n", Status);
    else
        printf("本次生成的随机数：%x %x\n", random_values[0], random_values[1]);


    loop;
}

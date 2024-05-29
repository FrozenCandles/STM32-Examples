/**
 * @brief 通过硬件控制 I2C 读写 AT24C02
 *
 * @note
 * 使用两台设备，一台设备载入上一节的程序，一台设备载入本节程序，
 * 上一节的程序仍可正常运行
 *
*/

#include "main.h"
#include "bsp/usart.h"
#include "bsp/adc.h"

#define AT24C02_Address 0xA0
#define AT24C02_PageSize 8

typedef enum {
    AT24C02_Success,
    AT24C02_PeripheralError,
    AT24C02_DeviceNotFound,
    AT24C02_InvalidCommand,
    AT24C02_ExecutionFailed,
} AT24C02_StatusType;

static volatile uint16_t Timeout;

static void I2C_Config(void) {

    // 1. 配置 GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7,
        .GPIO_Mode = GPIO_Mode_AF_OD
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    // 2. 配置 I2C
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_InitTypeDef I2C_InitStructure = {
        .I2C_Mode = I2C_Mode_I2C,
        .I2C_ClockSpeed = 400000,          // 通信速率为快速模式
        .I2C_DutyCycle = I2C_DutyCycle_2,  // 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比
        .I2C_OwnAddress1 = 0x00,           // 这个地址只要与STM32外挂的I2C器件地址不一样即可
        .I2C_Ack = I2C_Ack_Enable,
        .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,  // I2C 的寻址模式
    };
    I2C_Init(I2C1, &I2C_InitStructure);

	// 3. 使能 I2C
    I2C_Cmd(I2C1, ENABLE);
}

AT24C02_StatusType AT24C02_WriteByte(uint8_t Address, uint8_t Data) {

    I2C_GenerateSTART(I2C1, ENABLE);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--Timeout == 0)
            return AT24C02_PeripheralError;
    }

    I2C_Send7bitAddress(I2C1, AT24C02_Address, I2C_Direction_Transmitter);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (--Timeout == 0)
            return AT24C02_DeviceNotFound;
    }

    I2C_SendData(I2C1, Address);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (--Timeout == 0)
            return AT24C02_InvalidCommand;
    }

    I2C_SendData(I2C1, Data);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (--Timeout == 0)
            return AT24C02_ExecutionFailed;
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
    return AT24C02_Success;
}

/**
 * @brief 页写方式，写入的数据不应该超过页大小（8 bytes）
*/
AT24C02_StatusType AT24C02_WritePage(uint8_t Address, uint8_t* Data, uint8_t Size) {

    Timeout = 0xF000;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
        if (--Timeout == 0)
            return AT24C02_PeripheralError;
    }

    I2C_GenerateSTART(I2C1, ENABLE);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--Timeout == 0)
            return AT24C02_PeripheralError;
    }

    I2C_Send7bitAddress(I2C1, AT24C02_Address, I2C_Direction_Transmitter);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (--Timeout == 0)
            return AT24C02_DeviceNotFound;
    }

    I2C_SendData(I2C1, Address);
    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (--Timeout == 0)
            return AT24C02_InvalidCommand;
    }

    while (Size--) {
        I2C_SendData(I2C1, *Data);
        Data++;

        Timeout = 0x1000;
        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            if (--Timeout == 0)
                return AT24C02_ExecutionFailed;
        }
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    return AT24C02_Success;
}

AT24C02_StatusType AT24C02_Read(uint8_t Address, uint8_t* Data, uint8_t Size) {

    Timeout = 0x1000;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
        if (--Timeout == 0)
            return AT24C02_PeripheralError;
    }

    I2C_GenerateSTART(I2C1, ENABLE);

    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--Timeout == 0)
            return AT24C02_PeripheralError;
    }

    I2C_Send7bitAddress(I2C1, AT24C02_Address, I2C_Direction_Transmitter);

    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (--Timeout == 0)
            return AT24C02_DeviceNotFound;
    }

    I2C_Cmd(I2C1, ENABLE);

    I2C_SendData(I2C1, Address);

    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (--Timeout == 0)
            return AT24C02_InvalidCommand;
    }

    I2C_GenerateSTART(I2C1, ENABLE);

    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--Timeout == 0)
            return AT24C02_PeripheralError;
    }

    I2C_Send7bitAddress(I2C1, AT24C02_Address, I2C_Direction_Receiver);

    Timeout = 0x1000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (--Timeout == 0)
            return AT24C02_DeviceNotFound;
    }

    while (Size) {
        if (Size == 1) {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        Timeout = 0x1000;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (--Timeout == 0)
                return AT24C02_InvalidCommand;
        }
        *Data = I2C_ReceiveData(I2C1);
        Data++;
        Size--;
    }
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return AT24C02_Success;
}

void AT24C02_WaitStandBy(void) {
    volatile uint16_t SR1_Tmp = 0;

    do {
        /* Send START condition */
        I2C_GenerateSTART(I2C1, ENABLE);
        /* Read I2C1 SR1 register */
        SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
        /* Send EEPROM address for write */
        I2C_Send7bitAddress(I2C1, AT24C02_Address, I2C_Direction_Transmitter);
    } while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));

    /* Clear AF flag */
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);

    I2C_GenerateSTOP(I2C1, ENABLE);
}

/**
 * @brief 任意写，该函数会分批自动执行页写
*/
AT24C02_StatusType AT24C02_Write(uint8_t Address, uint8_t* Data, uint8_t Size) {
    uint8_t PageNum = Size / AT24C02_PageSize,    // 写入的页数
            SingleNum = Size % AT24C02_PageSize,  // 剩余待写入的字节数
            Addr = Address % AT24C02_PageSize,    // 剩余待写入字节的地址
            Count = AT24C02_PageSize - Addr;      //
    AT24C02_StatusType Status;

    if (Addr == 0) {
        if (PageNum == 0) {
            Status = AT24C02_WritePage(Address, Data, SingleNum);
            if (Status != AT24C02_Success)
                return Status;
            AT24C02_WaitStandBy();
        }
        else {
            while (PageNum--) {
                Status = AT24C02_WritePage(Address, Data, AT24C02_PageSize);
                if (Status != AT24C02_Success)
                    return Status;
                AT24C02_WaitStandBy();
                Address +=  AT24C02_PageSize;
                Data += AT24C02_PageSize;
            }

            if (SingleNum != 0) {
                Status = AT24C02_WritePage(Address, Data, SingleNum);
                if (Status != AT24C02_Success)
                    return Status;
                AT24C02_WaitStandBy();
            }
        }
    }
    else {
        if (PageNum== 0) {
            Status = AT24C02_WritePage(Address, Data, SingleNum);
            if (Status != AT24C02_Success)
                return Status;
            AT24C02_WaitStandBy();
        }
        else {
            Size -= Count;
            PageNum =  Size / AT24C02_PageSize;
            SingleNum = Size % AT24C02_PageSize;	

            if (Count != 0) {
                Status = AT24C02_WritePage(Address, Data, Count);
                if (Status != AT24C02_Success)
                    return Status;
                AT24C02_WaitStandBy();
                Address += Count;
                Data += Count;
            }
            while (PageNum--) {
                Status = AT24C02_WritePage(Address, Data, AT24C02_PageSize);
                if (Status != AT24C02_Success)
                    return Status;
                AT24C02_WaitStandBy();
                Address += AT24C02_PageSize;
                Data += AT24C02_PageSize;
            }
            if(SingleNum != 0) {
                Status = AT24C02_WritePage(Address, Data, SingleNum);
                if (Status != AT24C02_Success)
                    return Status;
                AT24C02_WaitStandBy();
            }
        }
    }
    return Status;
}

int main(void) {

    USART1_Config(115200);
    I2C_Config();
    ADC_SimpleConfig();

    AT24C02_StatusType Status;

    int random_values[8];
    size_t size = sizeof(random_values);
    uint8_t random_bytes[size];

    // static_assert(sizeof(random_values) <= 256, "写入的数据不能超过容量");

    #define StoredAddress 3

    // 上电后先读取前人留下的数据
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
    for (size_t i = 0; i < sizeof(random_values) / sizeof(*random_values); i++)
        random_values[i] = rand();

    memcpy(random_bytes, random_values, size);
    Status = AT24C02_Write(StoredAddress, random_bytes, size);
    if (Status != AT24C02_Success)
        printf("Error writing AT24C02 data (code: %d)\n", Status);
    else {
        puts("本次生成的随机数：");
        for (size_t i = 0; i < sizeof(random_values) / sizeof(*random_values); i++)
            printf("%x ", random_values[i]);
        putchar('\n');
    }

    loop;
}

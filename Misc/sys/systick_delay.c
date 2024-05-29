

#include "systick_delay.h"


void systick_delay_us(unsigned int us) {
    SysTick_Config(72);
    for (int i = 0; i < us; i++) {
        // ��ʱ������ 16 λ�Ƿ��� 1  ���Ϊ 1 ˵��ʱ�䵽���˳�ѭ��
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
    // ��ʱ s ʱ int ������ 72000000 С������ for �г� 1000 Ч��ʱһ����
    for (int i = 0; i < s * 1000; i++) {
        while (!SysTick_CountFlag_Read());
    }
    SysTick_Disable();
}

#if 0

void SysTick_Delay(uint32_t ms) {
    // 1. ��ʼ����ʱ����ֵ��ʹ�䶨ʱ����Ϊ 1/1000 ��
    SysTick->VAL = 0x00;
    SysTick->LOAD = SystemCoreClock / 1000;
    // 2. ��ʼ��ʱ
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    for (uint32_t i = 0; i < ms; i++) {
        uint32_t status;
        // 3. ��ʱ����� COUNTFLAG λ�Ƿ��� 1 ��ͬʱ ENABLE λҲΪ 1�����Ϊ 1 ˵��һ�ֶ�ʱ����
        do {
            status = SysTick->CTRL;
        } while (READ_BIT(status, SysTick_CTRL_ENABLE_Msk) &&
                 !READ_BIT(status, SysTick_CTRL_COUNTFLAG_Msk));
        // �ȴ�ʱ�䵽���CTRL�ĵ�16λ��COUNTFLAG���Ƿ�Ϊ1����STRL�ĵ�0λ�����Ƿ�Ϊ1
    }
    // �رռ����� 
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
} 
#endif

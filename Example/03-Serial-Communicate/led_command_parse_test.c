/**
 * @brief 在 PC 上测试 LED 命令是否可以正常工作
 * 
 * @note
 * 不是 STM32 的代码，使用其它编译器编译
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LED_R ((uint16_t)0)
#define LED_G ((uint16_t)1)
#define LED_B ((uint16_t)5)

bool LED_R_Status = 1;
bool LED_G_Status = 1;
bool LED_B_Status = 1;

bool LED_Status(uint16_t LED_Instance) {
    switch (LED_Instance) {
        case LED_R:
            return LED_R_Status;
        case LED_G:
            return LED_G_Status;
        case LED_B:
            return LED_B_Status;
    }
    return rand() & 1;  // 随便读个值
}

void LED_Turn(uint16_t LED_Instance, bool NewStatus) {
    switch (LED_Instance) {
        case LED_R:
            LED_R_Status = NewStatus;
            break;   
        case LED_G:
            LED_G_Status = NewStatus;
            break;   
        case LED_B:
            LED_B_Status = NewStatus;
            break;   
    }
}



static char* ToUpperCase(char* string) {
    for (char *p = string; *p; ++p) 
        *p = *p>='a' && *p<='z' ? *p & 0xDF : *p;
    return string;
}

static char* ToLowerCase(char* string) {
    for (char *p = string; *p; ++p) 
        *p = *p>='A' && *p<='Z' ? *p | 0x60 : *p;
    return string;
}

char CommandBuffer[50];
void ParseCommand(void);

int main(void) {

    puts("System initialized");

    while (1) {
        fgets(CommandBuffer, 49, stdin);
        ParseCommand();
    }
}

void ParseCommand(void) {

    char LED_Selected;
    char LED_NewStatus[10];
    bool LED_StatusSelected;
    ToLowerCase(CommandBuffer);

    if (sscanf(CommandBuffer, "led-%c-%s", &LED_Selected, LED_NewStatus) == 2) {
        if (strcmp(LED_NewStatus, "on") == 0)
            LED_StatusSelected = 0;
        else if (strcmp(LED_NewStatus, "off") == 0)
            LED_StatusSelected = 1;
        else {
            puts("Error: Invalid status");
            return;
        }

        switch (LED_Selected) {
            case 'r':
                if (LED_Status(LED_R) == LED_StatusSelected)
                    printf("LED R is %s\n", LED_NewStatus);
                else {
                    LED_Turn(LED_R, LED_StatusSelected);
                    printf("LED R turned %s\n", LED_NewStatus);
                }
                return;
            case 'g':
                if (LED_Status(LED_G) == LED_StatusSelected)
                    printf("LED G is %s\n", LED_NewStatus);
                else {
                    LED_Turn(LED_G, LED_StatusSelected);
                    printf("LED G turned %s\n", LED_NewStatus);
                }
                return;
            case 'b':
                if (LED_Status(LED_B) == LED_StatusSelected)
                    printf("LED B is %s\n", LED_NewStatus);
                else {
                    LED_Turn(LED_B, LED_StatusSelected);
                    printf("LED B turned %s\n", LED_NewStatus);
                }
                return;
            default:
                puts("Error: no such LED");
                return;
        }
    }
    else {
        puts("Error: Invalid command");
    }
}

/* 
$ gcc led_command_parse_test.c -o led_command_parse_test
$ ./led_command_parse_test
EXPECTED: System initialized
ENTER: led-r-on
EXPECTED: LED R turned on
ENTER: led-r-on
EXPECTED: LED R is on
ENTER: led-r-off
EXPECTED: LED R turned off
ENTER: led-r-off
EXPECTED: LED R is off
ENTER: led-g-off
EXPECTED: LED G is off
ENTER: led-x-off
EXPECTED: Error: no such LED
ENTER: led-g-ok
EXPECTED: Error: Invalid status
ENTER: led r on
EXPECTED: Error: Invalid command
*/

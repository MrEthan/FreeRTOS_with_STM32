#include <stdio.h>
#include "stm32f10x.h"
#include "LED.h"
#include "usart.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

static TaskHandle_t task_handle[10];

extern char recv_buf[644];
extern int index;
/**********************************************/
/*                                            */
/*            LED周期性闪烁任务1              */
/*                                            */
/**********************************************/
void LedTask1(void *data)
{
    while(1){
        led_toggle_0();
        vTaskDelay(1);
        if (index > 0){
          index--;
          printf("%c", recv_buf[index]);
        }
    }
}

/**********************************************/
/*                                            */
/*            LED周期性闪烁任务2              */
/*                                            */
/**********************************************/
void LedTask2(void *data)
{
    while(1){
        led_toggle_1();
        printf("LedTask2 run\r\n");
        vTaskDelay(1000);
    }
}

int main(void)
{ 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);             // 一定要将优先级分组设置分组方式4！！！
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);   // 不使用JTAG调试，对应的IO口作为普通IO口使用

    LED_Init();
    USART1_Init(115200);
    USART3_Init(115200);

    /* 创建LED1任务 */
    xTaskCreate(LedTask1,      // 任务函数指针
                "LED_TASK1",   // 任务名称
                128,            // 堆栈深度(字)
                NULL,          // 任务参数为空
                1,             // 任务优先级
                &task_handle[0]);        // 任务句柄

    /* 创建LED2任务 */
    xTaskCreate(LedTask2, "LED_TASK2", 128, NULL, 2, NULL );

    /* 启动任务调度器(操作系统开始运行) */
    vTaskStartScheduler();

    /* 不应该运行到这里 */
    while(1)
    {}
}

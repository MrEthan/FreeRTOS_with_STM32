#include <stdio.h>
#include "stm32f10x.h"
#include "LED.h"
#include "usart.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
/* private header */
#include "my_portable.h"
#include "base.h"
#include "debug.h"
#include "cmd.h"
#include "my_portable.h"
#include "lstLib.h"

static TaskHandle_t task_handle[10];

void LedTask1(void *data)
{
    BaseType_t ret;
    uint32_t notify_val = 0;

    while(1){
        /* 等待通知，任务进入阻塞状态 */
        //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ret = xTaskNotifyWait(UINT32_MAX, UINT32_MAX, &notify_val, portMAX_DELAY);
        if (pdTRUE == ret){
            led_toggle_0();
        }
    }
}

void LedTask2(void *data)
{
    while(1){
        led_toggle_1();
        /* 通知task1 */
        //xTaskNotifyGive(task_handle[0]);
        xTaskNotify(task_handle[0], 2, eSetBits);
        vTaskDelay(1000);
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);             // 一定要将优先级分组设置分组方式4！！！
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);   // 不使用JTAG调试，对应的IO口作为普通IO口使用

    LED_Init();
    //USART1_Init(115200);
    USART3_Init(115200);
    
    /* 创建LED1任务 */
    xTaskCreate(LedTask1,      // 任务函数指针
                "LED_TASK1",   // 任务名称
                1024,            // 堆栈深度(字)
                NULL,          // 任务参数为空
                NORMAL_TASK_PRIORITY,             // 任务优先级
                &task_handle[0]);        // 任务句柄

    /* 创建LED2任务 */
    xTaskCreate(LedTask2, "LED_TASK2", 1024, NULL, NORMAL_TASK_PRIORITY, &task_handle[1]);

    /* 创建cmd任务 */
    cmd_create_task();

    /* 启动任务调度器(操作系统开始运行) */
    vTaskStartScheduler();
    
    /* 不应该运行到这里 */
    while(1)
    {}
}

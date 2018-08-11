#include <stdio.h>
#include "stm32f10x.h"
#include "LED.h"
#include "usart.h"
/* FreeRTOSͷ�ļ� */
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
        /* �ȴ�֪ͨ�������������״̬ */
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
        /* ֪ͨtask1 */
        //xTaskNotifyGive(task_handle[0]);
        xTaskNotify(task_handle[0], 2, eSetBits);
        vTaskDelay(1000);
    }
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);             // һ��Ҫ�����ȼ��������÷��鷽ʽ4������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);   // ��ʹ��JTAG���ԣ���Ӧ��IO����Ϊ��ͨIO��ʹ��

    LED_Init();
    //USART1_Init(115200);
    USART3_Init(115200);
    
    /* ����LED1���� */
    xTaskCreate(LedTask1,      // ������ָ��
                "LED_TASK1",   // ��������
                1024,            // ��ջ���(��)
                NULL,          // �������Ϊ��
                NORMAL_TASK_PRIORITY,             // �������ȼ�
                &task_handle[0]);        // ������

    /* ����LED2���� */
    xTaskCreate(LedTask2, "LED_TASK2", 1024, NULL, NORMAL_TASK_PRIORITY, &task_handle[1]);

    /* ����cmd���� */
    cmd_create_task();

    /* �������������(����ϵͳ��ʼ����) */
    vTaskStartScheduler();
    
    /* ��Ӧ�����е����� */
    while(1)
    {}
}

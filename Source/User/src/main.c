#include <stdio.h>
#include "stm32f10x.h"
#include "LED.h"
#include "usart.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"

#define LED_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

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
            DDEBUG("LedTask1 wakeup. notify_val:%d \r\n", notify_val);
            led_toggle_0();
        }
    }
}

void LedTask2(void *data)
{
    while(1){
        led_toggle_1();
        DDEBUG("LedTask2 run\r\n");
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
    USART1_Init(115200);
    USART3_Init(115200);

    /* ����LED1���� */
    xTaskCreate(LedTask1,      // ������ָ��
                "LED_TASK1",   // ��������
                1024,            // ��ջ���(��)
                NULL,          // �������Ϊ��
                LED_TASK_PRIORITY,             // �������ȼ�
                &task_handle[0]);        // ������

    /* ����LED2���� */
    xTaskCreate(LedTask2, "LED_TASK2", 1024, NULL, LED_TASK_PRIORITY, &task_handle[1]);

    /* �������������(����ϵͳ��ʼ����) */
    vTaskStartScheduler();

    /* ��Ӧ�����е����� */
    while(1)
    {}
}

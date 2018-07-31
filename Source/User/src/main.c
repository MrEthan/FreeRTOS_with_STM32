#include <stdio.h>
#include "stm32f10x.h"
#include "LED.h"
#include "usart.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

static TaskHandle_t task_handle[10];

extern char recv_buf[644];
extern int index;
/**********************************************/
/*                                            */
/*            LED��������˸����1              */
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
/*            LED��������˸����2              */
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);             // һ��Ҫ�����ȼ��������÷��鷽ʽ4������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);   // ��ʹ��JTAG���ԣ���Ӧ��IO����Ϊ��ͨIO��ʹ��

    LED_Init();
    USART1_Init(115200);
    USART3_Init(115200);

    /* ����LED1���� */
    xTaskCreate(LedTask1,      // ������ָ��
                "LED_TASK1",   // ��������
                128,            // ��ջ���(��)
                NULL,          // �������Ϊ��
                1,             // �������ȼ�
                &task_handle[0]);        // ������

    /* ����LED2���� */
    xTaskCreate(LedTask2, "LED_TASK2", 128, NULL, 2, NULL );

    /* �������������(����ϵͳ��ʼ����) */
    vTaskStartScheduler();

    /* ��Ӧ�����е����� */
    while(1)
    {}
}

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "rtos_task_ctl.h"
#include "my_portable.h"
#include "debug.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("stackoverflow %s\r\n", pcTaskName);
    return;
}

void get_rtos_sys_state(void)
{
    int ArraySize = 0;
    TaskStatus_t *StatusArray = NULL;
    int x = 0;

    ArraySize = uxTaskGetNumberOfTasks();//��ȡϵͳ��������
    StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t));//�����ڴ�

    if(StatusArray != NULL){
        ArraySize = uxTaskGetSystemState((TaskStatus_t* )StatusArray, (UBaseType_t)ArraySize, NULL);
        printf("TaskName\t\tPriority\t\tTaskNumber\t\tHighWater\r\n");
        for(x=0; x < ArraySize; x++){
            //ͨ�����ڴ�ӡ����ȡ����ϵͳ���й���Ϣ ������������,
            //�������ȼ���������
            printf("%s\t\t%d\t\t\t%d\t\t\t%d\r\n",
                    StatusArray[x].pcTaskName,
                    (int)StatusArray[x].uxCurrentPriority,
                    (int)StatusArray[x].xTaskNumber,
                    (int)StatusArray[x].usStackHighWaterMark);
        }
        vPortFree(StatusArray);  //�ͷ��ڴ�
    }
    return;
}





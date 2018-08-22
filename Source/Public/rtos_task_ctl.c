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

    ArraySize = uxTaskGetNumberOfTasks();//获取系统任务数量
    StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t));//申请内存

    if(StatusArray != NULL){
        ArraySize = uxTaskGetSystemState((TaskStatus_t* )StatusArray, (UBaseType_t)ArraySize, NULL);
        printf("TaskName\t\tPriority\t\tTaskNumber\t\tHighWater\r\n");
        for(x=0; x < ArraySize; x++){
            //通过串口打印出获取到的系统的有关信息 比如任务名称,
            //任务优先级和任务编号
            printf("%s\t\t%d\t\t\t%d\t\t\t%d\r\n",
                    StatusArray[x].pcTaskName,
                    (int)StatusArray[x].uxCurrentPriority,
                    (int)StatusArray[x].xTaskNumber,
                    (int)StatusArray[x].usStackHighWaterMark);
        }
        vPortFree(StatusArray);  //释放内存
    }
    return;
}





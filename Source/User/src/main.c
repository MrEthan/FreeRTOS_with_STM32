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
static HeapRegion_t heap_region[] = {{(uint8_t *)0x2000B000, 0x5000}, {NULL, 0}};

/* test */
typedef struct{
    NODE node;
    int num;
}TEST_LSTLIB_NODE;

void lstLib_test(void)
{
    int i = 0, count = 0, free_heap_size;
    LIST list;
    TEST_LSTLIB_NODE *p_node = NULL;
    TEST_LSTLIB_NODE *p_lst_node = NULL;
    TEST_LSTLIB_NODE *p_lst_node_tmp = NULL;

    lstInit(&list);

    for (i = 0; i < 10000; i++){
        free_heap_size = xPortGetFreeHeapSize();
        DDEBUG("free_heap_size:%d\r\n", free_heap_size);
        if (free_heap_size < sizeof(TEST_LSTLIB_NODE)){
            DDEBUG("%d\r\n", i);
            return;
        }
        p_node = (TEST_LSTLIB_NODE *)MALLOC(sizeof(TEST_LSTLIB_NODE));
        if(NULL == p_node){
            DERROR("malloc err, i:%d\r\n", i);
            return;
        }
        p_node->num = i;
        lstAddTail(&list, &p_node->node);
    }

    printf("LIST_FOR_EACH count:%d\r\n", lstGetCount(&list));
    LIST_FOR_EACH(TEST_LSTLIB_NODE, p_lst_node, &list){
        printf("%d ", p_lst_node->num);
    }
    printf("\r\n");

    printf("del:");
    LIST_FOR_EACH_SAFE(TEST_LSTLIB_NODE, p_lst_node, p_lst_node_tmp, &list){
        if (p_lst_node->num % 2 == 0){
            printf("%d ", p_lst_node->num);

            p_node = (TEST_LSTLIB_NODE *)MALLOC(sizeof(TEST_LSTLIB_NODE));
            p_node->num = p_lst_node->num + 10;
            lstInsert(&list, (NODE *)p_lst_node, &p_node->node);

            lstDelete(&list, (NODE *)p_lst_node);

            SAFE_FREE(p_lst_node);
        }
    }
    printf("\r\n");

    printf("LIST_FOR_EACH after delete:%d\r\n", lstGetCount(&list));
    LIST_FOR_EACH(TEST_LSTLIB_NODE, p_lst_node, &list){
        printf("%d ", p_lst_node->num);
    }
    printf("\r\n");

    printf("test lstGet\r\n");
    count = lstGetCount(&list);
    for (i = 1; i <= count; i++){
        p_node = (TEST_LSTLIB_NODE *)lstGet(&list, i);
        printf("%d ", p_node->num);
    }
    return;
}

void LedTask1(void *data)
{
    BaseType_t ret;
    uint32_t notify_val = 0;

    lstLib_test();

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

    /* 初始化堆内存 */
    vPortDefineHeapRegions(heap_region);

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

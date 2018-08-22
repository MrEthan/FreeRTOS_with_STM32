#ifndef MY_PORTABLE_H
#define MY_PORTABLE_H

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

#define MALLOC(x)  pvPortMalloc(x)
#define FREE(x)    vPortFree(x)

#define NORMAL_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#endif

#ifndef MY_PORTABLE_H
#define MY_PORTABLE_H

#include "FreeRTOS.h"

#define MALLOC(x)  malloc(x)
#define FREE(x)    free(x)

#define NORMAL_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#endif

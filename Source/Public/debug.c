#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "debug.h"

#define MAX_LEN_ONE_DEBUG_PRINT   256

#define SPIDER_LEVEL_DEBUG 0
#define SPIDER_LEVEL_INFO  1
#define SPIDER_LEVEL_WARN  2
#define SPIDER_LEVEL_ERROR 3
#define SPIDER_LEVEL_CRIT  4

#define DEBUG_LEVEL 0

static const char * LOG_STR[] = {
    "ERROR",
    "WARN",
    "DEBUG",
    "INFO",
};

void debug_print(int level, const char *func_name, const char *format, ...)
{
    char debug_buf[MAX_LEN_ONE_DEBUG_PRINT];
    //char time_str[32];
    va_list ap;
    int debug_head_len = 0;

    //time_t now = time(NULL);

    va_start(ap, format);
    //strftime(time_str, sizeof(time_str), "%y%m%d-%H:%M:%S", localtime(&now));
    snprintf(debug_buf, MAX_LEN_ONE_DEBUG_PRINT, "[%s][%s]", LOG_STR[level], func_name);
    debug_head_len = strlen(debug_buf);
    vsnprintf(debug_buf + debug_head_len, MAX_LEN_ONE_DEBUG_PRINT - debug_head_len, format, ap);
    va_end(ap);

    printf("%s", debug_buf);

    return;
}

void dumpBuffer(const char *describe, unsigned char *buf, int len)
{
    int i = 0, count = 0;

    if (!buf || 0 > len){
        printf("dumpBuffer, para err. buf:%p, len:%d\r\n", buf, len);
        return;
    }
    if (describe){
        printf("dump => %s, len:%d\r\n", describe, len);
    }else{
        printf("dump => len:%d\r\n", len);
    }

    for (i = 0; i < len; i++){
        if (16 == count){
            count = 0;
            printf("\r\n");
        }
        printf("%02x ", buf[i]);
        count++;
    }
    printf("\r\n\r\n");
    return;
}

void dumpBuffer_char(const char *describe, char *buf, int len)
{
    int i = 0, count = 0;

    if (!buf || 0 > len){
        printf("dumpBuffer, para err. buf:%p, len:%d\r\n", buf, len);
        return;
    }
    if (describe){
        printf("dump => %s, len:%d\r\n", describe, len);
    }else{
        printf("dump => len:%d\r\n", len);
    }

    for (i = 0; i < len; i++){
        if (16 == count){
            count = 0;
            printf("\r\n");
        }
        printf("%c", buf[i]);
        count++;
    }
    printf("\r\n\r\n");
    return;
}

void debug_assert_failed(uint8_t* file, uint32_t line)
{
    printf("assert faile, file:%s, line:%d\r\n", file, line);
    while(1){
        ;
    }
}



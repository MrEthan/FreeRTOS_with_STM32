#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "my_portable.h"
#include "fifo.h"

/* 数组实现缓冲区，新增数据写到头，从尾巴开始读取，即先入先出 */

#define FIFO_MAX 512

void fifo_in(FIFO *p_fifo, unsigned char ch)
{
    if (p_fifo->head < p_fifo->len){
        p_fifo->buf[p_fifo->head++] = ch;
    }else{
        p_fifo->head = 0;
        p_fifo->buf[p_fifo->head++] = ch;
    }

    if (p_fifo->count < p_fifo->len){
        p_fifo->count++;
    }
    return;
}

void fifo_out(FIFO *p_fifo, unsigned char *out)
{
    if (p_fifo->count > 0){
        if (p_fifo->tail == p_fifo->len){
            p_fifo->tail = 0;
        }
        *out = p_fifo->buf[p_fifo->tail++];
        p_fifo->count--;
    }
    return;
}

int fifo_write(FIFO *p_fifo, unsigned char *buf, int len)
{
    int i = 0;

    if (!p_fifo || !buf || len > FIFO_MAX){
        DERROR("para err.\r\n");
        return -1;
    }

    for (i = 0; i < len; i++){
        fifo_in(p_fifo, buf[i]);
    }
    return 0;
}

int fifo_read(FIFO *p_fifo, unsigned char *buf, int len)
{
    int len_read = 0;
    int i = 0;

    if (!p_fifo || !buf || len > FIFO_MAX){
        DERROR("para err.\r\n");
        return -1;
    }

    if (len <= p_fifo->count){
        len_read = len;
    }else{
        len_read = p_fifo->count;
    }

    for (i = 0; i < len_read; i++){
        fifo_out(p_fifo, &buf[i]);
    }

    return len_read;
}

FIFO *fifo_create(int len)
{
    char *p_buf = NULL;
    FIFO *p_fifo = NULL;

    ASSERT(len < FIFO_MAX);

    p_fifo = MALLOC(sizeof(FIFO));
    p_buf = MALLOC(len);
    if (!p_fifo || !p_buf){
        DDEBUG("malloc err.\r\n");
        return NULL;
    }
    memset(p_fifo, 0, sizeof(FIFO));
    memset(p_buf, 0, len);
    p_fifo->buf = p_buf;
    p_fifo->len = len;
    return p_fifo;
}

void fifo_destroy(FIFO *p_fifo)
{
    if (NULL != p_fifo){
        free(p_fifo);
    }
    return;
}

#if 0
/* test */
void fifo_test(void)
{
    FIFO *p_fifo = NULL;
    unsigned char buf[100] = {1,2,3,4,5};
    unsigned char buf_out[100] = {0};
    int num = 0;
    int i = 0;

    p_fifo = fifo_create(10);

    fifo_write(p_fifo, buf, 5);
    num = fifo_read(p_fifo, buf_out, sizeof(buf_out));
    printf("num:%d\r\n", num);
    dumpBuffer(__FUNCTION__, buf_out, num);

    for(i = 0; i < 10; i++){
        buf[i] = i + 10;
    }
    fifo_write(p_fifo, buf, 10);
    num = fifo_read(p_fifo, buf_out, sizeof(buf_out));
    printf("num:%d\r\n", num);
    dumpBuffer(__FUNCTION__, buf_out, num);

    for(i = 0; i < 15; i++){
        buf[i] = i + 100;
    }
    fifo_write(p_fifo, buf, 10);
    num = fifo_read(p_fifo, buf_out, sizeof(buf_out));
    printf("num:%d\r\n", num);
    dumpBuffer(__FUNCTION__, buf_out, num);

    return;
}
#endif


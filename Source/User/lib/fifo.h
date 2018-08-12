#ifndef __FIFO_H
#define __FIFO_H

typedef struct{
    int head;
    int tail;
    int count; //fifo中有效数据个数
    int len;   //fifo缓冲区长度
    char *buf;
}FIFO;

FIFO *fifo_create(int len);
void fifo_destroy(FIFO *p_fifo);
int fifo_write(FIFO *p_fifo, unsigned char *buf, int len);
int fifo_read(FIFO *p_fifo, unsigned char *buf, int len);

#endif

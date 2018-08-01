#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <string.h>
#include <errno.h>
#include "debug.h"

#define SAFE_FREE(p)  \
do{                   \
    if (NULL != (p)){ \
        free(p);      \
        p = NULL;     \
    }                 \
}while(0)


#define SAFE_CLOSE(fd) \
do{                    \
    if(fd > 0){        \
        close(fd);      \
        fd = -1;        \
    }                   \
}while(0)

#ifndef container_of
#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#endif //BASE_H

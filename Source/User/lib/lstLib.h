#ifndef __LSTLIB_H
#define __LSTLIB_H

typedef struct NODE{
    struct NODE *next;
    struct NODE *prev;
}NODE;

typedef struct {
    NODE *next;
    NODE *prev;
    int   count;
}LIST;

#define LIST_FOR_EACH(type, var, list) \
    for(var = (type *)lstFirst(list); \
        var != NULL; \
        var = (type *)lstNext(list, (NODE *)var))

#define LIST_FOR_EACH_SAFE(type, var, tmp, list) \
    for(var = (type *)lstFirst(list), tmp = (type *)lstNext(list, (NODE *)var);\
        var != NULL; \
        var = tmp, tmp = (type *)lstNext(list, (NODE *)var))

void lstInit(LIST *p_list);
void lstNodeInit(NODE *p_node);
void lstAddHead(LIST *p_list, NODE *p_node);
void lstAddTail(LIST *p_list, NODE *p_node);
void lstInsert(LIST *p_list, NODE *p_prev, NODE *p_node);
void lstDelete(LIST *p_list, NODE *p_node);
NODE *lstFirst(LIST *p_list);
NODE *lstNext(LIST *p_list, NODE *p_node);
NODE *lstGet(LIST *p_list, int index);
int lstGetCount(LIST *p_list);
int lstEnpty(LIST *p_list);

#endif

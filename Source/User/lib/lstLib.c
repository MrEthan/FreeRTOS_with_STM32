#include <stdio.h>
#include "debug.h"
#include "base.h"
#include "my_portable.h"
#include "lstLib.h"

#define HEAD next
#define TAIL prev

void lstInit(LIST *p_list)
{
    p_list->next = NULL;
    p_list->prev = NULL;
    p_list->count = 0;
    return;
}

void lstNodeInit(NODE *p_node)
{
    p_node->next = NULL;
    p_node->prev = NULL;
    return;
}

/* 新节点插到表头，即使插到head后面 */
void lstAddHead(LIST *p_list, NODE *p_node)
{
    p_node->prev = NULL;
    p_node->next = p_list->next;

    if (p_list->next == NULL){
        //原链表为空，表尾指向新节点
        p_list->prev = p_node;
    }else{
        //表尾不变, 原第一个节点的prev指向新插入节点
        p_list->next->prev = p_node;
    }
    p_list->next = p_node; //表头指向新节点

    p_list->count++;
    return;
}

/* 新节点插到表尾 */
void lstAddTail(LIST *p_list, NODE *p_node)
{
    p_node->next = NULL;
    p_node->prev = p_list->prev;

    if (p_list->next == NULL){
        //原链表为空，表头指向新节点
        p_list->next = p_node;
    }else{
        //表头不变,原最后一个节点的next指向新插入节点
        p_list->prev->next = p_node;
    }
    p_list->prev = p_node; //表尾指向新节点

    p_list->count++;
    return;
}

/* 新节点插到节点p_prev后面 */
void lstInsert(LIST *p_list, NODE *p_prev, NODE *p_node)
{
    if (p_prev == p_list->next){
        //新节点插到表头
        lstAddHead(p_list, p_node);
    }else if(p_prev == p_list->prev){
        //新节点插到表尾
        lstAddTail(p_list, p_node);
    }else{
        //新节点插到中间
        p_node->next = p_prev->next;
        p_node->prev = p_prev;
        p_prev->next->prev = p_node;
        p_prev->next = p_node;
        p_list->count++;
    }
    return;
}

/* 从链表上摘除节点 */
void lstDelete(LIST *p_list, NODE *p_node)
{
    if (p_node == p_list->next){
        //摘除head后面的节点，链表头变化
        p_list->next = p_node->next;
        if (p_node->next != NULL){
            //后一个节点成为head后的第一个节点
            p_node->next->prev = NULL;
        }else{
            //如果删除节点为尾节点，则删除后，链表为空
            p_list->prev = NULL;
        }
    }else if(p_node == p_list->prev){
        //删除尾节点，链表尾变化
        p_list->prev = p_node->prev;
        if (p_node->prev != NULL){
            //前一个节点成为尾节点
            p_node->prev->next = NULL;
        }else{
            //如果删除节点为head后的第一个节点，则删除后，链表为空
            p_list->next = NULL;
        }
    }else{
        //删除中间节点，表头表尾都不变化
        p_node->prev->next = p_node->next;
        p_node->next->prev = p_node->prev;
    }
    p_list->count--;
    return;
}

NODE *lstFirst(LIST *p_list)
{
    return p_list->next;
}

NODE *lstNext(LIST *p_list, NODE *p_node)
{
    return p_node->next;
}

int lstGetCount(LIST *p_list)
{
    return p_list->count;
}

/* index从1开始*/
NODE *lstGet(LIST *p_list, int index)
{
    int i = 1;
    NODE *p_node = NULL;

    for (p_node = lstFirst(p_list); p_node != NULL; p_node = lstNext(p_list, p_node), i++){
        if (i == index){
            return p_node;
        }
    }
    return NULL;
}

int lstEnpty(LIST *p_list)
{
    return (p_list->HEAD == NULL);
}

/* test */
//typedef struct{
//    NODE node;
//    int num;
//}TEST_LSTLIB_NODE;

//void lstLib_test(void)
//{
//    int i = 0, count = 0, free_heap_size;
//    LIST list;
//    TEST_LSTLIB_NODE *p_node = NULL;
//    TEST_LSTLIB_NODE *p_lst_node = NULL;
//    TEST_LSTLIB_NODE *p_lst_node_tmp = NULL;

//    lstInit(&list);

//    for (i = 0; i < 50; i++){
//        free_heap_size = xPortGetFreeHeapSize();
//        DDEBUG("free_heap_size:%d\r\n", free_heap_size);
//        if (free_heap_size < sizeof(TEST_LSTLIB_NODE)){
//            DDEBUG("%d\r\n", i);
//            return;
//        }
//        p_node = (TEST_LSTLIB_NODE *)MALLOC(sizeof(TEST_LSTLIB_NODE));
//        if(NULL == p_node){
//            DERROR("malloc err, i:%d\r\n", i);
//            return;
//        }
//        p_node->num = i;
//        lstAddTail(&list, &p_node->node);
//    }

//    printf("LIST_FOR_EACH count:%d\r\n", lstGetCount(&list));
//    LIST_FOR_EACH(TEST_LSTLIB_NODE, p_lst_node, &list){
//        printf("%d ", p_lst_node->num);
//    }
//    printf("\r\n");

//    printf("del:");
//    LIST_FOR_EACH_SAFE(TEST_LSTLIB_NODE, p_lst_node, p_lst_node_tmp, &list){
//        if (p_lst_node->num % 2 == 0){
//            printf("%d ", p_lst_node->num);

//            p_node = (TEST_LSTLIB_NODE *)MALLOC(sizeof(TEST_LSTLIB_NODE));
//            p_node->num = p_lst_node->num + 10;
//            lstInsert(&list, (NODE *)p_lst_node, &p_node->node);

//            lstDelete(&list, (NODE *)p_lst_node);

//            SAFE_FREE(p_lst_node);
//        }
//    }
//    printf("\r\n");

//    printf("LIST_FOR_EACH after delete:%d\r\n", lstGetCount(&list));
//    LIST_FOR_EACH(TEST_LSTLIB_NODE, p_lst_node, &list){
//        printf("%d ", p_lst_node->num);
//    }
//    printf("\r\n");

//    printf("test lstGet\r\n");
//    count = lstGetCount(&list);
//    for (i = 1; i <= count; i++){
//        p_node = (TEST_LSTLIB_NODE *)lstGet(&list, i);
//        printf("%d ", p_node->num);
//    }
//    return;
//}



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

/* �½ڵ�嵽��ͷ����ʹ�嵽head���� */
void lstAddHead(LIST *p_list, NODE *p_node)
{
    p_node->prev = NULL;
    p_node->next = p_list->next;

    if (p_list->next == NULL){
        //ԭ����Ϊ�գ���βָ���½ڵ�
        p_list->prev = p_node;
    }else{
        //��β����, ԭ��һ���ڵ��prevָ���²���ڵ�
        p_list->next->prev = p_node;
    }
    p_list->next = p_node; //��ͷָ���½ڵ�

    p_list->count++;
    return;
}

/* �½ڵ�嵽��β */
void lstAddTail(LIST *p_list, NODE *p_node)
{
    p_node->next = NULL;
    p_node->prev = p_list->prev;

    if (p_list->next == NULL){
        //ԭ����Ϊ�գ���ͷָ���½ڵ�
        p_list->next = p_node;
    }else{
        //��ͷ����,ԭ���һ���ڵ��nextָ���²���ڵ�
        p_list->prev->next = p_node;
    }
    p_list->prev = p_node; //��βָ���½ڵ�

    p_list->count++;
    return;
}

/* �½ڵ�嵽�ڵ�p_prev���� */
void lstInsert(LIST *p_list, NODE *p_prev, NODE *p_node)
{
    if (p_prev == p_list->next){
        //�½ڵ�嵽��ͷ
        lstAddHead(p_list, p_node);
    }else if(p_prev == p_list->prev){
        //�½ڵ�嵽��β
        lstAddTail(p_list, p_node);
    }else{
        //�½ڵ�嵽�м�
        p_node->next = p_prev->next;
        p_node->prev = p_prev;
        p_prev->next->prev = p_node;
        p_prev->next = p_node;
        p_list->count++;
    }
    return;
}

/* ��������ժ���ڵ� */
void lstDelete(LIST *p_list, NODE *p_node)
{
    if (p_node == p_list->next){
        //ժ��head����Ľڵ㣬����ͷ�仯
        p_list->next = p_node->next;
        if (p_node->next != NULL){
            //��һ���ڵ��Ϊhead��ĵ�һ���ڵ�
            p_node->next->prev = NULL;
        }else{
            //���ɾ���ڵ�Ϊβ�ڵ㣬��ɾ��������Ϊ��
            p_list->prev = NULL;
        }
    }else if(p_node == p_list->prev){
        //ɾ��β�ڵ㣬����β�仯
        p_list->prev = p_node->prev;
        if (p_node->prev != NULL){
            //ǰһ���ڵ��Ϊβ�ڵ�
            p_node->prev->next = NULL;
        }else{
            //���ɾ���ڵ�Ϊhead��ĵ�һ���ڵ㣬��ɾ��������Ϊ��
            p_list->next = NULL;
        }
    }else{
        //ɾ���м�ڵ㣬��ͷ��β�����仯
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

/* index��1��ʼ*/
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



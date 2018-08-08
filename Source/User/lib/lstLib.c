#include <stdio.h>
#include "debug.h"
#include "my_portable.h"
#include "lstLib.h"

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
        //��β����
    }
    p_list->next = p_node; //��ͷָ���½ڵ�
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
        //��ͷ����
    }
    p_list->prev = p_node; //��βָ���½ڵ�
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



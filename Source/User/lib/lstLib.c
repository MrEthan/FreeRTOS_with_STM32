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

/* 新节点插到表头，即使插到head后面 */
void lstAddHead(LIST *p_list, NODE *p_node)
{
    p_node->prev = NULL;
    p_node->next = p_list->next;

    if (p_list->next == NULL){
        //原链表为空，表尾指向新节点
        p_list->prev = p_node;
    }else{
        //表尾不变
    }
    p_list->next = p_node; //表头指向新节点
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
        //表头不变
    }
    p_list->prev = p_node; //表尾指向新节点
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



#include <stdlib.h>
#include <string.h>
#include <lightship/linked_list.h>

list_t* list_create(void)
{
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list_init_list(list);
    return list;
}

void list_init_list(list_t* list)
{
    memset(list, 0, sizeof(list_t));
}

void list_destroy(list_t* list)
{
    list_clear(list);
    free(list);
}

void list_clear(list_t* list)
{
    list_node_t* current;
    while(current = list->tail)
    {
        list->tail = list->tail->next;
        free(current);
    }
    list->count = 0;
}

void list_push(list_t* list, void* data)
{
    list_node_t* node = (list_node_t*)malloc(sizeof(list_node_t));
    /* first element being inserted, set tail */
    if(!list->head)
        list->tail = node;
    else
        list->head->next = node;/* next node of current node is the new node */

    node->prev = list->head;    /* previous node of new node is the current head */
    list->head = node;          /* new head is new node */
    node->next = NULL;          /* new node has no next node */
    node->data = data;
    ++list->count;
}

void* list_pop(list_t* list)
{
    list_node_t* node = list->head;
    if(!node)
        return NULL;
    
    list->head = node->prev;    /* new head is previous node */
    if(list->head)              /* does the previous node exist? */
        list->head->next = NULL;/* previous node no longer has a next node */
    else                        /* the previous node doesn't exist */
        list->tail = NULL;      /* tail no longer exists */

    free(node);
    --list->count;
}

void* list_erase_node(list_t* list, list_node_t* node)
{
    list_node_t* prev = node->prev;
    list_node_t* next = node->next;
    if(prev)
        prev->next = next;  /* node after current node is the previous' node next node */
    else
        list->tail = next;  /* tail was pointing at current node - point to next */
        
    if(next)
        next->prev = prev;  /* node before current node is the next' node previous node */
    else
        list->head = prev;  /* head was pointing at current noid - point to previous */

    void* data = node->data;
    free(node);
    --list->count;
    return data;
}

void* list_erase_element(list_t* list, void* data)
{
    list_node_t* current = list->tail;
    while(current)
    {
        if(current->data == data)
        {
            return list_erase_node(list, current);
        }
        current = current->next;
    }
    return NULL;
}
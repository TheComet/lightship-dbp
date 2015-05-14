#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util/linked_list.h"
#include "util/memory.h"

/* ------------------------------------------------------------------------- */
struct list_t*
list_create(void)
{
    struct list_t* list = (struct list_t*)MALLOC(sizeof(struct list_t));
    list_init_list(list);
    return list;
}

/* ------------------------------------------------------------------------- */
void
list_init_list(struct list_t* list)
{
    memset(list, 0, sizeof(struct list_t));
}

/* ------------------------------------------------------------------------- */
void
list_destroy(struct list_t* list)
{
    list_clear(list);
    FREE(list);
}

/* ------------------------------------------------------------------------- */
void
list_clear(struct list_t* list)
{
    struct list_node_t* current;
    while((current = list->tail))
    {
        list->tail = list->tail->next;
        FREE(current);
    }
    list->head = NULL;
    list->count = 0;
}

/* ------------------------------------------------------------------------- */
char
list_push(struct list_t* list, void* data)
{
    struct list_node_t* node;
    node = (struct list_node_t*)MALLOC(sizeof(struct list_node_t));
    if(!node)
    {
        fprintf(stderr, "malloc() failed in list_push() -- not enough memory\n");
        return 0;
    }

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

    return 1;
}

/* ------------------------------------------------------------------------- */
void*
list_pop(struct list_t* list)
{
    void* data;

    struct list_node_t* node = list->head;
    if(!node)
        return NULL;

    list->head = node->prev;    /* new head is previous node */
    if(list->head)              /* does the previous node exist? */
        list->head->next = NULL;/* previous node no longer has a next node */
    else                        /* the previous node doesn't exist */
        list->tail = NULL;      /* tail no longer exists */

    data = node->data;
    FREE(node);
    --list->count;

    return data;
}

/* ------------------------------------------------------------------------- */
void*
list_erase_node(struct list_t* list, struct list_node_t* node)
{
    struct list_node_t* prev = node->prev;
    struct list_node_t* next = node->next;
    void* data;
    if(prev)
        prev->next = next;  /* node after current node is the previous' node next node */
    else
        list->tail = next;  /* tail was pointing at current node - point to next */

    if(next)
        next->prev = prev;  /* node before current node is the next' node previous node */
    else
        list->head = prev;  /* head was pointing at current noid - point to previous */

    data = node->data;
    FREE(node);
    --list->count;
    return data;
}

/* ------------------------------------------------------------------------- */
char
list_erase_element(struct list_t* list, void* data)
{
    struct list_node_t* current = list->tail;
    while(current)
    {
        if(current->data == data)
        {
            list_erase_node(list, current);
            return 1;
        }
        current = current->next;
    }
    return 0;
}

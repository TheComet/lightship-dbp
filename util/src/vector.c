#include <string.h>
#include <stdlib.h>
#include "util/vector.h"
#include "util/memory.h"

/*!
 * @brief Expands the underlying memory.
 * 
 * This implementation will expand the memory by a factor of 2 each time this
 * is called. All elements are copied into the new section of memory.
 * @param [in] insertion_index Set to -1 if no space should be made for element
 * insertion. Otherwise this parameter specifies the index of the element to
 * "evade" when re-allocating all other elements.
 */
static void vector_expand(struct vector_t* vector, intptr_t insertion_index);

struct vector_t* vector_create(const intptr_t element_size)
{
    struct vector_t* vector = (struct vector_t*)MALLOC(sizeof(struct vector_t));
    vector_init_vector(vector, element_size);
    return vector;
}

void vector_init_vector(struct vector_t* vector, const intptr_t element_size)
{
    memset(vector, 0, sizeof(struct vector_t));
    vector->element_size = element_size;
}

void vector_destroy(struct vector_t* vector)
{
    vector_clear(vector);
    FREE(vector);
}

void vector_clear(struct vector_t* vector)
{
    /* 
     * No need to free or overwrite existing memory, just reset the counter
     * and let future insertions overwrite
     */
    vector->count = 0;
}

void vector_clear_free(struct vector_t* vector)
{
    if(vector->data)
        FREE(vector->data);
    vector->data = NULL;
    vector->count = 0;
    vector->capacity = 0;
}

void* vector_push_emplace(struct vector_t* vector)
{
    void* data;
    if(vector->count == vector->capacity)
        vector_expand(vector, -1);
    data = vector->data + (vector->element_size * vector->count);
    ++(vector->count);
    return data;
}

void vector_push(struct vector_t* vector, void* data)
{
    memcpy(vector_push_emplace(vector), data, vector->element_size);
}

void* vector_pop(struct vector_t* vector)
{
    if(!vector->count)
        return NULL;

    --(vector->count);
    return vector->data + (vector->element_size * vector->count);
}

void vector_insert(struct vector_t* vector, intptr_t index, void* data)
{
    intptr_t offset = index * vector->element_size;

    /* 
     * Normally the last valid index is (capacity-1), but in this case it's valid
     * because it's possible the user will want to insert at the very end of
     * the vector.
     */
    if(index > vector->count)
        return;

    /* re-allocate? */
    if(vector->count == vector->capacity)
        vector_expand(vector, index);
    else
    {
        /* 
         * Move the element currently at insertion index to the end of the
         * vector
         */
        memcpy(vector->data + vector->count * vector->element_size, /* the end of the vector */
               vector->data + offset, /* the element that would be overwritten when inserting */
               vector->element_size);
    }

    /* copy new element into the specified index */
    memcpy(vector->data + offset, data, vector->element_size);
    ++(vector->count);
}

void vector_erase_index(struct vector_t* vector, intptr_t index)
{
    if(index >= vector->count)
        return;
    
    /* no need to copy memory if erasing the last index */
    if(index < vector->count)
    {
        /* copy last element to fill the gap */
        memcpy(vector->data + vector->element_size * index,    /* target is to overwrite the element specified by index */
            vector->data + (vector->count-1) * vector->element_size, /* last element */
            vector->element_size);
    }
    
    --(vector->count);
}

void vector_erase_element(struct vector_t* vector, void* element)
{
    /* copy last element to fill the gap, but only if it is not the last */
    if(element != vector->data + (vector->count-1) * vector->element_size)
    {
        memcpy(element,    /* target is to overwrite the element */
            vector->data + (vector->count-1) * vector->element_size, /* last element */
            vector->element_size);
    }
    --vector->count;
}

void* vector_get_element(struct vector_t* vector, intptr_t index)
{
    if(index >= vector->count)
        return NULL;
    return vector->data + (vector->element_size * index);
}

static void vector_expand(struct vector_t* vector, intptr_t insertion_index)
{
    intptr_t new_size;
    DATA_POINTER_TYPE* old_data;
    DATA_POINTER_TYPE* new_data;

    /* expand by factor 2 */
    new_size = vector->capacity << 1;
    
    /* 
     * If vector hasn't allocated anything yet, allocate the first two elements
     * and return 
     */
    if(new_size == 0)
    {
        new_size = 2;
        vector->data = MALLOC(vector->element_size * new_size);
        vector->capacity = new_size;
        return;
    }

    /* prepare for reallocating data */
    old_data = vector->data;
    new_data = (DATA_POINTER_TYPE*)MALLOC(vector->element_size * new_size);
    
    /* if no insertion index is required, copy all data to new memory */
    if(insertion_index == -1 || insertion_index >= new_size)
        memcpy(new_data, old_data, vector->element_size * vector->count);
    
    /* keep space for one element at the insertion index */
    else
    {
        /* copy old data up until right before insertion offset */
        intptr_t offset = vector->element_size * insertion_index;
        intptr_t total_size = vector->element_size * vector->count;
        memcpy(new_data, old_data, offset);
        /* copy the remaining amount of old data shifted one element ahead */
        memcpy(new_data + offset + vector->element_size,
               old_data + offset,
               total_size - offset);
    }
    vector->data = new_data;
    FREE(old_data);

    /* update counters */
    vector->capacity = new_size;
}

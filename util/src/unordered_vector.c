#include <string.h>
#include <stdlib.h>
#include "util/unordered_vector.h"
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
static void
unordered_vector_expand(struct unordered_vector_t* vector, 
                        intptr_t insertion_index);

struct unordered_vector_t*
unordered_vector_create(const intptr_t element_size)
{
    struct unordered_vector_t* vector = (struct unordered_vector_t*)MALLOC(sizeof(struct unordered_vector_t));
    unordered_vector_init_vector(vector, element_size);
    return vector;
}

void
unordered_vector_init_vector(struct unordered_vector_t* vector, const intptr_t element_size)
{
    memset(vector, 0, sizeof(struct unordered_vector_t));
    vector->element_size = element_size;
}

void
unordered_vector_destroy(struct unordered_vector_t* vector)
{
    unordered_vector_clear_free(vector);
    FREE(vector);
}

void
unordered_vector_clear(struct unordered_vector_t* vector)
{
    /* 
     * No need to free or overwrite existing memory, just reset the counter
     * and let future insertions overwrite
     */
    vector->count = 0;
}

void
unordered_vector_clear_free(struct unordered_vector_t* vector)
{
    if(vector->data)
        FREE(vector->data);
    vector->data = NULL;
    vector->count = 0;
    vector->capacity = 0;
}

void*
unordered_vector_push_emplace(struct unordered_vector_t* vector)
{
    void* data;
    if(vector->count == vector->capacity)
        unordered_vector_expand(vector, -1);
    data = (void*)((intptr_t)vector->data + (vector->element_size * vector->count));
    ++(vector->count);
    return data;
}

void
unordered_vector_push(struct unordered_vector_t* vector, void* data)
{
    memcpy(unordered_vector_push_emplace(vector), data, vector->element_size);
}

void*
unordered_vector_pop(struct unordered_vector_t* vector)
{
    if(!vector->count)
        return NULL;

    --(vector->count);
    return (void*)((intptr_t)vector->data + (vector->element_size * vector->count));
}

void
unordered_vector_erase_index(struct unordered_vector_t* vector, intptr_t index)
{
    if(index >= vector->count)
        return;
    
    /* no need to copy memory if erasing the last index */
    if(index + 1 < vector->count)
    {
        /* copy last element to fill the gap */
        memcpy(vector->data + vector->element_size * index,    /* target is to overwrite the element specified by index */
            (void*)((intptr_t)vector->data + (vector->count-1) * vector->element_size), /* last element */
            vector->element_size);
    }
    
    --(vector->count);
}

void
unordered_vector_erase_element(struct unordered_vector_t* vector, void* element)
{
    /* copy last element to fill the gap, but only if it is not the last */
    if(element != vector->data + (vector->count-1) * vector->element_size)
    {
        memcpy(element,    /* target is to overwrite the element */
            (void*)((intptr_t)vector->data + (vector->count-1) * vector->element_size), /* last element */
            vector->element_size);
    }
    --vector->count;
}

void*
unordered_vector_get_element(struct unordered_vector_t* vector, intptr_t index)
{
    if(index >= vector->count)
        return NULL;
    return vector->data + (vector->element_size * index);
}

static void
unordered_vector_expand(struct unordered_vector_t* vector,
                        intptr_t insertion_index)
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
        memcpy((void*)((intptr_t)new_data + offset + vector->element_size),
               (void*)((intptr_t)old_data + offset),
               total_size - offset);
    }
    vector->data = new_data;
    FREE(old_data);

    /* update counters */
    vector->capacity = new_size;
}

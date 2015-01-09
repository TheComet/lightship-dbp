#include <string.h>
#include <stdlib.h>
#include "util/ordered_vector.h"
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
ordered_vector_expand(struct ordered_vector_t* vector, 
                        intptr_t insertion_index);

struct ordered_vector_t*
ordered_vector_create(const intptr_t element_size)
{
    struct ordered_vector_t* vector = (struct ordered_vector_t*)MALLOC(sizeof(struct ordered_vector_t));
    ordered_vector_init_vector(vector, element_size);
    return vector;
}

void
ordered_vector_init_vector(struct ordered_vector_t* vector, const intptr_t element_size)
{
    memset(vector, 0, sizeof(struct ordered_vector_t));
    vector->element_size = element_size;
}

void
ordered_vector_destroy(struct ordered_vector_t* vector)
{
    ordered_vector_clear(vector);
    FREE(vector);
}

void
ordered_vector_clear(struct ordered_vector_t* vector)
{
    /* 
     * No need to free or overwrite existing memory, just reset the counter
     * and let future insertions overwrite
     */
    vector->count = 0;
}

void
ordered_vector_clear_free(struct ordered_vector_t* vector)
{
    if(vector->data)
        FREE(vector->data);
    vector->data = NULL;
    vector->count = 0;
    vector->capacity = 0;
}

void*
ordered_vector_push_emplace(struct ordered_vector_t* vector)
{
    void* data;
    if(vector->count == vector->capacity)
        ordered_vector_expand(vector, -1);
    data = vector->data + (vector->element_size * vector->count);
    ++(vector->count);
    return data;
}

void
ordered_vector_push(struct ordered_vector_t* vector, void* data)
{
    memcpy(ordered_vector_push_emplace(vector), data, vector->element_size);
}

void*
ordered_vector_pop(struct ordered_vector_t* vector)
{
    if(!vector->count)
        return NULL;

    --(vector->count);
    return vector->data + (vector->element_size * vector->count);
}

void ordered_vector_insert(struct ordered_vector_t* vector, intptr_t index, void* data)
{
    intptr_t offset;

    /* 
     * Normally the last valid index is (capacity-1), but in this case it's valid
     * because it's possible the user will want to insert at the very end of
     * the vector.
     */
    if(index > vector->count)
        return;

    /* re-allocate? */
    if(vector->count == vector->capacity)
        ordered_vector_expand(vector, index);
    else
    {
        /* shift all elements up by one to make space for insertion */
        intptr_t total_size = vector->count * vector->element_size;
        offset = vector->element_size * index;
        memmove(vector->data + offset + vector->element_size,
                vector->data + offset,
                total_size - offset);
    }

    /* copy new element into the specified index */
    offset = vector->element_size * index;
    memcpy(vector->data + offset, data, vector->element_size);
    ++vector->count;
}

void
ordered_vector_erase_index(struct ordered_vector_t* vector, intptr_t index)
{
    if(index >= vector->count)
        return;
    
    if(index == vector->count - 1)
        /* last element doesn't require memory shifting, just pop it */
        ordered_vector_pop(vector);
    else
    {
        /* shift memory right after the specified element down by one element */
        intptr_t offset = vector->element_size * index;  /* offset to the element being erased in bytes */
        intptr_t total_size = vector->element_size * vector->count; /* total current size in bytes */
        memmove(vector->data + offset,   /* target is to overwrite the element specified by index */
                vector->data + offset + vector->element_size,    /* copy beginning from one element ahead of element to be erased */
                total_size - offset - vector->element_size);     /* copying number of elements after element to be erased */
        --vector->count;
    }
}

void
ordered_vector_erase_element(struct ordered_vector_t* vector, DATA_POINTER_TYPE* element)
{
    DATA_POINTER_TYPE* last_element = vector->data + (vector->count-1) * vector->element_size;
    if(element != last_element)
    {
        memmove(element,    /* target is to overwrite the element */
                element + vector->element_size, /* read everything from next element */
                last_element - element - vector->element_size );
    }
    --vector->count;
}

void*
ordered_vector_get_element(struct ordered_vector_t* vector, intptr_t index)
{
    if(index >= vector->count)
        return NULL;
    return vector->data + (vector->element_size * index);
}

static void
ordered_vector_expand(struct ordered_vector_t* vector,
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
        memcpy(new_data + offset + vector->element_size,
               old_data + offset,
               total_size - offset);
    }
    vector->data = new_data;
    FREE(old_data);

    /* update counters */
    vector->capacity = new_size;
}

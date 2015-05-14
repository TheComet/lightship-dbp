#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util/unordered_vector.h"
#include "util/memory.h"

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

/*!
 * @brief Expands the underlying memory.
 *
 * This implementation will expand the memory by a factor of 2 each time this
 * is called. All elements are copied into the new section of memory.
 * @param [in] insertion_index Set to -1 if no space should be made for element
 * insertion. Otherwise this parameter specifies the index of the element to
 * "evade" when re-allocating all other elements.
 */
static void*
unordered_vector_expand(struct unordered_vector_t* vector,
                        uint32_t insertion_index);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
struct unordered_vector_t*
unordered_vector_create(const uint32_t element_size)
{
    struct unordered_vector_t* vector;
    if(!(vector = (struct unordered_vector_t*)MALLOC(sizeof(struct unordered_vector_t))))
        return NULL;
    unordered_vector_init_vector(vector, element_size);
    return vector;
}

/* ------------------------------------------------------------------------- */
void
unordered_vector_init_vector(struct unordered_vector_t* vector, const uint32_t element_size)
{
    assert(vector);
    assert(element_size);

    memset(vector, 0, sizeof(struct unordered_vector_t));
    vector->element_size = element_size;
}

/* ------------------------------------------------------------------------- */
void
unordered_vector_destroy(struct unordered_vector_t* vector)
{
    assert(vector);

    unordered_vector_clear_free(vector);
    FREE(vector);
}

/* ------------------------------------------------------------------------- */
void
unordered_vector_clear(struct unordered_vector_t* vector)
{
    assert(vector);
    /*
     * No need to free or overwrite existing memory, just reset the counter
     * and let future insertions overwrite
     */
    vector->count = 0;
}

/* ------------------------------------------------------------------------- */
void
unordered_vector_clear_free(struct unordered_vector_t* vector)
{
    assert(vector);

    if(vector->data)
        FREE(vector->data);
    vector->data = NULL;
    vector->count = 0;
    vector->capacity = 0;
}

/* ------------------------------------------------------------------------- */
void*
unordered_vector_push_emplace(struct unordered_vector_t* vector)
{
    void* data;

    assert(vector);
    assert(vector->element_size);

    if(vector->count == vector->capacity)
    {
        data = unordered_vector_expand(vector, -1);
        if(!data)
        {
            fprintf(stderr, "malloc() failed in unordered_vector_push_emplace() -- out of memory\n");
            return NULL;
        }
    }
    else
        data = vector->data;

    data = (void*)((intptr_t)data + (vector->element_size * vector->count));
    ++vector->count;
    return data;
}

/* ------------------------------------------------------------------------- */
char
unordered_vector_push(struct unordered_vector_t* vector, void* data)
{
    void* target;

    assert(vector);
    assert(data);

    target = unordered_vector_push_emplace(vector);
    if(!target)
        return 0;

    memcpy(target, data, vector->element_size);
    return 1;
}

/* ------------------------------------------------------------------------- */
void*
unordered_vector_pop(struct unordered_vector_t* vector)
{
    assert(vector);

    if(!vector->count)
        return NULL;

    --vector->count;
    return (void*)((intptr_t)vector->data + (vector->element_size * vector->count));
}

/* ------------------------------------------------------------------------- */
void*
unordered_vector_back(struct unordered_vector_t* vector)
{
    if(!vector->count)
        return NULL;

    return vector->data + (vector->element_size * (vector->count - 1));
}

/* ------------------------------------------------------------------------- */
void
unordered_vector_erase_index(struct unordered_vector_t* vector, uint32_t index)
{
    assert(vector);
    assert(vector->element_size);

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

    --vector->count;
}

/* ------------------------------------------------------------------------- */
void
unordered_vector_erase_element(struct unordered_vector_t* vector, void* element)
{
    void* last_element;

    assert(vector);
    assert(element);
    assert((DATA_POINTER_TYPE*)element >= vector->data);
    assert((DATA_POINTER_TYPE*)element < vector->data + vector->count * vector->element_size);

    /* copy last element to fill the gap, but only if it is not the last */
    --vector->count;
    last_element = (void*)(vector->data + vector->count * vector->element_size);
    if(element != last_element)
    {
        memcpy(element,    /* target is to overwrite the element */
               last_element,
               vector->element_size);
    }
}

/* ------------------------------------------------------------------------- */
void*
unordered_vector_get_element(struct unordered_vector_t* vector, uint32_t index)
{
    assert(vector);

    if(index >= vector->count)
        return NULL;
    return vector->data + index * vector->element_size;
}

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static void*
unordered_vector_expand(struct unordered_vector_t* vector,
                        uint32_t insertion_index)
{
    uint32_t new_size;
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
        if(!vector->data)
            return NULL;
        vector->capacity = new_size;
        return vector->data;
    }

    /* prepare for reallocating data */
    old_data = vector->data;
    new_data = (DATA_POINTER_TYPE*)MALLOC(vector->element_size * new_size);
    if(!new_data)
        return NULL;

    /* if no insertion index is required, copy all data to new memory */
    if(insertion_index == (uint32_t)-1 || insertion_index >= new_size)
        memcpy(new_data, old_data, vector->element_size * vector->count);

    /* keep space for one element at the insertion index */
    else
    {
        /* copy old data up until right before insertion offset */
        uint32_t offset = vector->element_size * insertion_index;
        uint32_t total_size = vector->element_size * vector->count;
        memcpy(new_data, old_data, offset);
        /* copy the remaining amount of old data shifted one element ahead */
        memcpy((void*)((intptr_t)new_data + offset + vector->element_size),
               (void*)((intptr_t)old_data + offset),
               total_size - offset);
    }
    vector->capacity = new_size;
    vector->data = new_data;
    FREE(old_data);

    return vector->data;
}

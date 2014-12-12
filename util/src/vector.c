#include <string.h>
#include <stdlib.h>
#include <util/vector.h>

struct vector_t* vector_create(const int element_size)
{
    struct vector_t* vector = (struct vector_t*)malloc(sizeof(struct vector_t));
    vector_init_vector(vector, element_size);
    return vector;
}

void vector_init_vector(struct vector_t* vector, const int element_size)
{
    struct vector_t tmp = {element_size, 0, 0, NULL};
    memcpy(vector, &tmp, sizeof(struct vector_t));
}

void vector_destroy(struct vector_t* vector)
{
    vector_clear(vector);
    free(vector);
}

void vector_clear(struct vector_t* vector)
{
    if(vector->data)
        free(vector->data);
    vector->count = 0;
}

void* vector_emplace(struct vector_t* vector)
{
	void* data;
    if(vector->count == vector->size)
        vector_expand(vector, -1);
    data = vector->data + (vector->element_size * vector->count);
    ++vector->count;
    return data;
}

void vector_push(struct vector_t* vector, void* data)
{
    memcpy(vector_emplace(vector), data, vector->element_size);
}

void* vector_pop(struct vector_t* vector)
{
    if(!vector->count)
        return NULL;
    
    --vector->count;
    return vector->data + (vector->element_size * vector->count);
}

void vector_insert(struct vector_t* vector, int index, void* data)
{
	int offset;

    /* last index (which would normally be invalid) is valid in this case */
    if(index > vector->size)
        return;
    
    if(vector->count == vector->size)
        vector_expand(vector, index);
    
    /* copy new element into the specified index */
    offset = vector->element_size * index;
    memcpy(vector->data + offset, data, vector->element_size);
    ++vector->count;
}

void vector_erase(struct vector_t* vector, int index)
{
	int offset;
	int total_size;

    if(index >= vector->size)
        return;
    
    /* shift memory right after the specified element down by one element */
    offset = vector->element_size * index;
    total_size = vector->element_size * vector->count;
    memcpy(vector->data + offset, vector->data + offset + vector->element_size, total_size - offset);
    --vector->count;
}

void* vector_get_element(struct vector_t* vector, int index)
{
    if(index >= vector->count)
        return NULL;
    return vector->data + (vector->element_size * index);
}

static void vector_expand(struct vector_t* vector, int insertion_index)
{
	int new_size;
	DATA_POINTER_TYPE old_data;
	DATA_POINTER_TYPE new_data;

    /* expand by factor 2 */
    new_size = vector->size << 2;
    if(new_size == 0)
        new_size = 2;
    
    /* prepare for reallocating data */
    old_data = vector->data;
    new_data = (DATA_POINTER_TYPE)malloc(vector->element_size * new_size);
    
    /* if no insertion index is required, copy all data to new memory */
    if(insertion_index == -1 || insertion_index >= new_size)
        memcpy(new_data, old_data, vector->element_size * vector->count);
    /* keep space for one element at the insertion index */
    else
    {
        /* copy old data up until right before insertion offset */
        int offset = vector->element_size * insertion_index;
        int total_size = vector->element_size * vector->count;
        memcpy(new_data, old_data, offset);
        /* copy the rest of the old data into the memory right after insertion offset */
        memcpy(new_data + offset + vector->element_size, old_data + offset, total_size - offset);
    }
    vector->data = new_data;
    free(old_data);
    
    /* update counters */
    vector->size = new_size;
}
#define DATA_POINTER_TYPE unsigned char*
typedef struct vector_t
{
    const int element_size;
    int count; /* number of elements inserted */
    int size;  /* how many elements actually fit into the allocated space */
    DATA_POINTER_TYPE data;
} vector_t;

vector_t* vector_create(const int element_size);
void vector_destroy(vector_t* vector);
void vector_clear(vector_t* vector);

#define vector_count(x) ((x)->count)

void vector_push(vector_t* vector, void* data);
void* vector_pop(vector_t* vector);
void vector_insert(vector_t* vector, int index, void* data);
void vector_erase(vector_t* vector, int index);
void* vector_get_element(vector_t*, int index);

static void vector_expand(vector_t* vector, int insertion_index);
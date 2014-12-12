#define DATA_POINTER_TYPE unsigned char*
struct vector_t
{
    const int element_size;
    int count; /* number of elements inserted */
    int size;  /* how many elements actually fit into the allocated space */
    DATA_POINTER_TYPE data;
};

struct vector_t* vector_create(const int element_size);
void vector_init_vector(struct vector_t* vector, const int element_size);
void vector_destroy(struct vector_t* vector);
void vector_clear(struct vector_t* vector);

#define vector_count(x) ((x)->count)

void vector_push(struct vector_t* vector, void* data);
void* vector_emplace(struct vector_t* vector);
void* vector_pop(struct vector_t* vector);
void vector_insert(struct vector_t* vector, int index, void* data);
void vector_erase(struct vector_t* vector, int index);
void* vector_get_element(struct vector_t*, int index);

static void vector_expand(struct vector_t* vector, int insertion_index);
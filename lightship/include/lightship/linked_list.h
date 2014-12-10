struct list_node_t;

typedef struct list_node_t
{
    struct list_node_t* prev;
    struct list_node_t* next;
    void* data;
} list_node_t;

typedef struct list_t
{
    int count;
    struct list_node_t* head;
    struct list_node_t* tail;
} list_t;

list_t* list_create(void);
void list_init_list(list_t* list);
void list_destroy(list_t* list);
void list_clear(list_t* list);

#define list_count(x) ((x)->count)
#define list_head_data(x) ((x)->head != NULL ? (x)->head->data : NULL)
#define list_tail_data(x) ((x)->tail != NULL ? (x)->tail->data : NULL)

void list_push(list_t* list, void* data);
void* list_pop(list_t* list);
void* list_erase_node(list_t* list, list_node_t* node);
void list_erase_element(list_t* list, void* data);

#define LIST_FOR_EACH(list, var_type, var) \
    var_type var; \
    for(list_node_t* node = (list)->tail; node != NULL && (var = node->data); node = node->next)

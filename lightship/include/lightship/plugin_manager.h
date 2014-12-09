/* node for doubly linked list between plugins */
struct plugin_t;
struct plugin_node_t
{
    struct plugin_t* next;
    struct plugin_t* prev;
} plugin_node_t;

/* plugin struct */
typedef struct plugin_t
{
    struct plugin_node_t node;
} plugin_t;

/* container for plugins */
typedef struct plugins_t
{
    struct plugin_t* head;
    struct plugin_t* tail;
} plugins_t;

void init_plugin_manager();
void load_plugin(const char* filename);

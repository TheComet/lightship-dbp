#include "util/pstdint.h"

struct plugin_t;
struct ptree_t;

/* external service function signatures */
typedef uint32_t (*yaml_load_func)(const char*);
typedef char* (*yaml_get_value_func)(uint32_t, const char*);
typedef struct ptree_t* (*yaml_get_dom_func)(uint32_t);
typedef void (*yaml_destroy_func)(uint32_t);

/* extern service function pointers */
extern yaml_load_func yaml_load;
extern yaml_get_value_func yaml_get_value;
extern yaml_get_dom_func yaml_get_dom;
extern yaml_destroy_func yaml_destroy;

void
register_services(const struct plugin_t* plugin);

char
get_required_services(void);

void
get_optional_services(void);

#include "util/pstdint.h"

struct plugin_t;
struct ptree_t;

typedef uint32_t(*yaml_load_func)(const char*);
typedef char*(*yaml_get_value_func)(uint32_t, const char*);
typedef struct ptree_t*(*yaml_get_dom_func)(uint32_t);
typedef void(*yaml_destroy_func)(uint32_t);

void
register_services(const struct plugin_t* plugin);

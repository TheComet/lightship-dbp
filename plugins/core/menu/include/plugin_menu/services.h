#include "util/pstdint.h"

struct plugin_t;
struct ptree_t;

/* external service function signatures */
typedef uint32_t (*yaml_load_func)(const char*);
typedef char* (*yaml_get_value_func)(uint32_t, const char*);
typedef struct ptree_t* (*yaml_get_dom_func)(uint32_t);
typedef void (*yaml_destroy_func)(uint32_t);
typedef void (*shapes_2d_begin_func)(void);
typedef uint32_t (*shapes_2d_end_func)(void);
typedef void (*shapes_2d_destroy_func)(uint32_t);
typedef void (*line_2d_func)(float, float, float, float, uint32_t);
typedef void (*box_2d_func)(float, float, float, float, uint32_t);

/* extern service function pointers */
extern yaml_load_func yaml_load;
extern yaml_get_value_func yaml_get_value;
extern yaml_get_dom_func yaml_get_dom;
extern yaml_destroy_func yaml_destroy;
extern shapes_2d_begin_func shapes_2d_begin;
extern shapes_2d_end_func shapes_2d_end;
extern shapes_2d_destroy_func shapes_2d_destroy;
extern line_2d_func line_2d;
extern box_2d_func box_2d;

void
register_services(const struct plugin_t* plugin);

char
get_required_services(void);

void
get_optional_services(void);

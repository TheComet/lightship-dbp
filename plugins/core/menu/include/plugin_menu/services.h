#include "util/pstdint.h"

struct plugin_t;
struct service_t;

/* extern service function pointers */
extern struct service_t* yaml_load;
extern struct service_t* yaml_get_value;
extern struct service_t* yaml_get_dom;
extern struct service_t* yaml_destroy;

extern struct service_t* shapes_2d_begin;
extern struct service_t* shapes_2d_end;
extern struct service_t* shapes_2d_destroy;
extern struct service_t* line_2d;
extern struct service_t* box_2d;
extern struct service_t* shapes_2d_show;
extern struct service_t* shapes_2d_hide;

extern struct service_t* text_group_create;
extern struct service_t* text_group_destroy;
extern struct service_t* text_group_load_character_set;
extern struct service_t* text_create;
extern struct service_t* text_destroy;
extern struct service_t* text_show;
extern struct service_t* text_hide;

void
register_services(struct plugin_t* plugin);

char
get_required_services(struct plugin_t* plugin);

void
get_optional_services(struct plugin_t* plugin);

#include "util/pstdint.h"
#include "plugin_yaml.h"
#include "plugin_renderer_gl.h"

struct plugin_t;
struct ptree_t;

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

extern text_load_font_func text_load_font;
extern text_load_characters_func text_load_characters;
extern text_add_static_center_string_func text_add_static_center_string;
extern text_destroy_static_string_func text_destroy_static_string;

void
register_services(const struct plugin_t* plugin);

char
get_required_services(void);

void
get_optional_services(void);

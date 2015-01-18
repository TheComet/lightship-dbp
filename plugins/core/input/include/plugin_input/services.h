#include "plugin_renderer_gl.h"

struct plugin_t;

/* extern service function pointers *
extern window_width_func window_width;
extern window_height_func window_height;*/

void
register_services(const struct plugin_t* plugin);

char
get_required_services(void);

void
get_optional_services(void);

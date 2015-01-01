#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "util/services.h"

void
register_services(const struct plugin_t* plugin)
{
    service_register(plugin, "shapes_2d_begin", (intptr_t)shapes_2d_begin);
    service_register(plugin, "shapes_2d_end", (intptr_t)shapes_2d_end);
    service_register(plugin, "line_2d", (intptr_t)line_2d);
}

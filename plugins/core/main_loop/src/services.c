#include "util/services.h"
#include "plugin_main_loop/main_loop.h"

void
register_services(struct plugin_t* plugin)
{
    SERVICE_REGISTER0(plugin, "start", main_loop_start, void);
    SERVICE_REGISTER0(plugin, "stop", main_loop_stop, void);
}

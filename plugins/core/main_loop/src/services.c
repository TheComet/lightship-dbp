#include "framework/services.h"
#include "framework/plugin_api.h"
#include "plugin_main_loop/services.h"
#include "plugin_main_loop/main_loop.h"

void
register_services(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    SERVICE_REGISTER0(game, plugin, "start", main_loop_start, void);
    SERVICE_REGISTER0(game, plugin, "stop", main_loop_stop, void);
}

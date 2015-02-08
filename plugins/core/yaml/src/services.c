#include "util/services.h"
#include "plugin_yaml/services.h"
#include "plugin_yaml/parser.h"

void
register_services(struct game_t* game, const struct plugin_t* plugin)
{
    /* -----------------------------------------------------
     * All services this plugin supports
     * ---------------------------------------------------*/
    SERVICE_REGISTER1(game, plugin, "load",       yaml_load_wrapper, uint32_t, const char*);
    SERVICE_REGISTER2(game, plugin, "get_value",  yaml_get_value_wrapper, char*, uint32_t, const char*);
    SERVICE_REGISTER1(game, plugin, "get_dom",    yaml_get_dom_wrapper, struct ptree_t*, uint32_t);
    SERVICE_REGISTER1(game, plugin, "destroy",    yaml_destroy_wrapper, void, uint32_t);
}

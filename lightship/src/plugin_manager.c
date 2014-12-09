#include <stdlib.h>
#include <lightship/plugin_manager.h>

static plugins_t plugins;

void init_plugin_manager()
{
    list_init(&plugins);
}

void load_plugin(const char* filename)
{
    plugin_t* plugin = (plugin_t*)malloc(sizeof(plugin_t));
    
}

static void list_init(plugins_t* list)
{
    list->head = NULL;
    list->tail = NULL;
}

static void ins
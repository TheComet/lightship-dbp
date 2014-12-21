#include "main_loop/main_loop.h"
#include "lightship/api.h"
#include "util/event_api.h"

static char is_looping = 0;
static struct event_t* evt_render = NULL;

void register_events(struct plugin_t* plugin, struct lightship_api_t* api)
{
    evt_render = api->event_create(plugin, "render");
}

void register_listeners(struct plugin_t* plugin, struct lightship_api_t* api)
{
    api->event_register_listener(plugin, "renderer_gl.close_window", main_loop_stop);
}

void main_loop_start(void)
{
    is_looping = 1;
    while(is_looping)
    {
        evt_render->exec(evt_render, NULL);
    }
}

void main_loop_stop(struct event_t* evt, void* args)
{
    is_looping = 0;
}

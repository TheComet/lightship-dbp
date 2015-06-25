#include "plugin_input/services.h"
#include "framework/services.h"

void
register_services(struct plugin_t* plugin)
{
	/* -----------------------------------------------------
	* All services this plugin supports
	* ---------------------------------------------------*/
	/*
	 * service_register(plugin, "service_name_1", service_callback_1);
	 * service_register(plugin, "service_name_2", service_callback_2);
	 * etc...
	 */

}

char
get_required_services(void)
{
	/*if(!(window_width = (window_width_func)service_get("renderer_gl.window_width")))
		return 0;
	if(!(window_height = (window_height_func)service_get("renderer_gl.window_height")))
		return 0;*/
	return 1;
}

void
get_optional_services(void)
{
}

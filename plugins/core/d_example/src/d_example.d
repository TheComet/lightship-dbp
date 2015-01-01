import api;
import std.stdio, std.string, core.runtime;

plugin_t* plugin;

extern(C)
plugin_t* plugin_init(lightship_api_t* lightship_api) {
  Runtime.initialize();
  plugin = plugin_create();
  plugin_set_info(plugin, "name".ptr, "category".ptr, "author".ptr, "description".ptr, "website".ptr);
  plugin_set_version(plugin, 0, 0, 1);

  Log.log_info("hello from d example plugin_init");

  return plugin;
}

extern(C)
char plugin_start(lightship_api_t* lightship_api) {
  Log.log_info("hello from d example plugin_start");
  return 1;
}

extern(C)
void plugin_stop() {
  Log.log_info("hello from d example plugin_stop");
  plugin_destroy(plugin);
  Runtime.terminate();
}

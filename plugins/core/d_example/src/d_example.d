import plugin;
import log;
import std.stdio, std.string, core.runtime;

plugin_t* example_plugin;

extern (C)
plugin_t* plugin_init(lightship_api_t* lightship_api) {
  Runtime.initialize();
  example_plugin = plugin_create();
  plugin_set_info(example_plugin, "name".ptr, "category".ptr, "author".ptr, "description".ptr, "website".ptr);
  plugin_set_version(example_plugin, 0, 0, 1);

  // Log.log_info("hello from d example plugin_init");

  return example_plugin;
}

extern (C)
char plugin_start(lightship_api_t* lightship_api) {
  // Log.log_info("hello from d example plugin_start");
  return 1;
}

extern (C)
void plugin_stop() {
  // Log.log_info("hello from d example plugin_stop");
  plugin_destroy(example_plugin);
  Runtime.terminate();
}

struct lightship_api_t;

extern(C) alias plugin_init_func = plugin_t function(lightship_api_t*);
extern(C) alias plugin_start_func = char function(lightship_api_t*);
extern(C) alias plugin_stop_func = void function();

enum plugin_programming_language_t
{
    PLUGIN_PROGRAMMING_LANGUAGE_UNSET,
    PLUGIN_PROGRAMMING_LANGUAGE_C,
    PLUGIN_PROGRAMMING_LANGUAGE_CPP,
    PLUGIN_PROGRAMMING_LANGUAGE_D
}

struct plugin_api_version_t
{
    uint major;
    uint minor;
    uint patch;
}

struct plugin_info_t
{
    char* name;
    char* category;
    char* author;
    char* description;
    char* website;
    plugin_programming_language_t language;
    plugin_api_version_t v;
}

struct plugin_t
{
    plugin_info_t info;
    void* handle;
    plugin_init_func init;
    plugin_start_func start;
    plugin_stop_func stop;
}

extern (C) {
  extern plugin_t* plugin_create();
  extern void plugin_set_info(plugin_t* plugin, immutable(char)* name, immutable(char)* category, immutable(char)* author, immutable(char)* description, immutable(char)* website);
  extern void plugin_set_version(plugin_t* plugin, uint major, uint minior, uint patch);
  extern void plugin_destroy(plugin_t* plugin);
}

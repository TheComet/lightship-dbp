struct plugin_t;
struct service_t;

void
register_services(struct plugin_t* plugin);

char
get_required_services(struct plugin_t* plugin);

void
get_optional_services(struct plugin_t* plugin);

struct plugin_t;

void
register_services(const struct plugin_t* plugin);

char
get_required_services(void);

void
get_optional_services(void);

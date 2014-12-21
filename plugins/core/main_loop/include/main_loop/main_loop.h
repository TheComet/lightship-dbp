struct lightship_api_t;
struct plugin_t;
struct event_t;

void register_events(struct plugin_t* plugin, struct lightship_api_t* api);
void register_listeners(struct plugin_t* plugin, struct lightship_api_t* api);
void main_loop_start(void);
void main_loop_stop(struct event_t* evt, void* args);

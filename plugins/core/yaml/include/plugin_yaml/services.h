#include "util/pstdint.h"

struct plugin_t;
struct game_t;

void
register_services(struct game_t* game, struct plugin_t* plugin);

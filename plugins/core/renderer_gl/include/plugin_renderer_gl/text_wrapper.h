#include "util/pstdint.h"
#include "util/service_api.h"

struct map_t;
struct text_group_t;

char
text_wrapper_init(void);

void
text_wrapper_deinit(void);

/* ------------------------------------------------------------------------- */
SERVICE(text_group_create_wrapper);
SERVICE(text_group_destroy_wrapper);
SERVICE(text_group_load_character_set_wrapper);

/* ------------------------------------------------------------------------- */
SERVICE(text_create_wrapper);
SERVICE(text_destroy_wrapper);
SERVICE(text_set_centered_wrapper);
SERVICE(text_set_position_wrapper);
SERVICE(text_set_string_wrapper);
SERVICE(text_show_wrapper);
SERVICE(text_hide_wrapper);

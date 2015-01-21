#include "util/pstdint.h"

typedef intptr_t    (*button_create_func)       (const char*, float, float, float, float);
typedef void        (*button_destroy_func)      (intptr_t);
typedef wchar_t*    (*button_get_text_func)     (intptr_t);
typedef void        (*screen_manager_load_func) (const char*);

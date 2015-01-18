#include "util/pstdint.h"
#include <wchar.h>

typedef uint32_t(*window_width_func)                    (void);
typedef uint32_t(*window_height_func)                   (void);
typedef void    (*shapes_2d_begin_func)                 (void);
typedef uint32_t(*shapes_2d_end_func)                   (void);
typedef void    (*line_2d_func)                         (float, float, float, float, uint32_t);
typedef void    (*box_2d_func)                          (float, float, float, float, uint32_t);
typedef void    (*shapes_2d_destroy_func)               (uint32_t);
typedef uint32_t(*text_load_font_func)                  (const char*, uint32_t);
typedef void    (*text_destroy_font_func)               (uint32_t);
typedef void    (*text_load_characters_func)            (uint32_t, const wchar_t*);
typedef intptr_t(*text_add_static_string_func)          (uint32_t, float, float, const wchar_t*);
typedef intptr_t(*text_add_static_center_string_func)   (uint32_t, float, float, const wchar_t*);
typedef void    (*text_destroy_static_string_func)      (uint32_t, intptr_t);
typedef void    (*text_destroy_all_static_strings_func) (uint32_t);

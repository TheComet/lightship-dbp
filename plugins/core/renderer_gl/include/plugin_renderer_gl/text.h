#include "ft2build.h"
#include FT_FREETYPE_H

struct font_t
{
    FT_Face face;
};

char text_init(void);
void text_deinit(void);
struct font_t* text_load_font(const char* filename);

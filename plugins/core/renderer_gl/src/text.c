#include "plugin_renderer_gl/text.h"
#include "util/log.h"
#include "ft2build.h"
#include FT_FREETYPE_H

static FT_Error error;
static FT_Library g_lib;
static FT_Face g_face;

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    llog(LOG_ERROR, 1, message);
}

char text_init(void)
{
    error = FT_Init_FreeType(&g_lib);
    if(error)
    {
        llog(LOG_ERROR, 1, "Failed to initialise freetype");
        return 1;
    }
    /*error = FT_New_Face(g_lib, "*/
    return 0;
}

void text_deinit(void)
{
}

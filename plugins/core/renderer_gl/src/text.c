#include "plugin_renderer_gl/text.h"
#include "util/log.h"
#include "ft2build.h"
#include FT_FREETYPE_H

FT_Library g_lib;

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    llog(LOG_ERROR, 1, message);
}

char text_init(void)
{
    if(FT_Init_FreeType(&g_lib))
    {
        llog(LOG_ERROR, 1, "Failed to initialise freetype");
        return 1;
    }
}

void text_deinit(void)
{
}

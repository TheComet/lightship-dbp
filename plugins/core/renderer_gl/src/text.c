#include "plugin_renderer_gl/text.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/string.h"
#include "ft2build.h"
#include FT_FREETYPE_H

static FT_Library g_lib;
static FT_Face g_face;

#ifdef _DEBUG
static const char* ttf_prefix = "../../plugins/core/renderer_gl/";
#else
static const char* ttf_prefix = "./";
#endif

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    llog(LOG_ERROR, 1, message);
}

char text_init(void)
{
    FT_Error error;
    char* ttf_filename;

    /* init freetype */
    error = FT_Init_FreeType(&g_lib);
    if(error)
    {
        llog(LOG_ERROR, 1, "Failed to initialise freetype");
        return 0;
    }
    
    /* load face */
    ttf_filename = cat_strings(2, ttf_prefix, "ttf/DejaVuSans.ttf");
    error = FT_New_Face(g_lib, ttf_filename, 0, &g_face);
    if(error == FT_Err_Unknown_File_Format)
    {
        llog(LOG_ERROR, 3, "The font file \"", ttf_filename, "\" could be opened and read, but it appears that its font format is unsupported");
        FREE(ttf_filename);
        return 0;
    }
    else if(error)
    {
        llog(LOG_ERROR, 3, "Failed to open font file \"", ttf_filename, "\"");
        FREE(ttf_filename);
        return 0;
    }
    FREE(ttf_filename);
    
    /* set default size 9 *
    error = FT_Set_Char_Size(&g_face, 0, 9*64, 300, 300);
    if(error)
    {
        llog(LOG_ERROR, 1, "Failed to set the character size to 9");
        return 0;
    }*/
    
    

    return 1;
}

void text_deinit(void)
{
}

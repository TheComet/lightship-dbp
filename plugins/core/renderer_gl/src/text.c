#include "plugin_renderer_gl/text.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/unordered_vector.h"
#include "util/string.h"
#include <wchar.h>

static FT_Library g_lib;
static struct unordered_vector_t g_fonts;

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

    /* init freetype */
    error = FT_Init_FreeType(&g_lib);
    if(error)
    {
        llog(LOG_ERROR, 1, "Failed to initialise freetype");
        return 0;
    }
    
    unordered_vector_init_vector(&g_fonts, sizeof(struct font_t));
    
    return 1;
}

void text_deinit(void)
{
    UNORDERED_VECTOR_FOR_EACH(&g_fonts, struct font_t, font)
    {
        FT_Done_Face(font->face);
    }
    unordered_vector_clear_free(&g_fonts);
    FT_Done_FreeType(g_lib);
}

struct font_t* text_load_font(const char* filename)
{
    FT_Error error;
    char* ttf_filename;
    struct font_t* font;
    
    /* create new font object */
    font = (struct font_t*)unordered_vector_push_emplace(&g_fonts);
    
    /* add path prefix to filename */
    ttf_filename = cat_strings(2, ttf_prefix, filename);
    
    /* if the program breaks from this for-loop, something went wrong. */
    for(;;)
    {

        /* load face */
        error = FT_New_Face(g_lib, ttf_filename, 0, &font->face);
        if(error == FT_Err_Unknown_File_Format)
        {
            llog(LOG_ERROR, 3, "The font file \"", ttf_filename, "\" could be opened and read, but it appears that its font format is unsupported");
            break;
        }
        else if(error)
        {
            llog(LOG_ERROR, 3, "Failed to open font file \"", ttf_filename, "\"");
            break;
        }
        
        /* set default size 9 */
        error = FT_Set_Char_Size(font->face, 0, 9*64, 300, 300);
        if(error)
        {
            llog(LOG_ERROR, 1, "Failed to set the character size to 9");
            break;
        }

        /*
         * If the program reaches this point, it means loading the font was
         * successful.
         */
        FREE(ttf_filename);
        return font;
    }

    /*
     * If the program reaches this point, it means something went wrong.
     * Clean up and return.
     */
    FREE(ttf_filename);
    unordered_vector_erase_element(&g_fonts, font);
    return NULL;
}

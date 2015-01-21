#include "plugin_menu/screen_manager.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "util/ptree.h"
#include <string.h>
#include <stdlib.h>

void
screen_manager_load(const char* file_name)
{
    struct ptree_t* dom;
    uint32_t doc = yaml_load(file_name);
    dom = yaml_get_dom(doc);
    if(!dom)
        return;
    
    {   /* iterate screens */
        PTREE_FOR_EACH(dom, screen)
        {   /* iterate objects in current screen */
            PTREE_FOR_EACH(screen, object)
            {
                if(PTREE_HASH_STRING("button") == object->hash)
                {
                    char* text_str = NULL;
                    const struct ptree_t* text   = ptree_find_by_key(object, "text");
                    const struct ptree_t* x      = ptree_find_by_key(object, "position.x");
                    const struct ptree_t* y      = ptree_find_by_key(object, "position.y");
                    const struct ptree_t* width  = ptree_find_by_key(object, "size.x");
                    const struct ptree_t* height = ptree_find_by_key(object, "size.y");
                    if(!x || !y || !width || !height)
                        continue;
                    if(text)
                        text_str = (char*)text->value;
                    button_create(text_str,
                                  atof(x->value),
                                  atof(y->value),
                                  atof(width->value),
                                  atof(height->value));
                }
            }
        }
    }

    yaml_destroy(doc);
}

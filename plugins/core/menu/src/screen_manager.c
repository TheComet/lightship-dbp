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
                    char *x, *y, *width, *height;
                    if(!(x = ptree_find_by_key(object, "position.x")))
                        continue;
                    if(!(y = ptree_find_by_key(object, "position.y")))
                        continue;
                    if(!(width = ptree_find_by_key(object, "size.x")))
                        continue;
                    if(!(height = ptree_find_by_key(object, "size.y")))
                        continue;
                    button_create(ptree_find_by_key(object, "text"),
                                  atof(x),
                                  atof(y),
                                  atof(width),
                                  atof(height));
                }
            }
        }
    }

    yaml_destroy(doc);
}

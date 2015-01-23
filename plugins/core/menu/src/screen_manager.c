#include "plugin_menu/screen_manager.h"
#include "plugin_menu/screen.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"
#include <string.h>
#include <stdlib.h>

struct menu_t*
menu_load(const char* file_name)
{
    struct menu_t* menu;
    struct ptree_t* dom;
    struct ptree_t* screens;
    uint32_t doc;
    
    llog(LOG_INFO, 3, "[menu] Loading menu from file \"", file_name, "\"");
    
    /* load and parse yaml file, get DOM */
    doc = yaml_load(file_name);
    dom = yaml_get_dom(doc);
    if(!dom)
    {
        yaml_destroy(doc);
        return NULL;
    }
    
    /* get ptree for screens */
    screens = ptree_find_by_key(dom, "screens");
    if(!screens)
    {
        llog(LOG_ERROR, 1, "[menu] Failed to find \"screens\" node");
        yaml_destroy(doc);
        return NULL;
    }
    
    /* create new menu object in which to store menu elements */
    menu = (struct menu_t*)MALLOC(sizeof(struct menu_t));
    menu_init_menu(menu);
    
    /* iterate screens */
    { PTREE_FOR_EACH(screens, screen_node)
    {
        struct screen_t* screen;

        /* 
         * Screen names must be unique. Verify by searching the map of
         * screens currently registered.
         */
        if(map_find(&menu->screens, screen_node->hash))
        {
            llog(LOG_WARNING, 2, "[menu] Screen with duplicate name found: ", (char*)screen_node->value);
            llog(LOG_WARNING, 1, "[menu] Screen will not be created");
            continue;
        }
        
        /* create new screen object */
        screen = screen_create();
        map_insert(&menu->screens, screen_node->hash, screen);

        /* iterate objects to add to this screen */
        { PTREE_FOR_EACH(screen_node, object_node)
        {
            if(PTREE_HASH_STRING("button") == object_node->hash)
            {
                struct button_t* button;

                /* retrieve button parameters required to create a button */
                char* text = NULL;
                char* action = NULL;
                const struct ptree_t* text_node   = ptree_find_by_key(object_node, "text");
                const struct ptree_t* action_node = ptree_find_by_key(object_node, "action");
                const struct ptree_t* x_node      = ptree_find_by_key(object_node, "position.x");
                const struct ptree_t* y_node      = ptree_find_by_key(object_node, "position.y");
                const struct ptree_t* width_node  = ptree_find_by_key(object_node, "size.x");
                const struct ptree_t* height_node = ptree_find_by_key(object_node, "size.y");
                if(!x_node || !y_node || !width_node || !height_node)
                    continue;
                if(text_node)
                    text = (char*)text_node->value;
                if(action_node)
                    action = (char*)action_node->value;
                
                /* add button to current screen */
                button = button_create(text,  /* text is allowed to be NULL */
                                    atof(x_node->value),
                                    atof(y_node->value),
                                    atof(width_node->value),
                                    atof(height_node->value));
                screen_add_button(screen, button);
            }
        }}
    }}

    yaml_destroy(doc);
    return menu;
}

void
menu_init_menu(struct menu_t* menu)
{
    map_init_map(&menu->screens);
}

void
menu_destroy(struct menu_t* menu)
{
    MAP_FOR_EACH(&menu->screens, struct screen_t, key, screen)
    {
        screen_destroy(screen);
    }
    map_clear_free(&menu->screens);
    FREE(menu);
}

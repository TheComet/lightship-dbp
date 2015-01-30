#include "plugin_menu/screen_manager.h"
#include "plugin_menu/screen.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "util/ordered_vector.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/services.h"
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
    SERVICE_CALL1(yaml_load, &doc, file_name);
    SERVICE_CALL1(yaml_get_dom, &dom, doc);
    if(!dom)
    {
        SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc);
        return NULL;
    }
    
    /* get ptree for screens */
    screens = ptree_find_by_key(dom, "screens");
    if(!screens)
    {
        llog(LOG_ERROR, 1, "[menu] Failed to find \"screens\" node");
        SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc);
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
                const struct ptree_t* text_node   = ptree_find_by_key(object_node, "text");
                const struct ptree_t* x_node      = ptree_find_by_key(object_node, "position.x");
                const struct ptree_t* y_node      = ptree_find_by_key(object_node, "position.y");
                const struct ptree_t* width_node  = ptree_find_by_key(object_node, "size.x");
                const struct ptree_t* height_node = ptree_find_by_key(object_node, "size.y");
                const struct ptree_t* action_service_node = ptree_find_by_key(object_node, "action.service");
                const struct ptree_t* action_argv_node    = ptree_find_by_key(object_node, "action.args");
                if(!x_node || !y_node || !width_node || !height_node)
                    continue;
                if(text_node)
                    text = (char*)text_node->value;
                
                /* add button to current screen */
                button = button_create(text,  /* text is allowed to be NULL */
                                    atof(x_node->value),
                                    atof(y_node->value),
                                    atof(width_node->value),
                                    atof(height_node->value));
                screen_add_button(screen, button);
                
                /* extract service name and arguments tied to action, if any */
                if(action_service_node && action_argv_node)
                {
                    struct ptree_t* arg_node;
                    char arg_key[sizeof(int)*8+1];
                    struct ordered_vector_t action_argv; /* holds argument list as (char**) */
                    int action_argc = 0;

                    /* inserts the arguments into action_argv */
                    ordered_vector_init_vector(&action_argv, sizeof(char*));
                    while(1)
                    {
                        /* retrieve next argument */
                        sprintf(arg_key, "%d", action_argc);
                        arg_node = ptree_find_by_key(action_argv_node, arg_key);
                        if(!arg_node)
                            break;
                        /* argument found, add to argument list */
                        ordered_vector_push(&action_argv, &arg_node->value);
                        ++action_argc;
                    }
                    
                    
                    if(action_service_node->value)
                    {
                        struct service_t* action_service = service_get((char*)action_service_node->value);
                        if(action_service)
                        {
                            
                        }
                    }
                    
                    ordered_vector_clear_free(&action_argv);
                }
            }
        }}
    }}

    SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc);
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

#include "plugin_menu/menu.h"
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

struct map_t g_menus;
static intptr_t guid = 1;

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
#ifdef _DEBUG
            llog(LOG_WARNING, 2, "[menu] Screen with duplicate name found: ", (char*)screen_node->key);
#else
            llog(LOG_WARNING, 1, "[menu] Screen with duplicate name found");
#endif
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
                const struct ptree_t* action_node = ptree_find_by_key(object_node, "action");
                if(!x_node || !y_node || !width_node || !height_node)
                {
                    llog(LOG_WARNING, 1, "[menu] Not enough data to create button. Need at least position and size.");
                    continue;
                }
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
                if(action_node)
                {
                    struct ptree_t* service_node;
                    struct ptree_t* argv_node;
                    char arg_key[sizeof(int)*8+1];
                    int action_argc = 0;

                    /* extract each argument and insert into vector */
                    button->action.argv = ordered_vector_create(sizeof(void**));
                    argv_node = ptree_find_by_key(action_node, "argv");
                    while(argv_node)
                    {
                        /* retrieve next argument */
                        struct ptree_t* arg_node;
                        sprintf(arg_key, "%d", action_argc);
                        arg_node = ptree_find_by_key(argv_node, arg_key);
                        if(!arg_node)
                            break;
                        /* argument found, add to argument list */
                        ordered_vector_push(button->action.argv, &arg_node->value);
                        ++action_argc;
                    }
                    
                    /* get service name */
                    service_node = ptree_find_by_key(action_node, "service");

                    /* set up action to trigger when this button is pressed */
                    if(service_node && service_node->value)
                    {
                        struct service_t* action_service = service_get((char*)service_node->value);
                        if(action_service)
                        {
                            button->action.service = action_service;
                        }
                        else
                        {
                            llog(LOG_WARNING, 3, "[menu] Tried to bind button to service \"",
                                                 (char*)service_node->value,
                                                 "\", but the service was not found.");
                        }
                    }
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

SERVICE(menu_load_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, file_name, const char*, const char*);

    intptr_t id = guid++;
    struct menu_t* menu = menu_load(file_name);
    if(!menu)
        SERVICE_RETURN(0, intptr_t);

    map_insert(&g_menus, id, menu);
    SERVICE_RETURN(id, intptr_t);
}

SERVICE(menu_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, intptr_t, intptr_t);
    
    struct menu_t* menu = map_erase(&g_menus, id);
    if(menu)
        menu_destroy(menu);
}

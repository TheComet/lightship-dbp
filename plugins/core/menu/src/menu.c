#include "plugin_menu/config.h"
#include "plugin_menu/menu.h"
#include "plugin_menu/screen.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "util/ordered_vector.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/services.h"
#include "util/string.h"
#include <string.h>
#include <stdlib.h>

struct map_t g_menus;

static void
menu_load_screens(struct menu_t* menu, const struct ptree_t* screen_node);

static void
menu_load_button(struct screen_t* screen, const struct ptree_t* button_node);

static void
menu_load_button_action(struct button_t* button, const struct ptree_t* action_node);

void
menu_init(void)
{
    map_init_map(&g_menus);
}

void
menu_deinit(void)
{
    map_clear_free(&g_menus);
}

struct menu_t*
menu_load(const char* file_name)
{
    struct menu_t* menu;
    struct ptree_t* dom;
    struct ptree_t* screens;
    char* menu_name;
    uint32_t doc;
    
    llog(LOG_INFO, PLUGIN_NAME, 3, "Loading menu from file \"", file_name, "\"");
    
    /* load and parse yaml file, get DOM */
    SERVICE_CALL1(yaml_load, &doc, *file_name);
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
        llog(LOG_ERROR, PLUGIN_NAME, 1, "Failed to find \"screens\" node");
        SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc);
        return NULL;
    }
    
    /* menu must have a name */
    {
        struct ptree_t* name_node = ptree_find_by_key(dom, "name");
        if(name_node && name_node->value)
            menu_name = name_node->value;
        else
        {
            llog(LOG_ERROR, PLUGIN_NAME, 1, "Failed to find \"name\" node");
            SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc);
            return NULL;
        }
    }
    
    /* create new menu object in which to store menu elements */
    menu = (struct menu_t*)MALLOC(sizeof(struct menu_t));
    menu_init_menu(menu);
    
    /* set menu name */
    menu->name = malloc_string(menu_name);
    
    /* load all screens into menu structure */
    menu_load_screens(menu, screens);
    
    /* screens are hidden by default. Show the screen specified in start_screen */
    {
        struct ptree_t* start_node = ptree_find_by_key(dom, "start_screen");
        if(start_node && start_node->value)
            menu_set_active_screen(menu, (char*)start_node->value);
        else
            llog(LOG_WARNING, PLUGIN_NAME, 1, "You didn't specify start_screen: \"name\" in your YAML file. Don't know which screen to begin with.");
    }

    /* clean up */
    SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc);

    return menu;
}

void
menu_init_menu(struct menu_t* menu)
{
    memset(menu, 0, sizeof(struct menu_t));
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
    free_string(menu->name);
    FREE(menu);
}

void
menu_set_active_screen(struct menu_t* menu, const char* name)
{
    intptr_t screen_id = hash_jenkins_oaat(name, strlen(name));
    struct screen_t* screen = map_find(&menu->screens, screen_id);
    if(!screen)
        return;

    if(menu->active_screen)
        screen_hide(menu->active_screen);

    screen_show(screen);
    menu->active_screen = screen;
}

static void
menu_load_screens(struct menu_t* menu, const struct ptree_t* screens)
{
    struct map_t created_screen_names;
    map_init_map(&created_screen_names);

    /* iterate screens */
    { PTREE_FOR_EACH(screens, screen_node)
    {
        /* handle screens */
        if(PTREE_HASH_STRING("screen") == screen_node->hash)
        {
            struct screen_t* screen;
            char* screen_name;
            
            /* get screen name */
            {
                struct ptree_t* screen_name_node = ptree_find_by_key(screen_node, "name");
                if(screen_name_node && screen_name_node->value)
                {
                    screen_name = (char*)screen_name_node->value;
                }
                else
                {
                    llog(LOG_WARNING, PLUGIN_NAME, 1, "Screen missing the \"name\" property. Skipping.");
                    continue;
                }
            }

            /* 
            * Screen names must be unique. Verify by searching the map of
            * screens currently registered.
            */
            if(map_key_exists(&created_screen_names, hash_jenkins_oaat(screen_name, strlen(screen_name))))
            {
                llog(LOG_WARNING, PLUGIN_NAME, 2, "Screen with duplicate name found: ", screen_name);
                llog(LOG_WARNING, PLUGIN_NAME, 1, "Screen will not be created");
                continue;
            }
            map_insert(&created_screen_names, hash_jenkins_oaat(screen_name, strlen(screen_name)), NULL);
            
            /* create new screen object */
            screen = screen_create();
            map_insert(&menu->screens, hash_jenkins_oaat(screen_name, strlen(screen_name)), screen);

            /* iterate objects to add to this screen */
            { PTREE_FOR_EACH(screen_node, object_node)
            {
                if(PTREE_HASH_STRING("button") == object_node->hash)
                    menu_load_button(screen, object_node);
            }}
            
            /* hide the screen by default */
            screen_hide(screen);
        }
    }}
    
    map_clear_free(&created_screen_names);
}

static void
menu_load_button(struct screen_t* screen, const struct ptree_t* button_node)
{
    struct button_t* button;

    /* retrieve button parameters required to create a button */
    char* text = NULL;
    const struct ptree_t* text_node   = ptree_find_by_key(button_node, "text");
    const struct ptree_t* x_node      = ptree_find_by_key(button_node, "position.x");
    const struct ptree_t* y_node      = ptree_find_by_key(button_node, "position.y");
    const struct ptree_t* width_node  = ptree_find_by_key(button_node, "size.x");
    const struct ptree_t* height_node = ptree_find_by_key(button_node, "size.y");
    const struct ptree_t* action_node = ptree_find_by_key(button_node, "action");
    if(!x_node || !y_node || !width_node || !height_node)
    {
        llog(LOG_WARNING, PLUGIN_NAME, 1, "Not enough data to create button. Need at least position and size.");
        return;
    }
    if(text_node)
        text = (char*)text_node->value;
    
    /* add button to current screen */
    button = button_create(text,  /* text is allowed to be NULL */
                        atof(x_node->value),
                        atof(y_node->value),
                        atof(width_node->value),
                        atof(height_node->value));
    screen_add_element(screen, (struct element_t*)button);

    /* extract service name and arguments tied to action, if any */
    if(action_node)
        menu_load_button_action(button, action_node);
}

static void
menu_load_button_action(struct button_t* button, const struct ptree_t* action_node)
{
    struct ptree_t* service_node = ptree_find_by_key(action_node, "service");
    if(service_node && service_node->value)
    {
        struct service_t* action_service = service_get((char*)service_node->value);
        if(!action_service)
        {
            llog(LOG_WARNING, PLUGIN_NAME, 3, "Tried to bind button to service \"",
                                (char*)service_node->value,
                                "\", but the service was not found.");
        }
        else
        {
            /*
             * The service exists and can be called. Extract
             * arguments and create compatible argument list.
             */
            struct ptree_t* argv_node;
            struct ordered_vector_t argv;
            char arg_key[sizeof(int)*8+1];
            int action_argc = 0;
            ordered_vector_init_vector(&argv, sizeof(char*));

            /* extract each argument and insert into vector as string */
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
                ordered_vector_push(&argv, &arg_node->value);
                ++action_argc;
            }
            
            /* convert the vector of strings into a vector of arguments */
            button->base.element.action.argv = service_create_argument_list_from_strings(action_service, &argv);
            ordered_vector_clear_free(&argv);
            /* only set service if the arguments were successfully created */
            if(button->base.element.action.argv)
            {
                button->base.element.action.service = action_service;
            }
        }

    }
}

SERVICE(menu_load_wrapper)
{
    intptr_t id;
    SERVICE_EXTRACT_ARGUMENT_PTR(0, file_name, const char*);

    struct menu_t* menu = menu_load(file_name);
    if(!menu)
        SERVICE_RETURN(0, intptr_t);

    /* cannot have duplicate menu names */
    id = hash_jenkins_oaat(menu->name, strlen(menu->name));
    if(map_find(&g_menus, id))
    {
        llog(LOG_ERROR, PLUGIN_NAME, 3, "Tried to load a menu with duplicate name: \"", menu->name, "\"");
        menu_destroy(menu);
        SERVICE_RETURN(0, intptr_t);
    }
    
    map_insert(&g_menus, id, menu);
    SERVICE_RETURN(menu->name, const char*);
}

SERVICE(menu_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT_PTR(0, menu_name, const char*);

    intptr_t id = hash_jenkins_oaat(menu_name, strlen(menu_name));
    struct menu_t* menu = map_erase(&g_menus, id);
    if(menu)
        menu_destroy(menu);
}

SERVICE(menu_set_active_screen_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT_PTR(0, menu_name, const char*);
    SERVICE_EXTRACT_ARGUMENT_PTR(1, screen_name, const char*);
    
    intptr_t menu_id = hash_jenkins_oaat(menu_name, strlen(menu_name));
    struct menu_t* menu = map_find(&g_menus, menu_id);
    if(!menu)
        return;
    
    menu_set_active_screen(menu, screen_name);
}

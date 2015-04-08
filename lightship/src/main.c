#include <stdio.h>
#include "lightship/argv.h"
#include "lightship/init.h"
#include "framework/services.h"

int
main(int argc, char** argv)
{
    struct arg_obj_t* args;

    init();
    args = argv_parse(argc, argv);
    
    if(args->run_game)
    {
        const char* menu_file_name;
        struct menu_t;
        struct menu_t* menu;
        struct service_t* menu_load_service;
        struct service_t* menu_destroy_service;
        
        init_game(args->is_server);
        
        menu_load_service = service_get(g_localhost, "menu.load");
        menu_destroy_service = service_get(g_localhost, "menu.destroy");
#ifdef _DEBUG
        menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
        menu_file_name = "cfg/menu.yml";
#endif
        SERVICE_CALL1(menu_load_service, &menu, PTR(menu_file_name));
        run_game();
        SERVICE_CALL1(menu_destroy_service, SERVICE_NO_RETURN, PTR(menu));
    }
    
    argv_free(args);
    deinit();

    return 0;
}

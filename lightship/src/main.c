#include <stdio.h>
#include "lightship/init.h"
#include "plugin_manager/services.h"

int
main(int argc, char** argv)
{
    puts("=========================================");
    puts("Starting lightship");
    puts("=========================================");

    init();
    /*
    {
        struct menu_t;
        struct menu_t* menu;
        struct service_t* menu_load_service = service_get(g_local_game, "menu.load");
        struct service_t* menu_destroy_service = service_get(g_local_game, "menu.destroy");
#ifdef _DEBUG
        const char* menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
        const char* menu_file_name = "cfg/menu.yml";
#endif
        SERVICE_CALL1(menu_load_service, &menu, PTR(menu_file_name));
        run_game();
        SERVICE_CALL1(menu_destroy_service, SERVICE_NO_RETURN, PTR(menu));
    }*/
    
    run_game();
    
    deinit();

    return 0;
}

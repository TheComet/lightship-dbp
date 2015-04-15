#include <stdio.h>
#include "lightship/argv.h"
#include "lightship/init.h"
#include "framework/services.h"
#include "framework/log.h"
#include "framework/game.h"

int
main(int argc, char** argv)
{
    struct arg_obj_t* args;

    /* init global things */
    init();
    
    /* parse command line arguments */
    args = argv_parse(argc, argv);
    
    /* if the game is set to run, init and run game */
    if(args->run_game)
    {
        const char* menu_file_name;
        struct menu_t;
        struct menu_t* menu;
        
        init_game(args->is_server);
        
#ifdef _DEBUG
        menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
        menu_file_name = "cfg/menu.yml";
#endif
        SERVICE_CALL_NAME1(g_localhost, "menu.load", &menu, PTR(menu_file_name));
        games_run_all();
        /*SERVICE_CALL_NAME1(g_localhost, "menu.destroy", SERVICE_NO_RETURN, PTR(menu));*/
    }
    
    /* clean up */
    argv_free(args);
    deinit();

    return 0;
}

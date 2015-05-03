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
    while(args->run_game)
    {
        if(!init_game(args->is_server))
            break;

        games_run_all();

        break;
    }

    /* clean up */
    argv_free(args);
    deinit();

    return 0;
}

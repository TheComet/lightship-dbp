#include "framework/glob.h"
#include "framework/log.h"
#include "framework/game.h"
#include "util/memory.h"
#include "util/hash.h"
#include <string.h>
#include <assert.h>

uint32_t framework_global_hash = 0;

/* ------------------------------------------------------------------------- */
void
framework_glob_create(struct game_t* game)
{
    struct framework_glob_t* glob;
    
    assert(game);
    
    /* calculate global hash, if required */
    if(framework_global_hash == 0)
    {
        const char* framework_hash_str = "lightship_framework";
        framework_global_hash = hash_jenkins_oaat(framework_hash_str,
                                                strlen(framework_hash_str));
    }

    /* create glob object */
    glob = (struct framework_glob_t*)MALLOC(sizeof *glob);
    if(!glob)
        OUT_OF_MEMORY("[framework] glob_create()", RETURN_NOTHING);
    
    /* init and register glob object with game */
    memset(glob, 0, sizeof *glob);
    glob->game = game;
    game_add_global(game, framework_global_hash, glob);
}

/* ------------------------------------------------------------------------- */
void
framework_glob_destroy(struct game_t* game)
{
    struct glob_t* glob;
    glob = game_remove_global(game, framework_global_hash);
    FREE(glob);
}

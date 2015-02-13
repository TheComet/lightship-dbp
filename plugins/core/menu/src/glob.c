#include "plugin_menu/glob.h"
#include "plugin_menu/config.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/hash.h"
#include <string.h>
#include <assert.h>

uint32_t global_hash = 0;

/* ------------------------------------------------------------------------- */
void
glob_create(struct game_t* game)
{
    struct glob_t* glob;
    
    assert(game);
    assert(!global_hash);
    
    global_hash = hash_jenkins_oaat(PLUGIN_NAME, strlen(PLUGIN_NAME));

    glob = (struct glob_t*)MALLOC(sizeof(struct glob_t));
    if(!glob)
        OUT_OF_MEMORY("[" PLUGIN_NAME "] glob_create()", RETURN_NOTHING);
    memset(glob, 0, sizeof(struct glob_t));
    game_add_global(game, global_hash, glob);
}

/* ------------------------------------------------------------------------- */
void
glob_destroy(struct game_t* game)
{
    struct glob_t* glob;
    glob = game_remove_global(game, global_hash);
    FREE(glob);
    global_hash = 0;
}

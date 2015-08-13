#include "plugin_menu/context.h"
#include "plugin_menu/config.h"
#include "framework/log.h"
#include "util/memory.h"
#include "util/hash.h"
#include <string.h>
#include <assert.h>

uint32_t context_hash = 0;

/* ------------------------------------------------------------------------- */
void
context_create(struct game_t* game)
{
	struct context_t* glob;

	assert(game);
	assert(!context_hash);

	context_hash = hash_jenkins_oaat(PLUGIN_NAME, strlen(PLUGIN_NAME));

	glob = (struct context_t*)MALLOC(sizeof(struct context_t));
	if(!glob)
		OUT_OF_MEMORY("[" PLUGIN_NAME "] context_create()", RETURN_NOTHING);
	memset(glob, 0, sizeof(struct context_t));
	game_add_to_context_store(game, context_hash, glob);

	glob->game = game;
}

/* ------------------------------------------------------------------------- */
void
context_destroy(struct game_t* game)
{
	struct context_t* glob;
	glob = game_remove_from_context_store(game, context_hash);
	FREE(glob);
	context_hash = 0;
}

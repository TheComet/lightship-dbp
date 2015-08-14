#include "plugin_input/context.h"
#include "plugin_input/config.h"
#include "framework/log.h"
#include "util/memory.h"
#include <string.h>
#include <assert.h>

uint32_t context_hash = 0;

/* ------------------------------------------------------------------------- */
void
context_create(struct game_t* game)
{
	struct context_t* context;

	assert(game);

	if(!context_hash)
		context_hash = bsthv_hash_string(PLUGIN_NAME);

	context = (struct context_t*)MALLOC(sizeof(struct context_t));
	if(!context)
		OUT_OF_MEMORY("[" PLUGIN_NAME "] context_create()", RETURN_NOTHING);
	memset(context, 0, sizeof(struct context_t));
	game_add_to_context_store(game, context_hash, context);
}

/* ------------------------------------------------------------------------- */
void
context_destroy(struct game_t* game)
{
	struct context_t* context;
	context = game_remove_from_context_store(game, context_hash);
	FREE(context);
}

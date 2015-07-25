#include "util/bst_hashed_vector.h"
#include "util/hash.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <assert.h>

/* default hash function */
static uint32_t(*g_hash_func)(const char*, uint32_t len) = hash_jenkins_oaat;

/* ------------------------------------------------------------------------- */
void
bsthv_set_string_hash_func(uint32_t(*func)(const char*, uint32_t len))
{
	g_hash_func = func;
}

/* ------------------------------------------------------------------------- */
void
bsthv_restore_default_hash_func(void)
{
	g_hash_func = hash_jenkins_oaat;
}

/* ------------------------------------------------------------------------- */
uint32_t
bsthv_hash_string(const char* str)
{
	return g_hash_func(str, strlen(str));
}

/* ------------------------------------------------------------------------- */
struct bsthv_t*
bsthv_create(void)
{
	struct bsthv_t* bsthv;
	if(!(bsthv = (struct bsthv_t*)MALLOC(sizeof *bsthv)))
		return NULL;
	bsthv_init_bsthv(bsthv);
	return bsthv;
}

/* ------------------------------------------------------------------------- */
void
bsthv_init_bsthv(struct bsthv_t* bsthv)
{
	assert(bsthv);
	ordered_vector_init_vector(&bsthv->vector, sizeof(struct bsthv_key_value_t));
}

/* ------------------------------------------------------------------------- */
void
bsthv_destroy(struct bsthv_t* bsthv)
{
	assert(bsthv);
	bsthv_clear_free(bsthv);
	FREE(bsthv);
}

/* ------------------------------------------------------------------------- */
/* algorithm taken from GNU GCC stdlibc++'s lower_bound function, line 2121 in stl_algo.h */
/* https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a02014.html */
static struct bsthv_key_value_t*
bsthv_find_lower_bound(const struct bsthv_t* bsthv, uint32_t hash)
{
	uint32_t half;
	struct bsthv_key_value_t* middle;
	struct bsthv_key_value_t* data;
	uint32_t len;

	assert(bsthv);

	data = (struct bsthv_key_value_t*)bsthv->vector.data;
	len = bsthv->vector.count;

	/* if the vector has no data, return NULL */
	if(!len)
		return NULL;

	while(len > 0)
	{
		half = len >> 1;
		middle = data + half;
		if(middle->hash < hash)
		{
			data = middle;
			++data;
			len = len - half - 1;
		}
		else
			len = half;
	}

	/* if "data" is pointing outside of the valid elements in the vector, also return NULL */
	if((intptr_t)data >= (intptr_t)bsthv->vector.data + (intptr_t)bsthv->vector.count * (intptr_t)bsthv->vector.element_size)
		return NULL;
	else
		return data;
}

/* ------------------------------------------------------------------------- */
char
bsthv_insert(struct bsthv_t* bsthv, const char* key, void* value)
{
	struct bsthv_key_value_t* new_kv;
	uint32_t hash = bsthv_hash_string(key);

	/* get the lower bound of the insertion point */
	struct bsthv_key_value_t* lower_bound = bsthv_find_lower_bound(bsthv, hash);

	/* hash collision */
	if(lower_bound && lower_bound->hash == hash)
	{
		/*
		 * Get to the end of the chain and make sure no existing keys match the
		 * new key.
		 */
		struct bsthv_value_chain_t* vc = &lower_bound->value_chain;
		do
		{
			/* sanity check - all values in chain must have a key */
			assert(vc->key);

			if(strcmp(key, vc->key) == 0)
				return 0; /* key exists, abort */

		} while(vc->next && (vc = vc->next));

		/* allocate and link a new value at the end of the chain */
		vc->next = (struct bsthv_value_chain_t*)MALLOC(sizeof *vc);
		if(!vc->next)
			return 0;
		memset(vc->next, 0, sizeof *vc->next);
		/* key */
		vc->next->key = malloc_string(key);
		if(!vc->next->key)
		{
			FREE(vc->next);
			return 0;
		}
		/* value */
		vc->next->value = value;

		/* inc counter to keep track of number of elements */
		++bsthv->count;

		return 1;
	}

	/*
	 * No hash collision. Either push back or insert, depending on whether
	 * there is already data in the bsthv.
	 */
	if(!lower_bound)
		new_kv = (struct bsthv_key_value_t*)ordered_vector_push_emplace(&bsthv->vector);
	else
		new_kv = ordered_vector_insert_emplace(&bsthv->vector,
				lower_bound - (struct bsthv_key_value_t*)bsthv->vector.data);
	if(!new_kv)
		return 0;

	memset(new_kv, 0, sizeof *new_kv);
	new_kv->hash = hash;
	new_kv->value_chain.value = value;
	new_kv->value_chain.key = malloc_string(key);
	if(!new_kv->value_chain.key)
	{
		ordered_vector_erase_element(&bsthv->vector, new_kv);
		return 0;
	}

	/* inc counter to keep track of number of elements */
		++bsthv->count;

	return 1;
}

/* ------------------------------------------------------------------------- */
void
bsthv_set(struct bsthv_t* bsthv, const char* key, void* value)
{
	struct bsthv_key_value_t* kv;
	struct bsthv_value_chain_t* vc;
	uint32_t hash;

	assert(bsthv);
	assert(key);

	/*
	 * Compute hash and look up the key-value object. If the returned object
	 * doesn't have the same hash as the computed hash, it means the key
	 * doesn't exist.
	 */
	hash = bsthv_hash_string(key);
	kv = bsthv_find_lower_bound(bsthv, hash);
	if(!kv || kv->hash != hash)
		return;

	/*
	 * If there are no further values in the value chain, this must be the
	 * value.
	 */
	if(!kv->value_chain.next)
	{
		kv->value_chain.value = value;
		return;
	}

	/*
	 * Iterate the value chain and compare each string with the key until it is
	 * found. Then set the value.
	 */
	vc = &kv->value_chain;
	do
	{
		if(strcmp(key, vc->key) == 0)
		{
			vc->value = value;
			return;
		}
		vc = vc->next;
	} while(vc);
}

/* ------------------------------------------------------------------------- */
void*
bsthv_find(const struct bsthv_t* bsthv, const char* key)
{
	struct bsthv_key_value_t* data;
	struct bsthv_value_chain_t* vc;
	uint32_t hash;

	assert(bsthv);

	/*
	 * Compute hash and look up the key-value object. If the returned object
	 * doesn't have the same hash as the computed hash, it means the key
	 * doesn't exist.
	 */
	hash = bsthv_hash_string(key);
	data = bsthv_find_lower_bound(bsthv, hash);
	if(!data || data->hash != hash)
		return NULL;

	/*
	 * If there is only one value in the chain then it must be the value we're
	 * looking for.
	 */
	if(!data->value_chain.next)
		return data->value_chain.value;

	/*
	 * Iterate chain and string compare each key with the key we're looking for
	 * until a matching key is found.
	 */
	vc = &data->value_chain;
	do
	{
		if(strcmp(key, vc->key) == 0)
			return vc->value;

		vc = vc->next;
	} while(vc);

	return NULL;
}

/* ------------------------------------------------------------------------- */
const char*
bsthv_find_element(const struct bsthv_t* bsthv, const void* value)
{
	assert(bsthv);

	ORDERED_VECTOR_FOR_EACH(&bsthv->vector, struct bsthv_key_value_t, kv)
		struct bsthv_value_chain_t* vc = &kv->value_chain;
		do
		{
			if(vc->value == value)
				return vc->key;
			vc = vc->next;
		} while(vc);
	ORDERED_VECTOR_END_EACH

	return NULL;
}

/* ------------------------------------------------------------------------- */
void*
bsthv_get_any_element(const struct bsthv_t* bsthv)
{
	struct bsthv_key_value_t* kv;
	assert(bsthv);
	kv = (struct bsthv_key_value_t*)ordered_vector_back(&bsthv->vector);
	if(kv)
		return kv->value_chain.value;
	return NULL;
}

/* ------------------------------------------------------------------------- */
char
bsthv_key_exists(struct bsthv_t* bsthv, const char* key)
{
	struct bsthv_key_value_t* data;
	struct bsthv_value_chain_t* vc;
	uint32_t hash;

	assert(bsthv);

	/*
	 * Compute hash and look up the key-value object. If the returned object
	 * doesn't have the same hash as the computed hash, it means the key
	 * doesn't exist.
	 */
	hash = bsthv_hash_string(key);
	data = bsthv_find_lower_bound(bsthv, hash);
	if(!data || data->hash != hash)
		return 0;

	/*
	 * Iterate over chain and find the key we're looking for.
	 */
	vc = &data->value_chain;
	do
	{
		if(strcmp(key, vc->key) == 0)
			return 1;
		vc = vc->next;
	} while(vc);

	return 0;
}

/* ------------------------------------------------------------------------- */
void*
bsthv_erase(struct bsthv_t* bsthv, const char* key)
{
	struct bsthv_key_value_t* kv;
	uint32_t hash;

	assert(bsthv);
	assert(key);

	/*
	 * Compute hash and look up the key-value object. If the returned object
	 * doesn't have the same hash as the computed hash, it means the key
	 * doesn't exist.
	 */
	hash = bsthv_hash_string(key);
	kv = bsthv_find_lower_bound(bsthv, hash);
	if(!kv || kv->hash != hash)
		return NULL;

	/* kv object exists and is valid. Remove it */
	return bsthv_erase_key_value_object(bsthv, key, kv);
}

/* ------------------------------------------------------------------------- */
void*
bsthv_erase_key_value_object(struct bsthv_t* bsthv,
							 const char* key,
							 struct bsthv_key_value_t* kv)
{
	struct bsthv_value_chain_t* vc;
	struct bsthv_value_chain_t* parent_vc;

	if(!kv->value_chain.next)
	{
		void* value = kv->value_chain.value;
		free_string(kv->value_chain.key);
		ordered_vector_erase_element(&bsthv->vector, kv);
		--bsthv->count;
		return value;
	}

	/*
	 * Special case: If the first value in the chain has the key we're looking
	 * for, it must be erased from the vector (and if there are any further
	 * values down the chain, the next value must be copied into the vector and
	 * deallocated to maintain the structure).
	 */
	vc = &kv->value_chain;
	if(strcmp(key, kv->value_chain.key) == 0)
	{
		void* value = vc->value;
		free_string(vc->key);
		memcpy(vc, vc->next, sizeof *vc); /* copies the next value into the bsthv's internal vector */
		--bsthv->count;
		return value;
	}

	/*
	 * Iterate chain and find a value with a matching key. When it is found,
	 * unlink it from the chain and free it.
	 */
	parent_vc = vc; /* required for unlinking */
	vc = vc->next;  /* next will always exist, or we wouldn't be here */
	do
	{
		if(strcmp(key, vc->key) == 0)
		{
			void* value = vc->value;
			free_string(vc->key);
			parent_vc->next = vc->next; /* unlink this value by linking next with parent */
			FREE(vc);
			--bsthv->count;
			return value;
		}
		vc = vc->next;
	} while(vc);

	return NULL;
}

/* ------------------------------------------------------------------------- */
void*
bsthv_erase_element(struct bsthv_t* bsthv, void* value)
{
	const char* key;

	assert(bsthv);

	if(!(key = bsthv_find_element(bsthv, value)))
		return NULL;

	bsthv_erase(bsthv, key);

	return value;
}

/* ------------------------------------------------------------------------- */
static void
bsthv_free_all_chains_and_keys(struct bsthv_t* bsthv)
{
	assert(bsthv);
	ORDERED_VECTOR_FOR_EACH(&bsthv->vector, struct bsthv_key_value_t, kv)
		struct bsthv_value_chain_t* vc = kv->value_chain.next;

		free_string(kv->value_chain.key);

		while(vc)
		{
			struct bsthv_value_chain_t* to_free = vc;
			vc = vc->next;
			free_string(to_free->key);
			FREE(to_free);
		}
	ORDERED_VECTOR_END_EACH
}

/* ------------------------------------------------------------------------- */
void
bsthv_clear(struct bsthv_t* bsthv)
{
	assert(bsthv);
	bsthv_free_all_chains_and_keys(bsthv);
	ordered_vector_clear(&bsthv->vector);
}

/* ------------------------------------------------------------------------- */
void bsthv_clear_free(struct bsthv_t* bsthv)
{
	assert(bsthv);
	bsthv_free_all_chains_and_keys(bsthv);
	ordered_vector_clear_free(&bsthv->vector);
}


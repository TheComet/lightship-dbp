#include "util/map.h"
#include "util/memory.h"
#include "util/string.h"
#include <assert.h>
#include <string.h>

const uint32_t MAP_INVALID_KEY = (uint32_t)-1;

uint32_t(*g_hash_func)(const char*, uint32_t len);

/* ------------------------------------------------------------------------- */
void
map_set_string_hash_func(uint32_t(*func)(const char*, uint32_t len))
{
	g_hash_func = func;
}

/* ------------------------------------------------------------------------- */
uint32_t
map_hash_string(const char* str)
{
	return g_hash_func(str, strlen(str));
}

/* ------------------------------------------------------------------------- */
struct map_t*
map_create(void)
{
	struct map_t* map;
	if(!(map = (struct map_t*)MALLOC(sizeof *map)))
		return NULL;
	map_init_map(map);
	return map;
}

/* ------------------------------------------------------------------------- */
void
map_init_map(struct map_t* map)
{
	assert(map);
	ordered_vector_init_vector(&map->vector, sizeof(struct map_key_value_t));
}

/* ------------------------------------------------------------------------- */
void
map_destroy(struct map_t* map)
{
	assert(map);
	map_clear_free(map);
	FREE(map);
}

/* ------------------------------------------------------------------------- */
static struct map_key_value_t*
map_find_lower_bound(const struct map_t* map, uint32_t hash)
{
	uint32_t half;
	struct map_key_value_t* middle;
	struct map_key_value_t* data;
	uint32_t len;

	assert(map);

	data = (struct map_key_value_t*)map->vector.data;
	len = map->vector.count;

	/* if the vector has no data, return NULL */
	if(!len)
		return NULL;

	/* algorithm taken from GNU GCC stdlibc++'s lower_bound function, line 2121 in stl_algo.h */
	/* https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a02014.html */
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
	if((intptr_t)data >= (intptr_t)map->vector.data + (intptr_t)map->vector.count * (intptr_t)map->vector.element_size)
		return NULL;
	else
		return data;
}

/* ------------------------------------------------------------------------- */
void*
map_find(const struct map_t* map, uint32_t hash)
{
	struct map_key_value_t* data;

	assert(map);

	data = map_find_lower_bound(map, hash);
	if(!data || data->hash != hash)
		return NULL;
	return data->value;
}

/* ------------------------------------------------------------------------- */
uint32_t
map_find_element(const struct map_t* map, const void* value)
{
	assert(map);

	ORDERED_VECTOR_FOR_EACH(&map->vector, struct map_key_value_t, kv)
		if(kv->value == value)
			return kv->hash;
	ORDERED_VECTOR_END_EACH
	return MAP_INVALID_KEY;
}

/* ------------------------------------------------------------------------- */
void*
map_get_any_element(const struct map_t* map)
{
	struct map_key_value_t* kv;
	assert(map);
	kv = (struct map_key_value_t*)ordered_vector_back(&map->vector);
	if(kv)
		return kv->value;
	return NULL;
}

/* ------------------------------------------------------------------------- */
char
map_key_exists(struct map_t* map, uint32_t hash)
{
	struct map_key_value_t* data;

	assert(map);

	data = map_find_lower_bound(map, hash);
	if(data && data->hash == hash)
		return 1;
	return 0;
}

/* ------------------------------------------------------------------------- */
uint32_t
map_find_unused_key(struct map_t* map)
{
	uint32_t i = 0;

	assert(map);

	MAP_FOR_EACH(map, void, key, value)
		if(i != key)
			break;
		++i;
	MAP_END_EACH
	return i;
}

/* ------------------------------------------------------------------------- */
char
map_insert_using_hash(struct map_t* map, uint32_t hash, void* value)
{
	struct map_key_value_t* emplaced_data;
	struct map_key_value_t* lower_bound;

	assert(map);

	/* don't insert reserved hashes */
	if(hash == MAP_INVALID_KEY)
		return 0;

	/* lookup location in map to insert */
	lower_bound = map_find_lower_bound(map, hash);
	if(lower_bound && lower_bound->hash == hash)
		return 0;

	/* either push back or insert, depending on whether there is already data
	 * in the map */
	if(!lower_bound)
		emplaced_data = (struct map_key_value_t*)ordered_vector_push_emplace(&map->vector);
	else
		emplaced_data = ordered_vector_insert_emplace(&map->vector,
						  lower_bound - (struct map_key_value_t*)map->vector.data);

	if(!emplaced_data)
		return 0;

	memset(emplaced_data, 0, sizeof *emplaced_data);
	emplaced_data->hash = hash;
	emplaced_data->value_chain.value = value;

	return 1;
}

/* ------------------------------------------------------------------------- */
char
map_insert_using_key(struct map_t* map, const char* key, void* value)
{
	struct map_key_value_t* new_kv;
	uint32_t hash = map_hash_string(key);

	/* don't insert reserved hashes */
	if(hash == MAP_INVALID_KEY)
		return 0;

	/* get the lower bound of the insertion point */
	struct map_key_value_t* lower_bound = map_find_lower_bound(map, hash);

	/* hash collision */
	if(lower_bound && lower_bound->hash == hash)
	{
		/* get to the end of the chain */
		struct map_value_chain_t* vc = &lower_bound->value_chain;
		assert(vc->key); /* sanity check - all values in chain must have a key */
		while(vc->next && (vc = vc->next))
		{
			assert(vc->key); /* sanity check, same as above */
		}

		/* allocate and link a new value chain */
		vc->next = new_kv = (struct map_value_chain_t*)MALLOC(sizeof *vc);
	}
	else
	{
		/*
		 * No hash collision, either push back or insert, depending on whether
		 * there is already data in the map
		 */
		if(!lower_bound)
			new_kv = (struct map_key_value_t*)ordered_vector_push_emplace(&map->vector);
		else
			new_kv = ordered_vector_insert_emplace(&map->vector,
					lower_bound - (struct map_key_value_t*)map->vector.data);
	}

	memset(new_kv, 0, sizeof *new_kv);
	new_kv->hash = hash;
	new_kv->value_chain.key = malloc_string(key);
	new_kv->value_chain.value = value;
}

/* ------------------------------------------------------------------------- */
void
map_set(struct map_t* map, uint32_t hash, void* value)
{
	struct map_key_value_t* data;

	assert(map);

	data = map_find_lower_bound(map, hash);
	if(data && data->hash == hash)
		data->value = value;
}

/* ------------------------------------------------------------------------- */
void*
map_erase(struct map_t* map, uint32_t hash)
{
	void* value;
	struct map_key_value_t* data;

	assert(map);

	data = map_find_lower_bound(map, hash);
	if(!data || data->hash != hash)
		return NULL;

	value = data->value;
	ordered_vector_erase_element(&map->vector, (DATA_POINTER_TYPE*)data);
	return value;
}

/* ------------------------------------------------------------------------- */
void*
map_erase_element(struct map_t* map, void* value)
{
	void* data;
	uint32_t hash;

	assert(map);

	hash = map_find_element(map, value);
	if(hash == MAP_INVALID_KEY)
		return NULL;

	data = map_find_lower_bound(map, hash);
	ordered_vector_erase_element(&map->vector, (DATA_POINTER_TYPE*)data);

	return value;
}

/* ------------------------------------------------------------------------- */
void
map_clear(struct map_t* map)
{
	assert(map);
	ordered_vector_clear(&map->vector);
}

/* ------------------------------------------------------------------------- */
void map_clear_free(struct map_t* map)
{
	assert(map);
	ordered_vector_clear_free(&map->vector);
}

/* ------------------------------------------------------------------------- */
#ifdef _DEBUG
#include <stdio.h>
void
map_print(struct map_t* map)
{
	int i = 0;
	ORDERED_VECTOR_FOR_EACH(&map->vector, struct map_key_value_t, item)
		printf("hash: %d, value (ptr): %p\n", item->hash, (void*)item->value);
		i++;
	ORDERED_VECTOR_END_EACH
	printf("items in map: %d\n", i);
}
#endif

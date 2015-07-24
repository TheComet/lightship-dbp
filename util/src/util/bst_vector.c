#include "util/bst_vector.h"
#include "util/memory.h"
#include "util/string.h"
#include <assert.h>
#include <string.h>

const uint32_t BST_VECTOR_INVALID_HASH = (uint32_t)-1;

uint32_t(*g_hash_func)(const char*, uint32_t len);

/* ------------------------------------------------------------------------- */
void
bst_vector_set_string_hash_func(uint32_t(*func)(const char*, uint32_t len))
{
	g_hash_func = func;
}

/* ------------------------------------------------------------------------- */
uint32_t
bst_vector_hash_string(const char* str)
{
	return g_hash_func(str, strlen(str));
}

/* ------------------------------------------------------------------------- */
struct bst_vector_t*
bst_vector_create(void)
{
	struct bst_vector_t* bst_vector;
	if(!(bst_vector = (struct bst_vector_t*)MALLOC(sizeof *bst_vector)))
		return NULL;
	bst_vector_init_bst_vector(bst_vector);
	return bst_vector;
}

/* ------------------------------------------------------------------------- */
void
bst_vector_init_bst_vector(struct bst_vector_t* bst_vector)
{
	assert(bst_vector);
	ordered_vector_init_vector(&bst_vector->vector, sizeof(struct bst_vector_key_value_t));
}

/* ------------------------------------------------------------------------- */
void
bst_vector_destroy(struct bst_vector_t* bst_vector)
{
	assert(bst_vector);
	bst_vector_clear_free(bst_vector);
	FREE(bst_vector);
}

/* ------------------------------------------------------------------------- */
static struct bst_vector_key_value_t*
bst_vector_find_lower_bound(const struct bst_vector_t* bst_vector, uint32_t hash)
{
	uint32_t half;
	struct bst_vector_key_value_t* middle;
	struct bst_vector_key_value_t* data;
	uint32_t len;

	assert(bst_vector);

	data = (struct bst_vector_key_value_t*)bst_vector->vector.data;
	len = bst_vector->vector.count;

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
	if((intptr_t)data >= (intptr_t)bst_vector->vector.data + (intptr_t)bst_vector->vector.count * (intptr_t)bst_vector->vector.element_size)
		return NULL;
	else
		return data;
}

/* ------------------------------------------------------------------------- */
void*
bst_vector_find(const struct bst_vector_t* bst_vector, uint32_t hash)
{
	struct bst_vector_key_value_t* data;

	assert(bst_vector);

	data = bst_vector_find_lower_bound(bst_vector, hash);
	if(!data || data->hash != hash)
		return NULL;
	return data->value;
}

/* ------------------------------------------------------------------------- */
uint32_t
bst_vector_find_element(const struct bst_vector_t* bst_vector, const void* value)
{
	assert(bst_vector);

	ORDERED_VECTOR_FOR_EACH(&bst_vector->vector, struct bst_vector_key_value_t, kv)
		if(kv->value == value)
			return kv->hash;
	ORDERED_VECTOR_END_EACH
	return BST_VECTOR_INVALID_HASH;
}

/* ------------------------------------------------------------------------- */
void*
bst_vector_get_any_element(const struct bst_vector_t* bst_vector)
{
	struct bst_vector_key_value_t* kv;
	assert(bst_vector);
	kv = (struct bst_vector_key_value_t*)ordered_vector_back(&bst_vector->vector);
	if(kv)
		return kv->value;
	return NULL;
}

/* ------------------------------------------------------------------------- */
char
bst_vector_key_exists(struct bst_vector_t* bst_vector, uint32_t hash)
{
	struct bst_vector_key_value_t* data;

	assert(bst_vector);

	data = bst_vector_find_lower_bound(bst_vector, hash);
	if(data && data->hash == hash)
		return 1;
	return 0;
}

/* ------------------------------------------------------------------------- */
uint32_t
bst_vector_find_unused_key(struct bst_vector_t* bst_vector)
{
	uint32_t i = 0;

	assert(bst_vector);

	MAP_FOR_EACH(bst_vector, void, key, value)
		if(i != key)
			break;
		++i;
	MAP_END_EACH
	return i;
}

/* ------------------------------------------------------------------------- */
char
bst_vector_insert_using_hash(struct bst_vector_t* bst_vector, uint32_t hash, void* value)
{
	struct bst_vector_key_value_t* emplaced_data;
	struct bst_vector_key_value_t* lower_bound;

	assert(bst_vector);

	/* don't insert reserved hashes */
	if(hash == BST_VECTOR_INVALID_HASH)
		return 0;

	/* lookup location in bst_vector to insert */
	lower_bound = bst_vector_find_lower_bound(bst_vector, hash);
	if(lower_bound && lower_bound->hash == hash)
		return 0;

	/* either push back or insert, depending on whether there is already data
	 * in the bst_vector */
	if(!lower_bound)
		emplaced_data = (struct bst_vector_key_value_t*)ordered_vector_push_emplace(&bst_vector->vector);
	else
		emplaced_data = ordered_vector_insert_emplace(&bst_vector->vector,
						  lower_bound - (struct bst_vector_key_value_t*)bst_vector->vector.data);

	if(!emplaced_data)
		return 0;

	memset(emplaced_data, 0, sizeof *emplaced_data);
	emplaced_data->hash = hash;
	emplaced_data->value_chain.value = value;

	return 1;
}

/* ------------------------------------------------------------------------- */
char
bst_vector_insert_using_key(struct bst_vector_t* bst_vector, const char* key, void* value)
{
	struct bst_vector_key_value_t* new_kv;
	uint32_t hash = bst_vector_hash_string(key);

	/* don't insert reserved hashes */
	if(hash == BST_VECTOR_INVALID_HASH)
		return 0;

	/* get the lower bound of the insertion point */
	struct bst_vector_key_value_t* lower_bound = bst_vector_find_lower_bound(bst_vector, hash);

	/* hash collision */
	if(lower_bound && lower_bound->hash == hash)
	{
		/* get to the end of the chain */
		struct bst_vector_value_chain_t* vc = &lower_bound->value_chain;
		assert(vc->key); /* sanity check - all values in chain must have a key */
		while(vc->next && (vc = vc->next))
		{
			assert(vc->key); /* sanity check, same as above */
		}

		/* allocate and link a new value chain */
		vc->next = new_kv = (struct bst_vector_value_chain_t*)MALLOC(sizeof *vc);
	}
	else
	{
		/*
		 * No hash collision, either push back or insert, depending on whether
		 * there is already data in the bst_vector
		 */
		if(!lower_bound)
			new_kv = (struct bst_vector_key_value_t*)ordered_vector_push_emplace(&bst_vector->vector);
		else
			new_kv = ordered_vector_insert_emplace(&bst_vector->vector,
					lower_bound - (struct bst_vector_key_value_t*)bst_vector->vector.data);
	}

	memset(new_kv, 0, sizeof *new_kv);
	new_kv->hash = hash;
	new_kv->value_chain.key = malloc_string(key);
	new_kv->value_chain.value = value;
}

/* ------------------------------------------------------------------------- */
void
bst_vector_set_using_key(struct bst_vector_t* bst_vector, const char* key, void* value)
{
	struct bst_vector_key_value_t* kv;
	struct bst_vector_value_chain_t* vc;
	uint32_t hash;

	assert(bst_vector);
	assert(key);

	/*
	 * Compute hash and look up the key-value object. If the returned object
	 * doesn't have the same hash as the computed hash, it means the key
	 * doesn't exist.
	 */
	hash = bst_vector_hash_string(key);
	kv = bst_vector_find_lower_bound(bst_vector, hash);
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
void
bst_vector_set_using_hash(struct bst_vector_t* bst_vector, uint32_t hash, void* value)
{
	struct bst_vector_key_value_t* data;

	assert(bst_vector);

	data = bst_vector_find_lower_bound(bst_vector, hash);
	if(data && data->hash == hash)
		data->value_chain.value = value;
}

/* ------------------------------------------------------------------------- */
void*
bst_vector_erase_using_key(struct bst_vector_t* bst_vector, const char* key)
{
	struct bst_vector_key_value_t* kv;
	struct bst_vector_value_chain_t* vc;
	struct bst_vector_value_chain_t* parent_vc;
	uint32_t hash;

	assert(bst_vector);
	assert(key);

	/*
	 * Compute hash and look up the key-value object. If the returned object
	 * doesn't have the same hash as the computed hash, it means the key
	 * doesn't exist.
	 */
	hash = bst_vector_hash_string(key);
	kv = bst_vector_find_lower_bound(bst_vector, hash);
	if(!kv || kv->hash != hash)
		return NULL;

	if(!kv->value_chain.next)
	{
		void* value = kv->value_chain.value;
		ordered_vector_erase_element(&bst_vector->vector, (DATA_POINTER_TYPE)kv);
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
		memcpy(vc, vc->next, sizeof *vc); /* copies the next value into the bst_vector's internal vector */
		return value;
	}

	/*
	 * Iterate chain and find a value with a matching key. When it is found,
	 * unlink it from the chain and free it.
	 */
	parent_vc = vc;
	vc = vc->next; /* next will always exist, or we wouldn't be here */
	do
	{
		if(strcmp(key, vc->key) == 0)
		{
			void* value = vc->value;
			free_string(vc->key);
			parent_vc->next = vc->next; /* unlink this value by linking next with parent */
			FREE(vc);
			return value;
		}
	} while(vc);
}

/* ------------------------------------------------------------------------- */
void*
bst_vector_erase_using_hash(struct bst_vector_t* bst_vector, uint32_t hash)
{
	void* value;
	struct bst_vector_key_value_t* data;

	assert(bst_vector);

	data = bst_vector_find_lower_bound(bst_vector, hash);
	if(!data || data->hash != hash)
		return NULL;

	value = data->value_chain.value;
	ordered_vector_erase_element(&bst_vector->vector, (DATA_POINTER_TYPE*)data);
	return value;
}

/* ------------------------------------------------------------------------- */
void*
bst_vector_erase_element(struct bst_vector_t* bst_vector, void* value)
{
	void* data;
	uint32_t hash;

	assert(bst_vector);

	hash = bst_vector_find_element(bst_vector, value);
	if(hash == BST_VECTOR_INVALID_HASH)
		return NULL;

	data = bst_vector_find_lower_bound(bst_vector, hash);
	ordered_vector_erase_element(&bst_vector->vector, (DATA_POINTER_TYPE*)data);

	return value;
}

/* ------------------------------------------------------------------------- */
static void
bst_vector_free_all_chains(struct bst_vector_t* bst_vector)
{
	assert(bst_vector);
	ORDERED_VECTOR_FOR_EACH(&bst_vector->vector, struct bst_vector_key_value_t, kv)
		struct bst_vector_value_chain_t* vc = kv->value_chain.next;
		while(vc)
		{
			struct bst_vector_value_chain_t* to_free = vc;
			vc = vc->next;
			FREE(to_free);
		}
	ORDERED_VECTOR_END_EACH
}

/* ------------------------------------------------------------------------- */
void
bst_vector_clear(struct bst_vector_t* bst_vector)
{
	assert(bst_vector);
	bst_vector_free_all_chains(bst_vector);
	ordered_vector_clear(&bst_vector->vector);
}

/* ------------------------------------------------------------------------- */
void bst_vector_clear_free(struct bst_vector_t* bst_vector)
{
	assert(bst_vector);
	bst_vector_free_all_chains(bst_vector);
	ordered_vector_clear_free(&bst_vector->vector);
}

/* ------------------------------------------------------------------------- */
#ifdef _DEBUG
#include <stdio.h>
void
bst_vector_print(struct bst_vector_t* bst_vector)
{
	int i = 0;
	ORDERED_VECTOR_FOR_EACH(&bst_vector->vector, struct bst_vector_key_value_t, item)
		printf("hash: %d, value (ptr): %p\n", item->hash, (void*)item->value);
		i++;
	ORDERED_VECTOR_END_EACH
	printf("items in bst_vector: %d\n", i);
}
#endif

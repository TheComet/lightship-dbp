/*!
 * @file bst_hashed_vector.h
 * @brief Implements a container of ordered hash-value pairs stored in a vector
 * (ordered by a hash).
 */

#ifndef LIGHTSHIP_UTIL_BST_HASHED_VECTOR_H
#define LIGHTSHIP_UTIL_BST_HASHED_VECTOR_H

#include "util/config.h"
#include "util/ordered_vector.h"

C_HEADER_BEGIN

extern const uint32_t MAP_INVALID_KEY;

struct bsthv_value_chain_t
{
	char*                       key;
	void*                       value;
	struct bsthv_value_chain_t* next;
};

struct bsthv_key_value_t
{
	uint32_t                    hash;
	struct bsthv_value_chain_t  value_chain;
};

struct bsthv_t
{
	struct ordered_vector_t     vector;
	uint32_t                    count;
};

/*!
 * @brief Sets the function to use for computing the hash value of keys.
 * @param func The callback function to use. Must return a uint32_t and accept
 * the parameters "key" (string) and "len" (length of the string).
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_set_string_hash_func(uint32_t(*func)(const char*, uint32_t len));

/*!
 * @brief Restores the default hash function used to compute the hash value of
 * keys.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_restore_default_hash_func(void);

/*!
 * @brief Computes the hash of a given string.
 * @return The hash value of the string.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
bsthv_hash_string(const char* str);

/*!
 * @brief Creates a new bsthv object.
 * @return Returns the newly created bsthv object. It must be freed with
 * bst_vector_destroy() when no longer required.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct bsthv_t*
bsthv_create(void);

/*!
 * @brief Initialises an existing bsthv object.
 * @note This does **not** FREE existing elements. If you have elements in your
 * bsthv and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] bsthv The bsthv object to initialise.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_init_bsthv(struct bsthv_t* bsthv);

/*!
 * @brief Destroys an existing bsthv object and FREEs the underlying memory.
 * @note Elements inserted into the bsthv are not FREEd.
 * @param[in] bsthv The bsthv object to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_destroy(struct bsthv_t* bsthv);

/*!
 * @brief Inserts an element into the bsthv by using a string as a key.
 * @note Hash collisions are resolved when using this function.
 * @note Complexity is O(log2(n)) to find the insertion point.
 * @warning It is highly discouraged to mix bst_vector_insert_hash() and bst_vector_insert().
 * Use one or the other.
 * @param[in] bsthv A pointer to the bsthv object to insert into.
 * @param[in] key A unique string to assign to the element being inserted. The
 * string must not exist in the bsthv or the element will not be inserted.
 * @param[in] value The data to insert into the bsthv.
 * @note The value is **not** copied into the bsthv, only referenced. For this
 * reason don't insert stack allocated items into the bsthv.
 * @return Returns 1 if insertion was successful, 0 if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
bsthv_insert(struct bsthv_t* bsthv, const char* key, void* value);

/*!
 * @brief Sets the value bsthvped to the specified hash in the bsthv.
 * @note If the hash is not found, this function silently fails.
 * @param[in] bsthv A pointer to the bsthv object to change the value of.
 * @param[in] hash The unique key associated with the value you want to change.
 * @param[in] value The new value to set.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_set(struct bsthv_t* bsthv, const char* key, void* value);

/*!
 * @brief Looks for an element in the bsthv and returns it if found.
 * @note Complexity is O(log2(n)).
 * @param[in] bsthv The bsthv to search in.
 * @param[in] hash The hash to search for.
 * @return Returns the data associated with the specified hash. If the hash is
 * not found in the bsthv, then NULL is returned.
 * @note Potential pitfall: The value could be NULL even if the hash was found,
 * as NULL is a valid thing for a value to be. If you are checking to see if a
 * hash exists, use bsthv_key_exists() instead.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
bsthv_find(const struct bsthv_t* bsthv, const char* key);

/*!
 * @brief Finds the specified element in the bsthv and returns its key.
 * @note Complexity is O(n).
 * @param[in] bsthv The bsthv to search.
 * @param[in] value The value to search for.
 * @return Returns the key if it was successfully found, or MAP_INVALID_KEY if
 * otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API const char*
bsthv_find_element(const struct bsthv_t* bsthv, const void* value);

/*!
 * @brief Gets any element from the bsthv.
 *
 * This is useful when you want to iterate and remove all items from the bsthv
 * at the same time.
 * @return Returns an element as a void pointer. Which element is random.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
bsthv_get_any_element(const struct bsthv_t* bsthv);

/*!
 * @brief Returns 1 if the specified hash exists, 0 if otherwise.
 * @param bsthv The bsthv to find the hash in.
 * @param hash The hash to search for.
 * @return 1 if the hash was found, 0 if the hash was not found.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
bsthv_key_exists(struct bsthv_t* bsthv, const char* key);

/*!
 * @brief Erases an element from the bsthv using a key.
 * @note Complexity is O(log2(n))
 * @param[in] bsthv The bsthv to erase from.
 * @param[in] key The key that maps to the element to be removed from the bsthv.
 * @return Returns the data assocated with the specified key. If the key is
 * not found in the bsthv, NULL is returned.
 * @note The bsthv only holds references to values and does **not** FREE them.
 * It is up to the programmer to correctly free the elements being erased from
 * the bsthv.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
bsthv_erase(struct bsthv_t* bsthv, const char* key);

LIGHTSHIP_UTIL_PUBLIC_API void*
bsthv_erase_element(struct bsthv_t* bsthv, void* value);

LIGHTSHIP_UTIL_PUBLIC_API void*
bsthv_erase_key_value_object(struct bsthv_t* bsthv,
							 const char* key,
							 struct bsthv_key_value_t* kv);

/*!
 * @brief Erases the entire bsthv, including the underlying memory.
 * @note This does **not** FREE existing elements. If you have elements in your
 * bsthv and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] bsthv The bsthv to clear.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_clear(struct bsthv_t* bsthv);

LIGHTSHIP_UTIL_PUBLIC_API void
bsthv_clear_free(struct bsthv_t* bsthv);

/*!
 * @brief Returns the number of elements in the specified bsthv.
 * @param[in] bsthv The bsthv to count the elements of.
 * @return The number of elements in the specified bsthv.
 */
#define bsthv_count(bsthv) ((bsthv)->vector.count)

/*!
 * @brief Iterates over the specified bsthv's elements and opens a FOR_EACH
 * scope.
 * @param[in] bsthv The bsthv to iterate.
 * @param[in] var_type The type of data being held in the bsthv.
 * @param[in] var The name to give the variable pointing to the current
 * element.
 */
#define BSTHV_FOR_EACH(bsthv_v, var_t, key_v, var_v) {                                          \
	uint32_t i_##var_v;                                                                        \
	struct bsthv_key_value_t* kv_##var_v; \
	var_t* var_v;                                                                                       \
	for(i_##var_v = 0;                                                                                   \
		i_##var_v != bsthv_count(bsthv_v) &&                                                                 \
			(kv_##var_v = (((struct bsthv_key_value_t*)(bsthv_v)->vector.data) + i_##var_v));       \
		++i_##var_v) { \
		struct bsthv_value_chain_t* vc_##var_v = &(kv_##var_v)->value_chain; \
		const char* key_v; \
		for(; vc_##var_v && \
			((key_v = (vc_##var_v)->key) || 1) && \
			((var_v = (var_t*)(vc_##var_v)->value) || 1); \
		    vc_##var_v = (vc_##var_v)->next) {

/*!
 * @brief Closes a for each scope previously opened by BSTV_FOR_EACH.
 */
#define BSTHV_END_EACH }}}

#define BSTHV_ERASE_CURRENT_ITEM_IN_FOR_LOOP(bsthv_v, key_v, var_v) do { \
	bsthv_erase_key_value_object(bsthv_v, key_v, ((struct bsthv_key_value_t*)(bsthv_v)->vector.data) + i_##var_v); \
	--i_##var_v; } while(0)

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_BST_HASHED_VECTOR_H */

/*!
 * @file bst_vector.h
 * @brief Implements a container of ordered key-value pairs stored in a vector
 * (ordered by key). The hash is computed from a key (string) provided by the
 * user.
 */

#ifndef LIGHTSHIP_UTIL_BST_VECTOR_H
#define LIGHTSHIP_UTIL_BST_VECTOR_H

#include "util/pstdint.h"
#include "util/config.h"
#include "util/ordered_vector.h"

C_HEADER_BEGIN

struct bstv_key_value_t
{
	uint32_t hash;
	void*    value;
};

struct bstv_t
{
	struct ordered_vector_t   vector;
};

/*!
 * @brief Creates a new bstv object.
 * @return Returns the newly created bstv object. It must be freed with
 * bstv_destroy() when no longer required.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct bstv_t*
bstv_create(void);

/*!
 * @brief Initialises an existing bstv object.
 * @note This does **not** FREE existing elements. If you have elements in your
 * bstv and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] bstv The bstv object to initialise.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bstv_init_bstv(struct bstv_t* bstv);

/*!
 * @brief Destroys an existing bstv object and FREEs the underlying memory.
 * @note Elements inserted into the bstv are not FREEd.
 * @param[in] bstv The bstv object to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bstv_destroy(struct bstv_t* bstv);

/*!
 * @brief Inserts an element into the bstv using a hashed key.
 *
 * @warning There is no way to test for hash collisions since this function
 * doesn't have access to the key which generated the hash. It is highly
 * discouraged to mix bstv_insert_hash() and bstv_insert(). Use one or the other.
 *
 * @note Complexity is O(log2(n)) to find the insertion point.
 *
 * @param[in] bstv The bstv object to insert into.
 * @param[in] hash A unique key to assign to the element being inserted. The
 * key must not exist in the bstv, or the element will not be inserted.
 * @param[in] value The data to insert into the bstv.
 * @note The value is **not** copied into the bstv, only referenced. For this
 * reason don't insert stack allocated items into the bstv.
 * @return Returns 1 if insertion was successful, 0 if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
bstv_insert(struct bstv_t* bstv, uint32_t hash, void* value);

/*!
 * @brief Sets the value bstvped to the specified hash in the bstv.
 * @note If the hash is not found, this function silently fails.
 * @param[in] bstv A pointer to the bstv object to change the value of.
 * @param[in] hash The unique key associated with the value you want to change.
 * @param[in] value The new value to set.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bstv_set(struct bstv_t* bstv, uint32_t hash, void* value);

/*!
 * @brief Looks for an element in the bstv and returns it if found.
 * @note Complexity is O(log2(n)).
 * @param[in] bstv The bstv to search in.
 * @param[in] hash The hash to search for.
 * @return Returns the data associated with the specified hash. If the hash is
 * not found in the bstv, then NULL is returned.
 * @note Potential pitfall: The value could be NULL even if the hash was found,
 * as NULL is a valid thing for a value to be. If you are checking to see if a
 * hash exists, use bstv_key_exists() instead.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
bstv_find(const struct bstv_t* bstv, uint32_t hash);

/*!
 * @brief Finds the specified element in the bstv and returns its key.
 * @note Complexity is O(n).
 * @param[in] bstv The bstv to search.
 * @param[in] value The value to search for.
 * @return Returns the key if it was successfully found, or MAP_INVALID_KEY if
 * otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
bstv_find_element(const struct bstv_t* bstv, const void* value);

/*!
 * @brief Gets any element from the bstv.
 *
 * This is useful when you want to iterate and remove all items from the bstv
 * at the same time.
 * @return Returns an element as a void pointer. Which element is random.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
bstv_get_any_element(const struct bstv_t* bstv);

/*!
 * @brief Returns 1 if the specified hash exists, 0 if otherwise.
 * @param bstv The bstv to find the hash in.
 * @param hash The hash to search for.
 * @return 1 if the hash was found, 0 if the hash was not found.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
bstv_hash_exists(struct bstv_t* bstv, uint32_t hash);

/*!
 * @brief Returns a hash that does not yet exist in the bstv.
 * @note Complexity is O(n)
 * @param[in] bstv The bstv to generate a hash from.
 * @return Returns a hash that does not yet exist in the bstv.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
bstv_find_unused_hash(struct bstv_t* bstv);

/*!
 * @brief Erases an element from the bstv using a hash.
 * @warning It is highly discouraged to mix bstv_erase_using_hash() and
 * bstv_erase_using_key(). Use bstv_erase_using_hash() if you used
 * bstv_insert_using_hash(). Use bstv_erase_using_key() if you used
 * bstv_insert_using_key().
 * @note Complexity is O(log2(n))
 * @param[in] bstv The bstv to erase from.
 * @param[in] hash The hash that bstvs to the element to remove from the bstv.
 * @return Returns the data assocated with the specified hash. If the hash is
 * not found in the bstv, NULL is returned.
 * @note The bstv only holds references to values and does **not** FREE them. It
 * is up to the programmer to correctly free the elements being erased from the
 * bstv.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
bstv_erase(struct bstv_t* bstv, uint32_t hash);

LIGHTSHIP_UTIL_PUBLIC_API void*
bstv_erase_element(struct bstv_t* bstv, void* value);

/*!
 * @brief Erases the entire bstv, including the underlying memory.
 * @note This does **not** FREE existing elements. If you have elements in your
 * bstv and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] bstv The bstv to clear.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
bstv_clear(struct bstv_t* bstv);

LIGHTSHIP_UTIL_PUBLIC_API void
bstv_clear_free(struct bstv_t* bstv);

/*!
 * @brief Returns the number of elements in the specified bstv.
 * @param[in] bstv The bstv to count the elements of.
 * @return The number of elements in the specified bstv.
 */
#define bstv_count(bstv) ((bstv)->vector.count)

/*!
 * @brief Iterates over the specified bstv's elements and opens a FOR_EACH
 * scope.
 * @param[in] bstv The bstv to iterate.
 * @param[in] var_type The type of data being held in the bstv.
 * @param[in] var The name to give the variable pointing to the current
 * element.
 */
#define BSTV_FOR_EACH(bstv, var_type, hash_n, var) {                                                                 \
	uint32_t bstv_internal_##var_i;                                                                                  \
	uint32_t hash_n;                                                                                                 \
	var_type* var;                                                                                                   \
	for(bstv_internal_##var_i = 0;                                                                                   \
		bstv_internal_##var_i != bstv_count(bstv) &&                                                                 \
			((hash_n = ((struct bstv_key_value_t*) (bstv)->vector.data)[bstv_internal_##var_i].hash) || 1) &&        \
			((var  = (var_type*)((struct bstv_key_value_t*)(bstv)->vector.data)[bstv_internal_##var_i].value) || 1); \
		++bstv_internal_##var_i) {

/*!
 * @brief Closes a for each scope previously opened by BSTV_FOR_EACH.
 */
#define BSTV_END_EACH }}

/*!
 * @brief Will erase the current selected item in a for loop from the bstv.
 * @note This does not free the data being referenced by the bstv. You will have
 * to erase that manually (either before or after this operation, it doesn't
 * matter).
 * @param[in] bstv A pointer to the bstv object currently being iterated.
 */
#define BSTV_ERASE_CURRENT_ITEM_IN_FOR_LOOP(bstv)  \
	ordered_vector_erase_element(&(bstv)->vector, ((struct bstv_key_value_t*)(bstv)->vector.data) + bstv_internal_##var_i); \
	--bstv_internal_##var_i;

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_BST_VECTOR_H */

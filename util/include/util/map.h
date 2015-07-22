#ifndef LIGHTSHIP_UTIL_MAP_H
#define LIGHTSHIP_UTIL_MAP_H

#include "util/pstdint.h"
#include "util/config.h"
#include "util/ordered_vector.h"

C_HEADER_BEGIN

extern const uint32_t MAP_INVALID_KEY;

struct map_key_value_t
{
	void* value;
	uint32_t hash;
};

struct map_t
{
	struct ordered_vector_t vector;
};

/*!
 * @brief Creates a new map object.
 * @return Returns the newly created map object. It must be freed with
 * map_destroy() when no longer required.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct map_t*
map_create(void);

/*!
 * @brief Initialises an existing map object.
 * @note This does **not** FREE existing elements. If you have elements in your
 * map and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] map The map object to initialise.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
map_init_map(struct map_t* map);

/*!
 * @brief Destroys an existing map object and FREEs the underlying memory.
 * @note Elements inserted into the map are not FREEd.
 * @param[in] map The map object to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
map_destroy(struct map_t* map);

/*!
 * @brief Inserts an element into the map.
 * @note Complexity is O(log2(n)) to find the insertion point.
 * @param[in] map The map object to insert into.
 * @param[in] hash The key to assign to the element being inserted. The key
 * must not exist in the map, or the element will not be inserted.
 * @param[in] value The data to insert into the map.
 * @note The value is **not** copied into the map, only referenced. For this
 * reason, don't insert stack allocated items into the map.
 * @return Returns 1 if insertion was successful, 0 if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
map_insert(struct map_t* map, uint32_t hash, void* value);

/*!
 * @brief Sets the value of the specified key in the map.
 * @note If the key is not found, this function silently fails.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
map_set(struct map_t* map, uint32_t hash, void* value);

/*!
 * @brief Looks for an element in the map and returns it if found.
 * @note Complexity is O(log2(n)).
 * @param[in] map The map to search in.
 * @param[in] hash The key to search for.
 * @return Returns the data associated with the specified key. If the key is
 * not found in the map, then NULL is returned.
 * @note Potential pitfall: The value could be NULL even if the key was found,
 * as NULL is a valid thing for a value to be. If you are checking to see if a
 * key exists, use map_key_exists() instead.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
map_find(const struct map_t* map, uint32_t hash);

/*!
 * @brief Finds the specified element in the map and returns its key.
 * @note Complexity is O(n).
 * @param[in] map The map to search.
 * @param[in] value The value to search for.
 * @return Returns the key if it was successfully found, or MAP_INVALID_KEY if
 * otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
map_find_element(const struct map_t* map, const void* value);

/*!
 * @brief Gets any element from the map.
 *
 * This is useful when you want to iterate and remove all items from the map
 * at the same time.
 * @return Returns an element as a void pointer. Which element is random.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
map_get_any_element(const struct map_t* map);

/*!
 * @brief Returns 1 if the specified key exists, 0 if otherwise.
 * @param map The map to find the key in.
 * @param key The key to search for.
 * @return 1 if the key was found, 0 if the key was not found.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
map_key_exists(struct map_t* map, uint32_t hash);

/*!
 * @brief Returns a key that does not yet exist in the map.
 * @note Complexity is O(n)
 * @param[in] map The map to generate a key from.
 * @return Returns a key that does not yet exist in the map.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
map_find_unused_key(struct map_t* map);

/*!
 * @brief Erases the specified element from the map.
 * @note Complexity is O(log2(n))
 * @param[in] map The map to erase from.
 * @param[in] hash The key to erase from the map.
 * @return Returns the data assocated with the specified key. If the key is
 * not found in the map, NULL is returned.
 * @note The map only holds references to values and does **not** FREE them. It
 * is up to the programmer to correctly free the elements being erased from the
 * map.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
map_erase(struct map_t* map, uint32_t hash);

LIGHTSHIP_UTIL_PUBLIC_API void*
map_erase_element(struct map_t* map, void* value);

/*!
 * @brief Erases the entire map, including the underlying memory.
 * @note This does **not** FREE existing elements. If you have elements in your
 * map and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] map The map to clear.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
map_clear(struct map_t* map);

LIGHTSHIP_UTIL_PUBLIC_API void
map_clear_free(struct map_t* map);

/*!
 * @brief Returns the number of elements in the specified map.
 * @param[in] map The map to count the elements of.
 * @return The number of elements in the specified map.
 */
#define map_count(map) ((map)->vector.count)

/*!
 * @brief Dumps the contents of the specified map to stdout.
 * @note This is for debugging purposes only.
 * @param[in] map The map to dumpl
 */
#ifdef _DEBUG
void
map_print(struct map_t* map);
#endif

/*!
 * @brief Iterates over the specified map's elements and opens a FOR_EACH
 * scope.
 * @param[in] map The map to iterate.
 * @param[in] var_type The type of data being held in the map.
 * @param[in] var The name to give the variable pointing to the current
 * element.
 */
#define MAP_FOR_EACH(map, var_type, hash_n, var) {                                                                \
	uint32_t map_internal_##var_i;                                                                                \
	uint32_t hash_n;                                                                                              \
	var_type* var;                                                                                                \
	for(map_internal_##var_i = 0;                                                                                 \
		map_internal_##var_i != (map)->vector.count &&                                                            \
			((hash_n = ((struct map_key_value_t*) (map)->vector.data)[map_internal_##var_i].hash) || 1) &&        \
			((var  = (var_type*)((struct map_key_value_t*)(map)->vector.data)[map_internal_##var_i].value) || 1); \
		++map_internal_##var_i) {

/*!
 * @brief Closes a for each scope previously opened by MAP_FOR_EACH.
 */
#define MAP_END_EACH }}

/*!
 * @brief Will erase the current selected item in a for loop from the map.
 * @note This does not free the data being referenced by the map. You will have
 * to erase that manually (either before or after this operation, it doesn't
 * matter).
 * @param[in] map A pointer to the map object currently being iterated.
 */
#define MAP_ERASE_CURRENT_ITEM_IN_FOR_LOOP(map) \
	ordered_vector_erase_element(&(map)->vector, &((struct map_key_value_t*)(map)->vector.data)[map_internal_##var_i]); \
	--map_internal_##var_i;

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_MAP_H */

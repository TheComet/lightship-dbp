#ifndef LIGHTSHIP_UTIL_MAP_H
#define LIGHTSHIP_UTIL_MAP_H

#include "util/pstdint.h"
#include "util/config.h"
#include "util/ordered_vector.h"

C_HEADER_BEGIN

struct map_key_value_t
{
    intptr_t hash;
    void* value;
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
LIGHTSHIP_PUBLIC_API struct map_t*
map_create(void);

/*!
 * @brief Initialises an existing map object.
 * @note This does **not** FREE existing elements. If you have elements in your
 * map and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] map The map object to initialise.
 */
LIGHTSHIP_PUBLIC_API void
map_init_map(struct map_t* map);

/*!
 * @brief Destroys an existing map object and FREEs the underlying memory.
 * @note Elements inserted into the map are not FREEd.
 * @param[in] map The map object to destroy.
 */
LIGHTSHIP_PUBLIC_API void
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
 */
LIGHTSHIP_PUBLIC_API void
map_insert(struct map_t* map, intptr_t hash, void* value);

/*!
 * @brief Looks for an element in the map and returns it if found.
 * @note Complexity is O(log2(n)).
 * @param[in] map The map to search in.
 * @param[in] hash The key to search for.
 * @return Returns the data associated with the specified key. If the key is
 * not found in the map, then NULL is returned.
 */
LIGHTSHIP_PUBLIC_API void*
map_find(struct map_t* map, intptr_t hash);

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
LIGHTSHIP_PUBLIC_API void*
map_erase(struct map_t* map, intptr_t hash);

/*!
 * @brief Erases the entire map, including the underlying memory.
 * @note This does **not** FREE existing elements. If you have elements in your
 * map and call this, those elements will be lost and a memory leak will have
 * been created.
 * @param[in] map The map to clear.
 */
LIGHTSHIP_PUBLIC_API void
map_clear(struct map_t* map);

/*!
 * @brief Returns the number of elements in the specified map.
 * @param[in] map The map to count the elements of.
 * @return The number of elements in the specified map.
 */
LIGHTSHIP_PUBLIC_API intptr_t
map_count(struct map_t* map);

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
 * @brief Iterates over the specified map's elements.
 * @param[in] map The map to iterate.
 * @param[in] var_type The type of data being held in the map.
 * @param[in] var The name to give the variable pointing to the current
 * element.
 */
#define MAP_FOR_EACH(map, var_type, var) \
    intptr_t map_internal_i; \
    var_type* var; \
    for(map_internal_i = 0, var = ((struct map_key_value_t*)(map)->vector.data)[map_internal_i].value; \
        map_internal_i != (map)->vector.count; \
        ++map_internal_i, var = ((struct map_key_value_t*)(map)->vector.data)[map_internal_i].value)

#define MAP_ERASE_IN_FOR_LOOP(map, element) \
    ordered_vector_erase_element((map)->vector, element);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_MAP_H */

#ifndef LIGHTSHIP_UTIL_VECTOR_H
#define LIGHTSHIP_UTIL_VECTOR_H

#include "util/pstdint.h"
#include "util/config.h"

#define DATA_POINTER_TYPE unsigned char
struct unordered_vector_t
{
    intptr_t element_size;       /* how large one element is in bytes */
    intptr_t capacity;           /* how many elements actually fit into the allocated space */
    intptr_t count;              /* number of elements inserted */
    DATA_POINTER_TYPE* data;/* pointer to the contiguous section of memory */
};

/*!
 * @brief Creates a new vector object.
 * @param[in] element_size Specifies the size in bytes of the type of data you want
 * the vector to store. Typically one would pass sizeof(my_data_type).
 * @return Returns the newly created vector object.
 */
LIGHTSHIP_PUBLIC_API struct unordered_vector_t* unordered_vector_create(const intptr_t element_size);

/*!
 * @brief Initialises an existing vector object.
 * @note This does **not** FREE existing memory. If you've pushed elements
 * into your vector and call this, you will have created a memory leak.
 * @param[in] vector The vector to initialise.
 * @param[in] element_size Specifies the size in bytes of the type of data you
 * want the vector to store. Typically one would pass sizeof(my_data_type).
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_init_vector(struct unordered_vector_t* vector, const intptr_t element_size);

/*!
 * @brief Destroys an existing vector object and FREEs all memory allocated by
 * inserted elements.
 * @param[in] vector The vector to destroy.
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_destroy(struct unordered_vector_t* vector);

/*!
 * @brief Erases all elements in a vector.
 * @note This does not actually erase the underlying memory, it simply resets
 * the element counter. If you wish to FREE the underlying memory, see
 * unordered_vector_clear_FREE().
 * @param[in] vector The vector to clear.
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_clear(struct unordered_vector_t* vector);

/*!
 * @brief Erases all elements in a vector and FREEs their memory.
 * @param[in] vector The vector to clear.
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_clear_free(struct unordered_vector_t* vector);

/*!
 * @brief Gets the number of elements that have been inserted into the vector.
 */
#define unordered_vector_count(x) ((x)->count)

/*!
 * @brief Inserts (copies) a new element at the head of the vector.
 * @note This can cause a re-allocation of the underlying memory. This
 * implementation expands the allocated memory by a factor of 2 every time a
 * re-allocation occurs to cut down on the frequency of re-allocations.
 * @note If you do not wish to copy data into the vector, but merely make
 * space, see unordered_vector_push_emplace().
 * @param[in] vector The vector to push into.
 * @param[in] data The data to copy into the vector. It is assumed that
 * sizeof(data) is equal to what was specified when the vector was first
 * created. If this is not the case then it could cause undefined behaviour.
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_push(struct unordered_vector_t* vector, void* data);

/*!
 * @brief Allocates space for a new element at the head of the vector, but does
 * not initialise it.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param[in] vector The vector to emplace an element into.
 * @return A pointer to the allocated memory for the requested element. See
 * warning and use with caution.
 */
LIGHTSHIP_PUBLIC_API void* unordered_vector_push_emplace(struct unordered_vector_t* vector);

/*!
 * @brief Removes an element from the head of the vector.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param[in] vector The vector to pop an element from.
 * @return A pointer to the popped element. See warning and use with caution.
 * If there are no elements to pop, NULL is returned.
 */
LIGHTSHIP_PUBLIC_API void* unordered_vector_pop(struct unordered_vector_t* vector);

/*!
 * @brief Erases the specified element from the vector.
 * @note This causes all elements with indices greater than **index** to be
 * re-allocated (shifted 1 element down) so the vector remains contiguous.
 * @param[in] index The position of the element in the vector to erase. The index
 * ranges from **0** to **unordered_vector_count()-1**.
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_erase_index(struct unordered_vector_t* vector, intptr_t index);

/*!
 * @brief Removes the element in the vector pointed to by **element**.
 * @param[in] vector The vector from which to erase the data.
 * @param[in] element A pointer to an element within the vector.
 */
LIGHTSHIP_PUBLIC_API void unordered_vector_erase_element(struct unordered_vector_t* vector, void* element);

/*!
 * @brief Gets a pointer to the specified element in the vector.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param[in] vector The vector to get the element from.
 * @param[in] index The index of the element to get. The index ranges from
 * **0** to **unordered_vector_count()-1**.
 * @return [in] A pointer to the element. See warning and use with caution.
 * If the specified element doesn't exist (index out of bounds), NULL is
 * returned.
 */
LIGHTSHIP_PUBLIC_API void* unordered_vector_get_element(struct unordered_vector_t*, intptr_t index);

/*!
 * @brief Convenient macro for iterating a vector's elements.
 * 
 * Example:
 * @code
 * unordered_vector_t* someVector = (a vector containing elements of type "struct bar")
 * LIST_FOR_EACH(someList, struct bar, element)
 * {
 *     do_something_with(element);  ("element" is now of type "struct bar*")
 * }
 * @endcode
 * @param[in] vector The vector to iterate.
 * @param[in] var_type Should be the type of data stored in the vector.
 * @param[in] var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define UNORDERED_VECTOR_FOR_EACH(vector, var_type, var) \
    var_type* var; \
    DATA_POINTER_TYPE* end_of_vector = (vector)->data + (vector)->count * (vector)->element_size; \
    for(var = (var_type*)(vector)->data; \
        (DATA_POINTER_TYPE*)var != end_of_vector; \
        var = (var_type*)(((DATA_POINTER_TYPE*)var) + (vector)->element_size))

#define UNORDERED_VECTOR_FOR_EACH_ERASE(vector, var_type, var) \
    var_type* var; \
    for(var = (vector)->data; \
        (DATA_POINTER_TYPE*)var < (vector)->data + (vector)->count * (vector)->element_size; \
        var = ((DATA_POINTER_TYPE*)var) + (vector)->element_size)

#endif /* LIGHTSHIP_UTIL_VECTOR_H */

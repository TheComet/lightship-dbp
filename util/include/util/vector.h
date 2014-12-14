#define DATA_POINTER_TYPE unsigned char*
struct vector_t
{
    int element_size;       /* how large one element is in bytes */
    int capacity;           /* how many elements actually fit into the allocated space */
    int count;              /* number of elements inserted */
    DATA_POINTER_TYPE data; /* pointer to the contiguous section of memory */
};

/*!
 * @brief Creates a new vector object.
 * @param element_size Specifies the size in bytes of the type of data you want
 * the vector to store. Typically one would pass sizeof(my_data_type).
 * @return Returns the newly created vector object.
 */
struct vector_t* vector_create(const int element_size);

/*!
 * @brief Initialises an existing vector object.
 * @note This does **not** free existing memory. If you've pushed elements
 * into your vector and call this, you will have created a memory leak.
 * @param vector The vector to initialise.
 * @param element_size Specifies the size in bytes of the type of data you want
 * the vector to store. Typically one would pass sizeof(my_data_type).
 */
void vector_init_vector(struct vector_t* vector, const int element_size);

/*!
 * @brief Destroys an existing vector object and frees all memory allocated by
 * inserted elements.
 * @param vector The vector to destroy.
 */
void vector_destroy(struct vector_t* vector);

/*!
 * @brief Erases all elements in a vector.
 * @note This does not actually erase the underlying memory, it simply resets
 * the element counter. If you wish to free the underlying memory, see
 * vector_clear_free().
 * @param vector The vector to clear.
 */
void vector_clear(struct vector_t* vector);

/*!
 * @brief Erases all elements in a vector and frees their memory.
 * @param vector The vector to clear.
 */
void vector_clear_free(struct vector_t* vector);

/*!
 * @brief Gets the number of elements that have been inserted into the vector.
 */
#define vector_count(x) ((x)->count)

/*!
 * @brief Inserts (copies) a new element at the head of the vector.
 * @note This can cause a re-allocation of the underlying memory. This
 * implementation expands the allocated memory by a factor of 2 every time a
 * re-allocation occurs to cut down on the frequency of re-allocations.
 * @note If you do not wish to copy data into the vector, but merely make
 * space, see vector_push_emplace().
 * @param vector The vector to push into.
 * @param data The data to copy into the vector. It is assumed that
 * sizeof(data) is equal to what was specified when the vector was first
 * created. If this is not the case then it could cause undefined behaviour.
 */
void vector_push(struct vector_t* vector, void* data);

/*!
 * @brief Allocates space for a new element at the head of the vector, but does
 * not initialise it.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param vector The vector to emplace an element into.
 * @return A pointer to the allocated memory for the requested element. See
 * warning and use with caution.
 */
void* vector_push_emplace(struct vector_t* vector);

/*!
 * @brief Removes an element from the head of the vector.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param vector The vector to pop an element from.
 * @return A pointer to the popped element. See warning and use with caution.
 * If there are no elements to pop, NULL is returned.
 */
void* vector_pop(struct vector_t* vector);

/*!
 * @brief Inserts (copies) a new element into the middle of the vector.
 * @note This can cause a re-allocation of the underlying memory. This
 * implementation expands the allocated memory by a factor of 2 every time a
 * re-allocation occurs to cut down on the frequency of re-allocations.
 * @param vector The vector to insert into.
 * @param index The position in the vector to insert into. The index ranges
 * from **0** to **vector_count()-1**.
 * @param data The data to copy into the vector. It is assumed that
 * sizeof(data) is equal to what was specified when the vector was first
 * created. If this is not the case then it could cause undefined behaviour.
 */
void vector_insert(struct vector_t* vector, int index, void* data);

/*!
 * @brief Erases the specified element from the vector.
 * @note This causes all elements with indices greater than **index** to be
 * re-allocated (shifted 1 element down) so the vector remains contiguous.
 * @param index The position of the element in the vector to erase. The index
 * ranges from **0** to **vector_count()-1**.
 */
void vector_erase(struct vector_t* vector, int index);

/*!
 * @brief Gets a pointer to the specified element in the vector.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param vector The vector to get the element from.
 * @param index The index of the element to get. The index
 * ranges from **0** to **vector_count()-1**.
 * @return A pointer to the element. See warning and use with caution.
 * If the specified element doesn't exist (index out of bounds), NULL is
 * returned.
 */
void* vector_get_element(struct vector_t*, int index);

/*!
 * @brief Expands the underlying memory.
 * 
 * This implementation will expand the memory by a factor of 2 each time this
 * is called. All elements are copied into the new section of memory.
 * @param insertion_index Set to -1 if no space should be made for element
 * insertion. Otherwise this parameter specifies the index of the element to
 * "evade" when re-allocating all other elements.
 */
static void vector_expand(struct vector_t* vector, int insertion_index);

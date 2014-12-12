/*!
 * @brief Holds user defined data and information on linked nodes.
 */
struct list_node_t
{
    struct list_node_t* prev;
    struct list_node_t* next;
    void* data;
};

/*!
 * @brief Holds information on the entire list of nodes.
 */
struct list_t
{
    int count;
    struct list_node_t* head;
    struct list_node_t* tail;
};

/*!
 * @brief Creates a new, empty list.
 * @return A pointer to the new list.
 */
struct list_t* list_create(void);

/*!
 * @brief Initialises an existing list with default values.
 * @param list The list to initialise.
 */
void list_init_list(struct list_t* list);

/*!
 * @brief Destroys a list.
 * @param list The list to destroy.
 */
void list_destroy(struct list_t* list);

/*!
 * @brief Unlinks and removes all nodes in a list. The list will be empty after
 * this operation.
 * @note The data each link held is not freed, it is up to you to traverse the
 * list and destroy any data contained within the list before clearing it.
 */
void list_clear(struct list_t* list);

/*!
 * @brief How many nodes the list has.
 */
#define list_count(x) ((x)->count)

/*!
 * @brief Returns the current head data in the list.
 * @param list The list to get the head data from.
 * @return NULL if the list is empty, otherwise a void* pointer pointing to the
 * data held by the head node.
 */
#define list_head_data(x) ((x)->head != NULL ? (x)->head->data : NULL)

/*!
 * @brief Returns the current tail data in the list.
 * @param list The list to get the tail data from.
 * @return NULL if the list is empty, otherwise a void* pointer pointing to the
 * data held by the head node.
 */
#define list_tail_data(x) ((x)->tail != NULL ? (x)->tail->data : NULL)

/*!
 * @brief Adds a new node to the head of the list.
 * 
 * Creates a new node and links it into the specified list. The new node
 * becomes the head node of the list. The specified data is referenced by
 * the new node.
 * @note The data is **not** copied into the list, it is referenced. Therefore,
 * the life of the object being pointed to must outlive the life of the node
 * in the list.
 * @param list The list to add a new node to.
 * @param data The data the new node should reference.
 */
void list_push(struct list_t* list, void* data);

/*!
 * @brief Removes a node from the head of the list.
 * 
 * Removes the head node from the list, if any. The head of the list will point
 * to the item preceeding the removed item, or if there is no preceeding item,
 * the head of the list will point to NULL.
 * @note The data being referenced by the node is **not** freed.
 * @param list The list from which to remove the node.
 * @return Returns the data that was referenced by the now destroyed node.
 */
void* list_pop(struct list_t* list);

/*!
 * @brief Removes a specified node from the list.
 * @note The data being referenced by the node is **not** freed.
 * @param list The list from which to remove the node.
 * @return Returns the data that was referenced by the now destroyed node.
 */
void* list_erase_node(struct list_t* list, struct list_node_t* node);

/*!
 * @brief Searches the list for the specified data, then erases the node.
 * @note The data being referenced by the node is **not** freed.
 * @param list The list from which to remove the node.
 * @return Returns the data that was referenced by the now destroyed node.
 * If the specified data is not found in the list, NULL is returned.
 */
void* list_erase_element(struct list_t* list, void* data);

/*!
 * @brief Convenient macro for iterating a list's elements.
 * 
 * Example:
 * @code
 * LIST_FOR_EACH(my_list, my_data_type*, element_name)
 * {
 *     do_things_with_element(element_name);
 * }
 * @endcode
 * @param list Should be of type list_t*.
 * @param var_type Should be the type of data you're storing in each node.
 * @param var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define LIST_FOR_EACH(list, var_type, var) \
    var_type var; \
	struct list_node_t* node; \
    for(node = (list)->tail; node != NULL && (var = node->data); node = node->next)

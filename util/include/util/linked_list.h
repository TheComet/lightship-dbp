#ifndef LIGHTSHIP_UTIL_LINKED_LIST_H
#define LIGHTSHIP_UTIL_LINKED_LIST_H

#include "util/config.h"

C_HEADER_BEGIN

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
LIGHTSHIP_UTIL_PUBLIC_API struct list_t*
list_create(void);

/*!
 * @brief Initialises an existing list with default values.
 * @param[in] list The list to initialise. Must be a valid list (cannot be
 * NULL).
 */
LIGHTSHIP_UTIL_PUBLIC_API void
list_init_list(struct list_t* list);

/*!
 * @brief Destroys a list.
 * @note The data each link holds is **not** de-allocated, it is up to you to
 * traverse the list and destroy any data contained within the list before
 * clearing it.
 * @param[in] list The list to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
list_destroy(struct list_t* list);

/*!
 * @brief Unlinks and removes all nodes in a list. The list will be empty after
 * this operation.
 * @note The data each link holds is **not** de-allocated, it is up to you to
 * traverse the list and destroy any data contained within the list before
 * clearing it.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
list_clear(struct list_t* list);

/*!
 * @brief How many nodes the list has.
 */
#define list_count(x) ((x)->count)

/*!
 * @brief Returns the current head data in the list.
 * @param[in] list The list to get the head data from.
 * @return NULL if the list is empty, otherwise a void* pointer pointing to the
 * data held by the head node.
 */
#define list_head_data(x) ((x)->head != NULL ? (x)->head->data : NULL)

/*!
 * @brief Returns the current tail data in the list.
 * @param[in] list The list to get the tail data from.
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
 * @param[in] list The list to add a new node to.
 * @param[in] data The data the new node should reference.
 * @return Returns non-zero if the item was successfully inserted, zero if
 * otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
list_push(struct list_t* list, void* data);

/*!
 * @brief Removes a node from the head of the list.
 *
 * Removes the head node from the list, if any. The head of the list will point
 * to the item preceeding the removed item, or if there is no preceeding item,
 * the head of the list will point to NULL.
 * @note The data being referenced by the node is **not** de-allocated.
 * @param[in] list The list from which to remove the node.
 * @return Returns the data that was referenced by the now destroyed node.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
list_pop(struct list_t* list);

/*!
 * @brief Removes a specified node from the list.
 * @note The data being referenced by the node is **not** de-allocated.
 * @param[in] list The list from which to remove the node.
 * @return Returns the data that was referenced by the now destroyed node.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
list_erase_node(struct list_t* list, struct list_node_t* node);

/*!
 * @brief Searches the list for the specified data, then erases the node.
 * @note The data being referenced by the node is **not** de-allocated.
 * @param[in] list The list from which to remove the node.
 * @return Returns 1 if the node was found and destroyed. Returns 0 if the data
 * was not found.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
list_erase_element(struct list_t* list, void* data);

/*!
 * @brief Convenient macro for iterating a list's elements in forward order.
 * @note It is **unsafe** to erase the current element from the list.
 *
 * Example:
 * @code
 * list_t* someList = (a list containing elements of type "struct bar")
 * LIST_FOR_EACH(someList, struct bar, element)
 * {
 *     do_something_with(element);  ("element" is now of type "struct bar*")
 * }
 * @endcode
 * @param[in] list Should be of type list_t*.
 * @param[in] var_type Should be the type of data you're storing in each node.
 * @param[in] var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define LIST_FOR_EACH(list, var_type, var) \
    var_type* var; \
    struct list_node_t* node_##var; \
    for(node_##var = (list)->tail; node_##var != NULL && (var = (var_type*)node_##var->data); node_##var = node_##var->next)

/*!
 * @brief Convenient macro for iterating a list's elements in reverse order.
 * @note It is **unsafe** to erase the current element from the list.
 *
 * Example:
 * @code
 * list_t* someList = (a list containing elements of type "struct bar")
 * LIST_FOR_EACH_R(someList, struct bar, element)
 * {
 *     do_something_with(element);  ("element" is now of type "struct bar*")
 * }
 * @endcode
 * @param[in] list Should be of type list_t*.
 * @param[in] var_type Should be the type of data you're storing in each node.
 * @param[in] var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define LIST_FOR_EACH_R(list, var_type, var) \
    var_type* var; \
    struct list_node_t* node_##var; \
    for(node_##var = (list)->head; node_##var != NULL && (var = (var_type*)node_##var->data); node_##var = node_##var->prev)

/*!
 * @brief Convenient macro for iterating a list's elements in forward order.
 * @note It is safe to erase the current element from the list when using this
 * form of iteration.
 *
 * Example:
 * @code
 * list_t* someList = (a list containing elements of type "struct bar")
 * LIST_FOR_EACH_ERASE(someList, struct bar, element)
 * {
 *     do_something_with(element);  ("element" is now of type "struct bar*")
 * }
 * @endcode
 * @param[in] list Should be of type list_t*.
 * @param[in] var_type Should be the type of data you're storing in each node.
 * @param[in] var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define LIST_FOR_EACH_ERASE(list, var_type, var) \
    var_type* var; \
    struct list_node_t* node_##var; \
    struct list_node_t* next_node_##var; \
    for(node_##var = (list)->tail; node_##var && ((var = (var_type*)node_##var->data, next_node_##var = node_##var->next) || 1); node_##var = next_node_##var)
    /*
     * Why ||1 ? -> It is possible that the expression after && evaluates to be
     * false (such is the case when node->data = NULL, or node->prev = NULL).
     * In order to not exit the for-loop in this situation, I've added a ||1 to
     * prevent short circuiting of the expression.
     */

/*!
 * @brief Convenient macro for iterating a list's elements in reverse order.
 * @note It is safe to erase the current element from the list when using this
 * form of iteration.
 *
 * Example:
 * @code
 * list_t* someList = (a list containing elements of type "struct bar")
 * LIST_FOR_EACH_ERASE_R(someList, struct bar, element)
 * {
 *     do_something_with(element);  ("element" is now of type "struct bar*")
 * }
 * @endcode
 * @param[in] list Should be of type list_t*.
 * @param[in] var_type Should be the type of data you're storing in each node.
 * @param[in] var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define LIST_FOR_EACH_ERASE_R(list, var_type, var) \
    var_type* var; \
    struct list_node_t* node_##var; \
    struct list_node_t* prev_node_##var; \
    for(node_##var = (list)->head; node_##var && ((var = (var_type*)node_##var->data, prev_node_##var = node_##var->prev) || 1); node_##var = prev_node_##var)
    /*
     * Why ||1 ? -> It is possible that the expression after && evaluates to be
     * false (such is the case when node->data = NULL, or node->prev = NULL).
     * In order to not exit the for-loop in this situation, I've added a ||1 to
     * prevent short circuiting of the expression.
     */

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_LINKED_LIST_H */

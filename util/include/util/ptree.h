/*!
 * @file ptree.h
 * @brief Property Tree
 *
 * A property tree is a non-balanced tree structure with arbitrary depth. It is
 * most suitable for holding information such as directory structures, XML/YAML
 * data, or similar.
 *
 * The ptree structure can be thought of as follows:
 * ```
 * struct ptree_t
 * {
 *     void* value;     // data associated with this node (can be anything)
 *     map_t children;  // a key-value container of nested ptree_t objects
 * }
 * ```
 */

#include "util/pstdint.h"
#include "util/config.h"
#include "util/hash.h"
#include "util/map.h"

C_HEADER_BEGIN

typedef void* (*ptree_dup_func)(void*);
typedef void (*ptree_free_func)(void*);

struct ptree_t
{
#ifdef _DEBUG
    char* key;
#endif
    void* value;
    struct map_t children;
    struct ptree_t* parent;
    ptree_dup_func dup_value;
    ptree_free_func free_value;
};

/*!
 * @brief Allocates and initialises a new empty ptree object.
 * @param[in] key The key to give the root node.
 * @param[in] value The data for the root node to reference. Can be NULL.
 * @return Returns the root node of a new, empty ptree object.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_create(void* value);

/*!
 * @brief Initialises an allocated ptree object.
 * @note Calling this does **not** delete the ptree. If you call this on a
 * non-empty ptree then all data in the tree will be leaked.
 * @param[in] key The key to give the root node.
 * @param[in] value The data for the root node to reference. Can be NULL.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_init_ptree(struct ptree_t* tree, void* value);

/*!
 * @brief Destroys an existing ptree.
 *
 * Traverses the tree and frees every node.
 * @param[in] tree The tree to destroy.
 * @param[in] do_free_values If set to 1, the data associated with every node
 * will additionally be de-allocated using the specified free function (@see
 * ptree_set_free_func). If the free function was not specified, the data will
 * not be freed, and warning messages will be generated.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_destroy(struct ptree_t* tree, char do_free_values);

/*!
 * @brief Destroys an existing ptree, but keeps the root node.
 *
 * Traverses the tree and frees every node except for the root node (the one
 * being passed to this function).
 * @note If do_free_falues is set to 1, then the data being referenced by the
 * root node is de-allocated anyway.
 * @param[in] tree The three to destroy.
 * @param[in] do_free_values If set to 1, the data associated with every node
 * will additionally be de-allocated using the specified free function (@see
 * ptree_set_free_func). If the free function was not specified, the data will
 * not be freed, and warning messages will be generated.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_destroy_keep_root(struct ptree_t* tree, char do_free_values);

/*!
 * @brief Creates a child node and adds it as a child of the specified root
 * node, and sets its key and data. The key can be in the form of
 * ```path.to.my.node```. If any of the nodes in between the root node and the
 * target node don't exist, they will be created.
 * @param[in] node The node in which to insert the new child node into.
 * @param[in] key The key to give the new child node.
 * @param[in] data The data the child node should reference. Can be NULL.
 * @return Returns the newly created child.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_create_node(struct ptree_t* node, const char* key, void* data);

/*!
 * @brief Sets the parent node, effectively merging a tree into part of another
 * tree.
 * @note This replaces whatever the parent is currently set to - a node cannot
 * have more than one parent. If the parent was non-NULL, then after this
 * function returns, it means this node will no longer be part of the previous
 * tree.
 * @param[in] node The node to merge.
 * @param[in] parent The parent node to set.
 * @param[in] key The key to give the node being merged.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
ptree_insert_node(struct ptree_t* node, struct ptree_t* parent, const char* key);

/*!
 * @brief Finds the root node of the tree, given any node within the tree.
 * @param[in] node Any node part of a tree you wish to find the root of.
 * @return Returns the root node of the tree.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_get_root(const struct ptree_t* node);

/*!
 * @brief Specifies a duplication function for a node's data.
 *
 * In order to be able to copy ptrees, you must specify a duplication callback
 * function for every node that holds data. The function must accept a void*
 * to data and return a void* to a duplicated, separate buffer of the same
 * data. ptree then inserts this new data into the copied tree's node.
 * @param func The duplication callback function.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_set_dup_func(struct ptree_t* node, ptree_dup_func func);

/*!
 * @brief Specifies a memory de-allocation function fit for freeing the data
 * held by the specified node.
 * @param[in] free_func The function to use for freeing the data at this node.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_set_free_func(struct ptree_t* node, ptree_free_func func);

/*!
 * @brief Copies the tree from source_node and returns an identical tree.
 * @warning This **only** works if every node has a duplication function
 * and free function assigned to it. If a node is found referencing data but is
 * unable to duplicate or free the data, copying is aborted entirely.
 * @param source_node The tree to copy.
 * @return Returns the new copied tree if successful, NULL if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_duplicate_tree(const struct ptree_t* source_node);

/*!
 * @brief Copies all children from source and inserts it as children into
 * target.
 * @warning This **only** works if every node has a duplication function
 * and free function assigned to it. If a node is found referencing data but is
 * unable to duplicate or free the data, copying is aborted entirely.
 * @param target The node in which to insert the children into.
 * @param source The node from which to copy the children from.
 * @return Returns 1 if successful, 0 if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
ptree_duplicate_children_into_existing_node(struct ptree_t* target,
                                       const struct ptree_t* source);

/*!
 * @brief Searches only the current node for the specified key.
 * @param[in] node The node to find the key in.
 * @return Returns the node associated with the specified key if the key was
 * found, NULL if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_get_node_in_node(const struct ptree_t* node, const char* key);

/*!
 * @brief Searches recursively for the specified key. The key can be in the
 * form of ```"path.to.my.node```.
 * @param[in] node The node from which to begin the search.
 * @param[in] key The key to search for.
 * @return Returns the node associated with the specified key if the key was
 * found, NULL if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_get_node(const struct ptree_t* node, const char* key);

/*!
 * @brief Searches the tree for the specified node.
 *
 * This is used to eliminate loops when inserting by first checking if the node
 * being inserted is already a child of the target node.
 * @param[in] node The node to search for.
 * @param[in] tree The tree to recursively search in.
 * @return Returns non-zero if the specified node exists in the specified tree.
 * Otherwise, 0 is returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
ptree_node_is_child_of(const struct ptree_t* node,
                       const struct ptree_t* tree);

/*!
 * @brief Prints the tree to stdout for debugging purposes.
 * @param tree The tree to print.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_print(const struct ptree_t* tree);

#define PTREE_FOR_EACH_IN_NODE(tree, hash, node) \
    MAP_FOR_EACH(&(tree)->children, struct ptree_t, hash, node)

#define PTREE_HASH_STRING(str) hash_jenkins_oaat(str, strlen(str))

C_HEADER_END

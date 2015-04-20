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
 *     char* key;       // a string identifying this node
 *     void* value;     // data associated with this node (can be anything)
 *     list_t children; // a nested list of more ptree_t objects
 * }
 * ```
 */

#include "util/pstdint.h"
#include "util/config.h"
#include "util/hash.h"
#include "util/map.h"

typedef void* (*ptree_dup_func)(void*);
typedef void (*ptree_free_func)(void*);

struct ptree_t
{
    struct map_t children;
#ifdef _DEBUG
    char* key;
#endif
    void* value;
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
ptree_create(const char* key, void* value);

/*!
 * @brief Initialises an allocated ptree object.
 * @note Calling this does **not** delete the ptree. If you call this on a
 * non-empty ptree then all data in the tree will be leaked.
 * @param[in] key The key to give the root node.
 * @param[in] value The data for the root node to reference. Can be NULL.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_init_ptree(struct ptree_t* tree, const char* key, void* value);

/*!
 * @brief Destroys an existing ptree.
 * 
 * Traverses the tree and frees every node.
 * @note This does **not** free any data being referenced by the nodes. If you
 * wish for the data being referenced to be freed along with the tree, use
 * ptree_destroy_free().
 * @param[in] tree The tree to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_destroy(struct ptree_t* tree);

/*!
 * @brief Destroys an existing ptree.
 * 
 * Traverses the tree and frees every node.
 * @note This **includes** data being referenced at every node. If you do not
 * wish the data being referenced to be freed along with the tree, use
 * ptree_destroy().
 * @param[in] tree The tree to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
ptree_destroy_free(struct ptree_t* tree);

/*!
 * @brief Adds a child node to the specified node and sets its key and data.
 * @param[in] node The node in which to insert the new child node into.
 * @param[in] key The key to give the new child node.
 * @param data The data the child node should reference. Can be NULL.
 * @return Returns the newly created child.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_add_node(struct ptree_t* node, const char* key, void* data);

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
 * @brief Copies the tree from source_node and inserts it as a child of
 * target_node.
 * @warning This **only** works if every node has a duplication function
 * assigned to it. If a node is found referencing data but is unable to
 * duplicate the data, copying is aborted entirely.
 * @param target_node The node in which to insert the copied tree into as a child.
 * @param source_node The tree to copy.
 * @param key The key to give the root node of the copied tree (now the child
 * of the target node). Set to NULL if you wish to copy the key of the source
 * node instead.
 * @return Returns 1 if successful, 0 if copying failed.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
ptree_duplicate_tree(struct ptree_t* target_node,
                     const struct ptree_t* source_node,
                     const char* key);

/*!
 * @brief Searches only the current node for the specified key.
 * @param[in] node The node to find the key in.
 * @return Returns the node associated with the specified key if the key was
 * found, NULL if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_find_in_node(const struct ptree_t* node, const char* key);

/*!
 * @brief Searches recursively for the specified key.
 * @param[in] node The node from which to begin the search.
 * @param key The key to search for.
 * @return Returns the node associated with the specified key if the key was
 * found, NULL if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct ptree_t*
ptree_find_in_tree(const struct ptree_t* node, const char* key);

LIGHTSHIP_UTIL_PUBLIC_API void
ptree_print(const struct ptree_t* tree);

#define PTREE_FOR_EACH(tree, var) \
    UNORDERED_VECTOR_FOR_EACH(&(tree)->children, struct ptree_t, var)

#define PTREE_HASH_STRING(str) hash_jenkins_oaat(str, strlen(str))

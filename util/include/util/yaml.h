/*!
 * @file yaml.h
 */

#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

struct ptree_t;

struct yaml_doc_t
{
    struct ptree_t* dom;
};

/*!
 * @brief Initialises the yaml parser. This must be called before using any
 * other yaml-related functions.
 */

LIGHTSHIP_UTIL_PUBLIC_API void
yaml_init(void);

/*!
 * @brief De-initialises the yaml parser and cleans up any docs that weren't
 * closed.
 */
void
yaml_deinit(void);

/*!
 * @brief Loads and parses a yaml file.
 * @param filename The file to load.
 * @return Returns a new yaml doc object if successful. If a parser error
 * occurs, or if the file doesn't exist, NULL is returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct yaml_doc_t*
yaml_load(const char* filename);

/*!
 * @brief Parses a string containing YAML code located in memory.
 * @param buffer The string to load YAML from.
 * @return Returns a new yaml doc object if successful. If a parser error
 * occurs, NULL is returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct yaml_doc_t*
yaml_load_from_memory(const char* buffer);

/*!
 * @brief Parses YAML code from a stream.
 * @param stream The stream to load YAML from.
 * @return Returns a new yaml doc object if successful. If a parser error
 * occurs, or if the stream isn't open, NULL is returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct yaml_doc_t*
yaml_load_from_stream(FILE* stream);

/*!
 * @brief Looks up the specified node from a loaded yaml document and retrieves
 * its value.
 *
 * For example, if your YAML file looked like this:
 * ```
 * root:
 *     my_items:
 *         item1: value1
 *         item2: value2
 * ```
 * And you wanted to retrieve *value2*, you would write:
 * ```const char* value = yaml_get_value(doc, "root.my_items.item1");```
 * @param doc The document to search in.
 * @param key The key(s) to search for.
 * @return Returns the value if it was successfully found, otherwise NULL is
 * returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API const char*
yaml_doc_get_value(struct yaml_doc_t* doc, const char* key);

/*!
 * @brief Returns the specified node's hash.
 * @param[in] node The node to get the hash of.
 * @return Returns the hash if it exists. Note that the root node will not
 * have a hash, in which case the value of 0 is returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
yaml_node_get_hash(const struct ptree_t* node);

/*!
 * @brief Returns the specified node's value.
 * @param[in] node The node to get the value of.
 * @return Returns the value as a string. If the value does not exist, NULL is
 * returned (such as is the case with yaml nodes that have children).
 */
LIGHTSHIP_UTIL_PUBLIC_API const char*
yaml_node_get_value(const struct ptree_t* node, const char* key);

/*!
 * @brief Looks up the specified node from a loaded yaml file and returns it.
 *
 * For example, if your YAML file looked like this:
 * ```
 * root:
 *     my_items:
 *         item1: value1
 *         item2: value2
 * ```
 * And you wanted to retrieve the node *item1*, you would write:
 * ```struct ptree_t* item1 = yaml_get_node(doc, "root.my_items.item1");```
 * @param doc The document to search in.
 * @param key The key(s) to search for.
 * @return Returns a property-tree node if the node was successfully found,
 * otherwise NULL is returned.
 */
struct ptree_t*
yaml_get_node(struct yaml_doc_t* doc, const char* key);

/*!
 * @brief Destroys a loaded yaml document.
 * @param doc The document to destroy.
 */
void
yaml_destroy(struct yaml_doc_t* doc);

#define YAML_FOR_EACH(doc, node_name, hash, node) {                     \
    struct ptree_t* yaml_internal_##value_node;                         \
    if((yaml_internal_##value_node = yaml_get_node(doc, node_name))) {  \
        MAP_FOR_EACH(&(yaml_internal_##value_node)->children, struct ptree_t, hash, node) \

#define YAML_END_FOR_EACH }}

#define YAML_END_FOR_EACH_IF_FAILED } else

C_HEADER_END

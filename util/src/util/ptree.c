#include "util/ptree.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <stdio.h>

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

static void
ptree_init_node(struct ptree_t* node, struct ptree_t* parent, void* value);

static struct ptree_t*
ptree_create_node_hashed_key(struct ptree_t* tree, uint32_t hash, void* value);

static void
ptree_destroy_recurse(struct ptree_t* tree, char do_free_value);

static const struct ptree_t*
ptree_find_in_tree_recurse(const struct ptree_t* tree,
                           const char* delim);

static void
ptree_print_impl(const struct ptree_t* tree, uint32_t depth);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
struct ptree_t*
ptree_create(void* value)
{
    struct ptree_t* tree = (struct ptree_t*)MALLOC(sizeof(struct ptree_t));
    ptree_init_ptree(tree, value);
    return tree;
}

/* ------------------------------------------------------------------------- */
void
ptree_init_ptree(struct ptree_t* tree, void* value)
{
    ptree_init_node(tree, NULL, value);
#ifdef _DEBUG
    tree->key = malloc_string("root");
#endif

}

/* ------------------------------------------------------------------------- */
static void
ptree_init_node(struct ptree_t* node, struct ptree_t* parent, void* value)
{
    memset(node, 0, sizeof *node);
    map_init_map(&node->children);
    node->parent = parent;
    node->value = value;
}

/* ------------------------------------------------------------------------- */
void
ptree_destroy(struct ptree_t* tree, char do_free_values)
{
    /* if tree has parent, detach */
    if(tree->parent)
        map_erase_element(&tree->parent->children, tree);

    /* destroy detached node */
    ptree_destroy_recurse(tree, do_free_values);
    FREE(tree);
}

/* ------------------------------------------------------------------------- */
void
ptree_destroy_keep_root(struct ptree_t* tree, char do_free_values)
{
    ptree_destroy_recurse(tree, do_free_values);
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_create_node(struct ptree_t* tree, const char* key, void* value)
{
    struct ptree_t* child =
            ptree_create_node_hashed_key(tree, PTREE_HASH_STRING(key), value);

#ifdef _DEBUG
    child->key = malloc_string(key);
#endif

    return child;
}

/* ------------------------------------------------------------------------- */
static struct ptree_t*
ptree_create_node_hashed_key(struct ptree_t* tree, uint32_t hash, void* value)
{
    struct ptree_t* child = (struct ptree_t*)MALLOC(sizeof(struct ptree_t));
    ptree_init_node(child, tree, value);
    map_insert(&tree->children, hash, child);
    return child;
}

/* ------------------------------------------------------------------------- */
void
ptree_set_dup_func(struct ptree_t* node, ptree_dup_func func)
{
    node->dup_value = func;
}

/* ------------------------------------------------------------------------- */
void
ptree_set_free_func(struct ptree_t* node, ptree_free_func func)
{
    node->free_value = func;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_duplicate_tree(const struct ptree_t* source_node)
{
    /*
     * Create a new root, into which source_node is copied.
     * Note that the value is being set to NULL, but will be overwritten
     * in ptree_duplicate_children_into_existing_node().
     */
    struct ptree_t* new_root = ptree_create(NULL);

    /* try duplicating */
    if(!ptree_duplicate_children_into_existing_node(new_root, source_node))
    {
        /* recursively free all nodes and values and return error */
        ptree_destroy(new_root, 1);
        return NULL;
    }

    return new_root;
}

/* ------------------------------------------------------------------------- */
char
ptree_duplicate_children_into_existing_node(struct ptree_t* target,
                                            const struct ptree_t* source)
{
    /* duplicate source into target */
#ifdef _DEBUG
    if(target->key)
        free_string(target->key);
    target->key = malloc_string(source->key);
#endif
    target->dup_value = source->dup_value;
    target->free_value = source->free_value;
    if(source->value)
    {
        /* duplication function and free functions must exist */
        if(!source->dup_value || !source->free_value)
            return 0;

        /* duplicate value */
        target->value = source->dup_value(source->value);
    }

    /* iterate over all children of source and duplicate them */
    { MAP_FOR_EACH(&source->children, struct ptree_t, key, node)
    {
        struct ptree_t* child;
        child = ptree_create_node_hashed_key(target, key, NULL);
        if(!ptree_duplicate_children_into_existing_node(child, node))
            return 0;
    }}

    return 1;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_find_in_node(const struct ptree_t* tree, const char* key)
{
    return map_find(&tree->children, PTREE_HASH_STRING(key));
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_find_in_tree(const struct ptree_t* tree, const char* key)
{
    /* prepare key for tokenisation */
    const struct ptree_t* result;
    const char* delim = ".";
    char* key_iter = cat_strings(2, "n.", key); /* root key name is ignored, but must exist */
    strtok(key_iter, delim);

    result = ptree_find_in_tree_recurse(tree, delim);
    free_string(key_iter);
    return result;
}

/* ------------------------------------------------------------------------- */
void
ptree_print(const struct ptree_t* tree)
{
    ptree_print_impl(tree, 0);
}

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static void
ptree_destroy_recurse(struct ptree_t* tree, char do_free_value)
{
    /* destroy all children recursively */
    { MAP_FOR_EACH(&tree->children, struct ptree_t, key, child)
    {
        ptree_destroy_recurse(child, do_free_value);
        FREE(child);
    }}
    map_clear_free(&tree->children);

    /* free the data of this node, if specified */
    if(do_free_value && tree->value)
    {
        if(tree->free_value)
            tree->free_value(tree->value);
        else
        {
            fprintf(stderr, "ptree_destroy_recurse(): Unable to de-allocate!"
                " No free() function was specified!");
#ifdef _DEBUG
            fprintf(stderr, " (at ptree node with name \"%s\")", tree->key);
#endif
            fprintf(stderr, "\n");
        }
    }

#ifdef _DEBUG
    if(tree->key)
        free_string(tree->key);
#endif
}

/* ------------------------------------------------------------------------- */
static const struct ptree_t*
ptree_find_in_tree_recurse(const struct ptree_t* tree,
                          const char* delim)
{
    /*
     * Get next token, hash, and search in current node. If the tree is NULL
     * or if there are no more tokens, then we've found the node we're looking
     * for.
     */
    char* token;
    if((token = strtok(NULL, delim)) && tree)
    {
        struct ptree_t* child;
        child = map_find(&tree->children, PTREE_HASH_STRING(token));
        return ptree_find_in_tree_recurse(child, delim);
    } else
        return tree;
}

/* ------------------------------------------------------------------------- */
static void
ptree_print_impl(const struct ptree_t* tree, uint32_t depth)
{
    char* value = "NULL";

    /* indentation */
    uint32_t i;
    for(i = 0; i != depth; ++i)
        printf("    ");

    /* print node info */
    if(tree->value)
        value = tree->value;
#ifdef _DEBUG
    printf("key: \"%s\", val: %s\n", tree->key, value);
#endif

    /* print children */
    {
        MAP_FOR_EACH(&tree->children, struct ptree_t, key, child)
        {
            ptree_print_impl(child, depth+1);
        }
    }
}

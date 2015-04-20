#include "util/ptree.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <stdio.h>

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

static void
ptree_init_ptree_bare(struct ptree_t* tree);

static void
ptree_destroy_recurse(ptree_t* tree, char do_free_value);

static char
ptree_duplicate_tree_recurse(struct ptree_t* target, const struct ptree_t* source);

static struct ptree_t*
ptree_find_in_tree_recurse(const struct ptree_t* tree,
                           const char* delim);

static void
ptree_print_impl(const struct ptree_t* tree, uint32_t depth);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
struct ptree_t*
ptree_create(const char* key, void* value)
{
    struct ptree_t* tree = (struct ptree_t*)MALLOC(sizeof(struct ptree_t));
    ptree_init_ptree(tree, key, value);
    return tree;
}

/* ------------------------------------------------------------------------- */
void
ptree_init_ptree(struct ptree_t* tree, const char* key, void* value)
{
    ptree_init_ptree_bare(tree);
#ifdef _DEBUG
    tree->key = malloc_string(key);
#endif
    tree->value = value;
}

/* ------------------------------------------------------------------------- */
void
ptree_destroy(struct ptree_t* tree)
{
    ptree_destroy_recurse(tree, 0);
    FREE(tree);
}

/* ------------------------------------------------------------------------- */
void
ptree_destroy_free(struct ptree_t* tree)
{
    ptree_destroy_recurse(tree, 1);
    FREE(tree);
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_add_node(struct ptree_t* tree, const char* key, void* value)
{
    struct ptree_t* child = ptree_create(key, value);
    map_insert(&tree->children, PTREE_HASH_STRING(key), child);
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
char
ptree_duplicate_tree(struct ptree_t* target_node,
                     const struct ptree_t* source_node,
                     const char* key)
{
    /* create a new root, into which source_node is copied */
    struct ptree_t* new_root = ptree_add_node(target_node, key, NULL);
    
    /* try duplicating */
    if(!ptree_duplicate_tree_recurse(new_root, source_node))
    {
        /* recursively free all nodes and values and return error */
        ptree_destroy_free(new_root, 1); 
        return NULL;
    }
    
    /* copy was successful, insert the new root into the target node as a
     * child */
    ptree_add_node(target_node, key);

#ifdef _DEBUG
    free_string(child->key);
    child->key = malloc_string(key);
#endif
        new_root->hash = PTREE_HASH_STRING(key);
    }
    return new_root;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_find_in_node(const struct ptree_t* tree, const char* key)
{
    uint32_t hash = PTREE_HASH_STRING(key);
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        if(hash == child->hash)
            return child;
    }
    return NULL;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_find_in_tree(const struct ptree_t* tree, const char* key)
{
    /* prepare key for tokenisation */
    struct ptree_t* result;
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
ptree_init_ptree_bare(struct ptree_t* tree)
{
    memset(tree, 0, sizeof(struct ptree_t));
    unordered_vector_init_vector(&tree->children, sizeof(struct ptree_t));
}

/* ------------------------------------------------------------------------- */
static void
ptree_destroy_recurse(ptree_t* tree, char do_free_value)
{
    /* destroy all children recursively */
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        ptree_destroy_recurse(child, free_func);
    }
    unordered_vector_clear_free(&tree->children);
    
    /* free the data of this node, if specified */
    if(do_free_value && tree->value)
    {
        if(tree->free_value)
            tree->free_value(tree->value);
        else
            fprintf(stderr, "ptree_destroy_recurse(): Unable to de-allocate!"
                "no free() function was specified!");
    }
    
#ifdef _DEBUG
    if(tree->key)
        free_string(tree->key);
#endif
}

/* ------------------------------------------------------------------------- */
static char
ptree_duplicate_tree_recurse(struct ptree_t* target, const struct ptree_t* source)
{
    /* iterate all children of source and duplicate them */
    MAP_FOR_EACH(&source->children, struct ptree_t, key, node)
    {
        struct ptree_t* child;
        child = (struct ptree_t*)unordered_vector_push_emplace(&target->children);
        ptree_init_ptree_bare(child);
        if(!ptree_duplicate_tree_recurse(child, node))
            return 0;
    }

    /* duplicate source root into target */
#ifdef _DEBUG
    target->key = malloc_string(source->key);
#endif
    target->dup_value = source->dup_value;
    target->free_value = source->free_value;
    if(source->value)
    {
        /* unable to duplicate, missing dup function */
        if(!source->dup_value)
            return 0;
        
        /* duplicate value */
        target->value = source->dup_value(source->value);
    }
    
    return 1;
}

/* ------------------------------------------------------------------------- */
static struct ptree_t*
ptree_find_by_key_recurse(const struct ptree_t* tree,
                          const char* delim)
{
    char* token;
    if((token = strtok(NULL, delim)))
    {
        uint32_t hash = PTREE_HASH_STRING(token);
        {
            UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
            {
                if(child->hash == hash)
                    return ptree_find_in_tree_recurse(child, delim);
            }
            return NULL;
        }
    } else
        return *(struct ptree_t**)&tree; /* to get around warning for const qualifiers */
                                         /* same as "return tree;" */
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
    printf("key: \"%s\", hash: %d, val: %s\n", tree->key, tree->hash, value);
#else
    printf("hash: %d, dup: %p\n", tree->hash, value);
#endif
    
    /* print children */
    {
        UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
        {
            ptree_print_impl(child, depth+1);
        }
    }
}

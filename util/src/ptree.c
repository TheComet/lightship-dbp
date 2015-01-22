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
ptree_destroy_recurse(struct ptree_t* tree, char free_value);

static char
ptree_duplicate_tree_recurse(struct ptree_t* target, const struct ptree_t* source);

static const struct ptree_t*
ptree_find_by_key_recurse(const struct ptree_t* tree,
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
    tree->hash = PTREE_HASH_STRING(key);
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
    struct ptree_t* child;
    child = (struct ptree_t*)unordered_vector_push_emplace(&tree->children);
    ptree_init_ptree(child, key, value);
    return child;
}

/* ------------------------------------------------------------------------- */
void
ptree_set_dup_func(struct ptree_t* node, ptree_dup_func func)
{
    node->dup_value = func;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_duplicate_tree(struct ptree_t* target_node, const struct ptree_t* source_node, const char* key)
{
    struct ptree_t* child = (struct ptree_t*)unordered_vector_push_emplace(&target_node->children);
    ptree_init_ptree_bare(child);
    if(!ptree_duplicate_tree_recurse(child, source_node))
    {
        ptree_destroy_recurse(child, 1); /* recursively free all nodes and values except for root node */
        unordered_vector_erase_element(&target_node->children, child);
        return NULL;
    }
    if(key)
    {
#ifdef _DEBUG
        FREE(child->key);
        child->key = malloc_string(key);
#endif
        child->hash = PTREE_HASH_STRING(key);
    }
    return child;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_find_local_by_key(const struct ptree_t* tree, const char* key)
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
const struct ptree_t*
ptree_find_by_key(const struct ptree_t* tree, const char* key)
{
    /* prepare key for tokenisation */
    const char* delim = ".";
    char* key_iter = cat_strings(2, "n.", key); /* root key name is ignored, but must exist */
    strtok(key_iter, delim);
    {
        const struct ptree_t* result = ptree_find_by_key_recurse(tree, delim);
        FREE(key_iter);
        return result;
    }
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
ptree_destroy_recurse(struct ptree_t* tree, char free_value)
{
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        ptree_destroy_recurse(child, free_value);
    }
    unordered_vector_clear_free(&tree->children);
    if(free_value && tree->value)
        FREE(tree->value);
#ifdef _DEBUG
    if(tree->key)
        FREE(tree->key);
#endif
}

/* ------------------------------------------------------------------------- */
static char
ptree_duplicate_tree_recurse(struct ptree_t* target, const struct ptree_t* source)
{
    /* iterate all children of source and duplicate them */
    UNORDERED_VECTOR_FOR_EACH(&source->children, struct ptree_t, node)
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
    target->hash = source->hash;
    target->dup_value = source->dup_value;
    if(source->value)
    {
        /* unable to duplicate, missing dup function */
        if(!source->dup_value)
            return 0;
        target->value = source->dup_value(source->value);
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
static const struct ptree_t*
ptree_find_by_key_recurse(const struct ptree_t* tree,
                          const char* delim)
{
    char* token;
    if((token = strtok(NULL, delim)))
    {
        uint32_t hash = hash_jenkins_oaat(token, strlen(token));
        {
            UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
            {
                if(child->hash == hash)
                    return ptree_find_by_key_recurse(child, delim);
            }
            return NULL;
        }
    } else
        return tree;
}

/* ------------------------------------------------------------------------- */
static void
ptree_print_impl(const struct ptree_t* tree, uint32_t depth)
{
    /* indentation */
    uint32_t i;
    for(i = 0; i != depth; ++i)
        printf("    ");
    
    /* print node info */
#ifdef _DEBUG
    printf("key: \"%s\", hash: %d, dup: %p\n", tree->key, tree->hash, tree->dup_value);
#else
    printf("hash: %d, dup: %p\n", tree->hash, (void*)tree->dup_value);
#endif
    
    /* print children */
    {
        UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
        {
            ptree_print_impl(child, depth+1);
        }
    }
}

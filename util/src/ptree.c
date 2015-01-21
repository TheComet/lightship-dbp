#include "util/ptree.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <stdio.h>

struct ptree_t*
ptree_create(const char* key, void* value)
{
    struct ptree_t* tree = (struct ptree_t*)MALLOC(sizeof(struct ptree_t));
    ptree_init_ptree(tree, key, value);
    return tree;
}

void
ptree_init_ptree(struct ptree_t* tree, const char* key, void* value)
{
#ifdef _DEBUG
    tree->key = malloc_string(key);
#endif
    tree->hash = PTREE_HASH_STRING(key);
    tree->value = value;
    unordered_vector_init_vector(&tree->children, sizeof(struct ptree_t));
}

static void
ptree_destroy_recurse(struct ptree_t* tree)
{
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        ptree_destroy_recurse(child);
    }
    unordered_vector_clear_free(&tree->children);
    if(tree->value)
        FREE(tree->value);
#ifdef _DEBUG
    FREE(tree->key);
#endif
}

void
ptree_destroy(struct ptree_t* tree)
{
    ptree_destroy_recurse(tree);
    FREE(tree);
}

struct ptree_t*
ptree_add_node(struct ptree_t* tree, const char* key, void* value)
{
    struct ptree_t* child;
    child = (struct ptree_t*)unordered_vector_push_emplace(&tree->children);
    ptree_init_ptree(child, key, value);
    return child;
}

void*
ptree_find_local_by_key(const struct ptree_t* tree, const char* key)
{
    uint32_t hash = PTREE_HASH_STRING(key);
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        if(hash == child->hash)
            return child->value;
    }
    return NULL;
}

static void*
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
        return tree->value;
}

void*
ptree_find_by_key(const struct ptree_t* tree, const char* key)
{
    /* prepare key for tokenisation */
    void* result;
    const char* delim = ".";
    char* key_iter = cat_strings(2, "n.", key); /* root key name is ignored, but must exist */
    strtok(key_iter, delim);
    result = ptree_find_by_key_recurse(tree, delim);
    FREE(key_iter);
    return result;
}

void
ptree_print_impl(struct ptree_t* tree, uint32_t depth)
{
    /* indentation */
    uint32_t i;
    for(i = 0; i != depth; ++i)
        printf("    ");
    
    /* print node info */
#ifdef _DEBUG
    printf("key: \"%s\", hash: %d\n", tree->key, tree->hash);
#else
    printf("hash: %d\n", tree->hash);
#endif
    
    /* print children */
    {
        UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
        {
            ptree_print_impl(child, depth+1);
        }
    }
}

void
ptree_print(struct ptree_t* tree)
{
    ptree_print_impl(tree, 0);
}

#include "util/ptree.h"
#include "util/memory.h"
#include "util/hash.h"
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
    tree->hash = hash_jenkins_oaat(key, strlen(key));
    tree->value = value;
    unordered_vector_init_vector(&tree->children, sizeof(struct ptree_t));
}

static void
ptree_destroy_recurse(struct ptree_t* tree)
{
    UNORDERED_VECTOR_FOR_EACH_ERASE(&tree->children, struct ptree_t, child)
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
    struct ptree_t child;
    struct ptree_t* alloc;
    ptree_init_ptree(&child, key, value);
    alloc = (struct ptree_t*)unordered_vector_push_emplace(&tree->children);
    memcpy(alloc, &child, sizeof(struct ptree_t));
    return alloc;
}

void*
ptree_find_local_by_key(const struct ptree_t* tree, const char* key)
{
    uint32_t hash = hash_jenkins_oaat(key, strlen(key));
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        if(hash == child->hash)
            return child->value;
    }
    return NULL;
}

void*
ptree_find_by_key(const struct ptree_t* tree, const char* key)
{
    char* key_cpy;
    char* key_cpy_cpy;
    char* token;
    const char* delim = ".";

    /* first, tokenise key */
    key_cpy = malloc_string(key);
    key_cpy_cpy = key_cpy;
    while((token = strtok(key_cpy_cpy, delim)))
    {
        uint32_t hash = hash_jenkins_oaat(token, strlen(token));
        struct ptree_t child;
        /*ptree_init_ptree(&child, key, value);*/
        unordered_vector_push(&tree->children, &child);
    }
    
    /* TODO */
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
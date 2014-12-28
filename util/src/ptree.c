#include "util/ptree.h"
#include "util/memory.h"
#include "util/hash.h"
#include "util/string.h"
#include <string.h>
#include <stdio.h>

struct ptree_t* ptree_create(const char* name, void* data)
{
    struct ptree_t* tree = (struct ptree_t*)MALLOC(sizeof(struct ptree_t));
    ptree_init_ptree(tree, name, data);
    return tree;
}

void ptree_init_ptree(struct ptree_t* tree, const char* name, void* data)
{
#ifdef _DEBUG
    tree->name = name;
#endif
    tree->hash = hash_jenkins_oaat(name, strlen(name));
    tree->data = data;
    unordered_vector_init_vector(&tree->children, sizeof(struct ptree_t));
}

void ptree_destroy(struct ptree_t* tree)
{
}

void ptree_add_node(struct ptree_t* tree, const char* name, void* data)
{
}

struct ptree_t* ptree_find_by_name(struct ptree_t* tree, const char* name)
{
    char* name_cpy;
    char* name_cpy_cpy;
    char* token;
    const char* delim = ".";

    /* first, tokenise name */
    name_cpy = malloc_string(name);
    name_cpy_cpy = name_cpy;
    while((token = strtok(name_cpy_cpy, delim)))
    {
        uint32_t hash = hash_jenkins_oaat(token, strlen(token));
        struct ptree_t child;
        /*ptree_init_ptree(&child, name, data);*/
        unordered_vector_push(&tree->children, &child);
    }
}

void ptree_print_impl(struct ptree_t* tree, uint32_t depth)
{
    /* indentation */
    uint32_t i;
    for(i = 0; i != depth; ++i)
        printf("    ");
    
    /* print node info */
#ifdef _DEBUG
    printf("name: \"%s\", hash: %d\n", tree->name, tree->hash);
#else
    printf("hash: %d\n", tree->hash);
#endif
    
    /* print children */
    UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t, child)
    {
        ptree_print_impl(child, depth+1);
    }
}

void ptree_print(struct ptree_t* tree)
{
    ptree_print_impl(tree, 0);
}
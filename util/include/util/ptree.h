#include "util/pstdint.h"
#include "util/unordered_vector.h"
struct ptree_t
{
    struct unordered_vector_t children;
#ifdef _DEBUG
    char* name;
#endif
    uint32_t hash;
    void* data;
};

struct ptree_t* ptree_create(const char* name, void* data);
void ptree_init_ptree(struct ptree_t* tree, const char* name, void* data);
void ptree_destroy(struct ptree_t* tree);
void ptree_add_node(struct ptree_t* tree, const char* name, void* data);
void ptree_print(struct ptree_t* tree);
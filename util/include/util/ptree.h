#include "util/pstdint.h"
#include "util/config.h"
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

LIGHTSHIP_PUBLIC_API struct ptree_t*
ptree_create(const char* name, void* data);

LIGHTSHIP_PUBLIC_API void
ptree_init_ptree(struct ptree_t* tree, const char* name, void* data);

LIGHTSHIP_PUBLIC_API void
ptree_destroy(struct ptree_t* tree);

LIGHTSHIP_PUBLIC_API void
ptree_add_node(struct ptree_t* tree, const char* name, void* data);

LIGHTSHIP_PUBLIC_API void
ptree_print(struct ptree_t* tree);

#include "util/pstdint.h"
#include "util/config.h"
#include "util/hash.h"
#include "util/unordered_vector.h"

struct ptree_t
{
    struct unordered_vector_t children;
#ifdef _DEBUG
    char* key;
#endif
    uint32_t hash;
    void* value;
};

LIGHTSHIP_PUBLIC_API struct ptree_t*
ptree_create(const char* key, void* data);

LIGHTSHIP_PUBLIC_API void
ptree_init_ptree(struct ptree_t* tree, const char* key, void* data);

LIGHTSHIP_PUBLIC_API void
ptree_destroy(struct ptree_t* tree);

LIGHTSHIP_PUBLIC_API struct ptree_t*
ptree_add_node(struct ptree_t* tree, const char* key, void* data);

LIGHTSHIP_PUBLIC_API void*
ptree_find_local_by_key(const struct ptree_t* tree, const char* key);

LIGHTSHIP_PUBLIC_API void*
ptree_find_by_key(const struct ptree_t* tree, const char* key);

LIGHTSHIP_PUBLIC_API void
ptree_print(struct ptree_t* tree);

#define PTREE_FOR_EACH(tree, var) \
    UNORDERED_VECTOR_FOR_EACH(&(tree)->children, struct ptree_t, var)

#define PTREE_HASH_STRING(str) hash_jenkins_oaat(str, strlen(str))

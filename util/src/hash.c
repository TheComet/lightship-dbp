#include "util/hash.h"

uint32_t
hash_jenkins_oaat(const char* key, uint32_t len)
{
    uint32_t hash, i;
    for(hash = i = 0; i != len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 1);
    hash += (hash << 15);
    return hash;
}

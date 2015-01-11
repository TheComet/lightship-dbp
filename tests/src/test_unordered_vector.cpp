#include "gmock/gmock.h"
#include "util/unordered_vector.h"

#define NAME unordered_vector

TEST(NAME, create_and_destroy)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    unordered_vector_destroy(vec);
}

#include "gmock/gmock.h"
#include "util/file.h"
#include "util/memory.h"

#define NAME file_malloc

using namespace testing;

TEST(NAME, load_into_memory)
{
    void* buffer = (void*)2387; /* garbage */
    force_malloc_fail_on();
    EXPECT_THAT(file_load_into_memory("tests/test_dir/files/file_a.txt", &buffer, FILE_BINARY), Eq(0));
    force_malloc_fail_off();
    ASSERT_THAT(buffer, IsNull());

    EXPECT_THAT(file_load_into_memory("tests/test_dir/files/file_a.txt", &buffer, FILE_BINARY), Ne(0));
    EXPECT_THAT(buffer, NotNull());

    free_file(buffer);
}

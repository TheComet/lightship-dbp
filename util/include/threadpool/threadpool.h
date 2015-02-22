#include "util/pstdint.h"
#include "util/config.h"

struct thread_pool_t;

#ifdef ENABLE_THREAD_POOL

uint32_t
get_number_of_cores();

struct thread_pool_t*
thread_pool_create(int num_threads);

void
thread_pool_destroy(struct thread_pool_t* pool);

#else /* ENABLE_THREAD_POOL */
#   define get_number_of_cores() 1
#   define thread_pool_create(x) (struct thread_pool_t*)1;
#   define thread_pool_destroy(x)
#endif /* ENABLE_THREAD_POOL */

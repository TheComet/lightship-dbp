#include "util/pstdint.h"
#include "util/config.h"

struct thread_pool_t;

typedef void (*thread_pool_job_func)(void*);

#ifdef ENABLE_THREAD_POOL

LIGHTSHIP_UTIL_PUBLIC_API uint32_t
get_number_of_cores();

LIGHTSHIP_UTIL_PUBLIC_API struct thread_pool_t*
thread_pool_create(int num_threads);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_destroy(struct thread_pool_t* pool);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data);

#else /* ENABLE_THREAD_POOL */
#   define get_number_of_cores() 1
#   define thread_pool_create(x) (struct thread_pool_t*)1;
#   define thread_pool_destroy(x)
#endif /* ENABLE_THREAD_POOL */

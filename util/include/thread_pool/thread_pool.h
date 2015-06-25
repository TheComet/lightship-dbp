#include "util/pstdint.h"
#include "util/config.h"

struct thread_pool_t;

typedef void (*thread_pool_job_func)(void*);

#ifdef ENABLE_THREAD_POOL

LIGHTSHIP_UTIL_PUBLIC_API uint32_t
get_number_of_cores();

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_set_max_buffer_size(uint32_t maximum_buffer_size);

LIGHTSHIP_UTIL_PUBLIC_API struct thread_pool_t*
thread_pool_create(uint32_t num_threads, uint32_t buffer_size_in_bytes);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_destroy(struct thread_pool_t* pool);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_suspend(struct thread_pool_t* pool);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_resume(struct thread_pool_t* pool);

LIGHTSHIP_UTIL_PUBLIC_API void
thread_pool_wait_for_jobs(struct thread_pool_t* pool);

#else /* ENABLE_THREAD_POOL */
	/* return 1 for single threaded */
#   define get_number_of_cores() 1
	/* nop */
#   define thread_pool_set_max_buffer_size(maximum_buffer_size)
	/* must return a non-zero value for success */
#   define thread_pool_create(num_threads, buffer_size_in_bytes) (struct thread_pool_t*) 1;
	/* nop */
#   define thread_pool_destroy(pool)
	/* directly call the job being queued */
#   define thread_pool_queue(pool, func, data) do { \
						thread_pool_job_func f = func; \
						f(data); } while(0)
	/* nop */
#   define thread_pool_suspend(pool)
	/* nop */
#   define thread_pool_resume(pool)
	/* no need to wait for jobs, nop */
#   define thread_pool_wait_for_jobs(pool)
#endif /* ENABLE_THREAD_POOL */

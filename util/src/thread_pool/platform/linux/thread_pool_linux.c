#include "thread_pool/thread_pool.h"
#include "util/log.h"
#include "util/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#ifdef _WIN32
#   include <windows.h>
#elif MACOS
#   include <sys/param.h>
#   include <sys/sysctl.h>
#else
#   include <unistd.h>
#endif

#define DATA_POINTER_TYPE unsigned char
struct ring_buffer_t
{
    intptr_t num_jobs;
    intptr_t element_size;
    intptr_t flg_buffer_size_in_bytes;
    intptr_t write_pos;
    intptr_t read_pos;
    DATA_POINTER_TYPE* obj_buffer;
    DATA_POINTER_TYPE* flg_buffer;
};

typedef enum buffer_flags_e
{
    FLAG_FREE = 0,
    FLAG_WRITE_IN_PROGRESS = 1,
    FLAG_READ_ME = 2,
    FLAG_READ_IN_PROGRESS = 3
} buffer_flags_e;

static void
ring_buffer_init_buffer(struct ring_buffer_t* rb, intptr_t element_size);

static void
ring_buffer_free_contents(struct ring_buffer_t* rb);

static void
ring_buffer_resize(struct ring_buffer_t* rb, intptr_t new_size);

struct thread_pool_t
{
    int num_threads;
    char active;
    pthread_t* thread;
    pthread_t realloc_thread;
    pthread_cond_t realloc_cv;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    struct ring_buffer_t rb;
};

struct thread_pool_job_t
{
    thread_pool_job_func func;
    void* data;
};

static void*
thread_pool_worker(struct thread_pool_t* pool);

static void
thread_pool_init_pool(struct thread_pool_t* pool, int num_threads);

/* ------------------------------------------------------------------------- */
uint32_t
get_number_of_cores()
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;
 
    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);
 
    if(count < 1)
    {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if(count < 1)
            count = 1;
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

/* ------------------------------------------------------------------------- */
struct thread_pool_t*
thread_pool_create(int num_threads)
{
    /* create and init thread pool object */
    struct thread_pool_t* pool = (struct thread_pool_t*)MALLOC(sizeof(struct thread_pool_t));
    thread_pool_init_pool(pool, num_threads);
    return pool;
}

/* ------------------------------------------------------------------------- */
void
thread_pool_destroy(struct thread_pool_t* pool)
{
    /*
     * If ENABLE_RING_BUFFER_REALLOC is enabled, then shutting down the
     * reallocation thread will also shut down all worker threads. If the
     * option is not enabled, we have to shut down the worker threads from
     * here.
     */
    thread_pool_suspend(pool);
    {
        pthread_mutex_lock(&pool->mutex);
        pthread_cond_signal(&pool->realloc_cv);
        pthread_mutex_unlock(&pool->mutex);
        pthread_join(pool->realloc_thread, NULL);
    }

    /* clean up */
    ring_buffer_free_contents(&pool->rb);
    FREE(pool->thread);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cv);
    FREE(pool);
}

/* ------------------------------------------------------------------------- */
static void*
thread_pool_ring_buffer_realloc_thread(struct thread_pool_t* pool)
{
    /*
     * Suspend this thread until a reallocation event occurs.
     */
    pthread_mutex_lock(&pool->mutex);
    while(pool->active)
    {
        pthread_cond_wait(&pool->realloc_cv, &pool->mutex);
        if(!pool->active)
            break;
        
        /* 
         * Reallocation event received. The safest approach, although not that
         * fast, is:
         *  - shut down worker threads.
         *  - realloc
         *  - resume worker threads.
         */
        pthread_mutex_unlock(&pool->mutex);
        thread_pool_suspend(pool);
        /* expand by factor 2 */
        ring_buffer_resize(&pool->rb, pool->rb.flg_buffer_size_in_bytes << 1);
        thread_pool_resume(pool);
        pthread_mutex_lock(&pool->mutex);
    }
    pthread_mutex_unlock(&pool->mutex);
    
    pthread_exit(NULL);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data)
{
    /*
     * WARNING: This function is not thread safe if ENABLE_RING_BUFFER_ALLOC is enabled.
     * This is to favour speed. You can of course use your own mutex if inserting from
     * multiple threads to get around this.
     */
    
    intptr_t write_pos;
    intptr_t obj_ptr_offset;
    struct thread_pool_job_t* job;
    DATA_POINTER_TYPE* flag_buffer;
    
    /* fetch and wrap write position */
    write_pos = __sync_fetch_and_add(&pool->rb.write_pos, 1) % pool->rb.flg_buffer_size_in_bytes;
    /* calculate relative address of where to store object in the object buffer */
    obj_ptr_offset = write_pos * pool->rb.element_size;
    
    /* 
     * Set flag to "write in progress" in flag buffer.
     * Depending on the configuration, if the buffer has overflown, either
     * spinlock until one of the worker threads completes and frees a job, or
     * reallocate the ring buffer.
     */
    flag_buffer = &(pool->rb.flg_buffer[write_pos]);
    if(!__sync_bool_compare_and_swap(flag_buffer, FLAG_FREE, FLAG_WRITE_IN_PROGRESS))
    {
/*#ifdef ENABLE_RING_BUFFER_REALLOC*/
        /*
         * Since this thread has failed to insert the new job into the queue,
         * and - if it is a pool thread - will have to terminate before
         * reallocation, the only choice left is to execute the job immediately
         * before reallocation.
         */
        func(data);
        
        /*
         * write_pos was incremented, but we already executed the job that would
         * have been inserted at that position. Therefore, it can be decremented
         * again.
         */
        __sync_fetch_and_sub(&pool->rb.write_pos, 1);
        
        /*
         * Ready for reallocation.
         */
        pthread_cond_signal(&pool->realloc_cv);
        
        return;
/*#endif*/
    }
    
    /*
     * Flag has been set to "write in progress", so the job can now be safely
     * copied into the target buffer.
     */
    job = (struct thread_pool_job_t*)(pool->rb.obj_buffer + obj_ptr_offset);
    job->func = func;
    job->data = data;
    
    /* buffer is ready for reading */
    __sync_bool_compare_and_swap(flag_buffer, FLAG_WRITE_IN_PROGRESS, FLAG_READ_ME);
    
    /* wake up a worker thread */
    pthread_cond_signal(&pool->cv);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_suspend(struct thread_pool_t* pool)
{
    int i;

    /* exit all threads */
    pthread_mutex_lock(&pool->mutex);
    if(!pool->active) /* already suspended */
    {
        pthread_mutex_unlock(&pool->mutex);
        return;
    }
    pool->active = 0;
    pthread_cond_broadcast(&pool->cv);
    pthread_mutex_unlock(&pool->mutex);
    for(i = 0; i != pool->num_threads; ++i)
        pthread_join(pool->thread[i], NULL);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_resume(struct thread_pool_t* pool)
{
    pthread_attr_t attr;
    int i;
    
    /* set pool to active, so threads know to not exit */
    pthread_mutex_lock(&pool->mutex);
    if(pool->active) /* already running */
    {
        pthread_mutex_unlock(&pool->mutex);
        return;
    }
    pool->active = 1;
    pthread_mutex_unlock(&pool->mutex);

    /* for portability, explicitely create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* launch threads and make them idle */
    for(i = 0; i != pool->num_threads; ++i)
        pthread_create(&(pool->thread[i]), &attr, (void*(*)(void*))thread_pool_worker, pool);

    /* clean up */
    pthread_attr_destroy(&attr);
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */
static void
thread_pool_worker_handler(struct thread_pool_t* pool)
{
    struct thread_pool_job_t* job;
    intptr_t read_pos;
    DATA_POINTER_TYPE* flag_buffer;
    
    while(pool->active)
    {
        /* get a unique read position in the buffer */
        read_pos = __sync_fetch_and_add(&pool->rb.read_pos, 1) % pool->rb.flg_buffer_size_in_bytes;
        flag_buffer = &(pool->rb.flg_buffer[read_pos]);
        
        /* even if there is no data there, we can wait until there is */
        if(!__sync_bool_compare_and_swap(flag_buffer, FLAG_READ_ME, FLAG_READ_IN_PROGRESS))
        {
            pthread_mutex_lock(&pool->mutex);
            while(!__sync_bool_compare_and_swap(flag_buffer, FLAG_READ_ME, FLAG_READ_IN_PROGRESS) && pool->active)
                pthread_cond_wait(&pool->cv, &pool->mutex);
            if(!pool->active)
            {
                /* restore unprocessed job - required for suspend/resume */
                __sync_fetch_and_sub(&pool->rb.read_pos, 1);
                pthread_mutex_unlock(&pool->mutex);
                return;
            }
            pthread_mutex_unlock(&pool->mutex);
        }

        /* exec job and set flag to free once done */
        job = (struct thread_pool_job_t*)(pool->rb.obj_buffer + read_pos * pool->rb.element_size);
        job->func(job->data);
        __sync_bool_compare_and_swap(flag_buffer, FLAG_READ_IN_PROGRESS, FLAG_FREE);
    }
}

/* this is the entry point for worker threads */
static void*
thread_pool_worker(struct thread_pool_t* pool)
{
    thread_pool_worker_handler(pool);
    pthread_exit(NULL);
}

/* ------------------------------------------------------------------------- */
static void
thread_pool_init_pool(struct thread_pool_t* pool, int num_threads)
{
    char thread_self_str[sizeof(int)*8+3];
    
    sprintf(thread_self_str, "0x%lx", (intptr_t)pthread_self());
    llog(LOG_INFO, NULL, 2, "Thread pool initialising on thread ", thread_self_str);
    
    memset(pool, 0, sizeof(struct thread_pool_t));
    
    /* init ring buffer (job queue) */
    ring_buffer_init_buffer(&pool->rb, sizeof(struct thread_pool_job_t));
    
    /* initialise mutexes and conditional variables */
    pthread_cond_init(&pool->cv, NULL);
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->realloc_cv, NULL);
    
    /* set number of threads to create - if num_threads is 0, set it to the number of CPU cores present */
    if(num_threads)
        pool->num_threads = num_threads;
    else
        pool->num_threads = get_number_of_cores();
    
    /* allocate buffer to store thread objects */
    pool->thread = (pthread_t*)MALLOC(pool->num_threads * sizeof(pthread_t));
    
    /* launches all worker threads */
    thread_pool_resume(pool);

    /* 
     * If ENABLE_RING_BUFFER_REALLOC is enabled, then the worker threads are
     * launched from the reallocation thread. If this option is disabled,
     * they can be launched from here directly.
     */
    {
        pthread_attr_t attr;

        /* for portability, explicitely create threads in a joinable state */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&pool->realloc_thread, &attr, (void*(*)(void*))thread_pool_ring_buffer_realloc_thread, pool);
        pthread_attr_destroy(&attr);
    }
}

/* ------------------------------------------------------------------------- */
/* LOCK FREE RING BUFFER IMPLEMENTATION */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
ring_buffer_init_buffer(struct ring_buffer_t* rb, intptr_t element_size)
{
    memset(rb, 0, sizeof(struct ring_buffer_t));
    rb->element_size = element_size;
    ring_buffer_resize(rb, RING_BUFFER_FIXED_SIZE);
}

/* ------------------------------------------------------------------------- */
void
ring_buffer_free_contents(struct ring_buffer_t* rb)
{
    rb->flg_buffer_size_in_bytes = 0;
    FREE(rb->flg_buffer);
    rb->flg_buffer = NULL;
    rb->obj_buffer = NULL;
}

/* ------------------------------------------------------------------------- */
static void
ring_buffer_resize(struct ring_buffer_t* rb, intptr_t new_size)
{
    DATA_POINTER_TYPE* buffer;
    DATA_POINTER_TYPE* old_buffer;
    intptr_t buffer_size, old_buffer_size;
    
    /*
     * NOTE: Mutex should be locked at this point.
     */
    char new_size_str[sizeof(int)*8+3];
    sprintf(new_size_str, "0x%lx", new_size);
    llog(LOG_INFO, NULL, 2, "Ring buffer resize: ", new_size_str);
    
    /* allocate new buffer */
    old_buffer = rb->flg_buffer;
    old_buffer_size = rb->flg_buffer_size_in_bytes;
    buffer_size = new_size + /* flg_buffer */
                  new_size * rb->element_size; /* obj_buffer */
    buffer = (DATA_POINTER_TYPE*)MALLOC(buffer_size);
    
    /* wrap read and write pointers to new size */
    if(rb->flg_buffer_size_in_bytes)
    {
        rb->read_pos  = rb->read_pos  % rb->flg_buffer_size_in_bytes;
        rb->write_pos = rb->write_pos % rb->flg_buffer_size_in_bytes;
    }
    
    /* swap old with new */
    if(old_buffer)
    {
        /* flag buffer */
        memcpy(buffer, old_buffer, old_buffer_size);
        /* object buffer */
        memcpy(buffer, old_buffer + old_buffer_size, old_buffer_size * rb->element_size);
    }
    else
    {
        memset(buffer, 0, buffer_size);
    }
    
    /* set pointers and new size correctly */
    rb->flg_buffer = buffer;
    rb->obj_buffer = buffer + new_size;
    rb->flg_buffer_size_in_bytes = new_size;
    if(!old_buffer)
        return;
    FREE(old_buffer);
}


/*!
 * @file thread_pool_linux.c
 * @brief Thread pool using a lock-free, dynamically resizable ring buffer
 * implementation for job storage.
 * 
 * Overview of implementation
 * ==========================
 * 
 * 
 * Requirements, Design Decisions, Problems and Solutions Explained
 * ================================================================
 * 1. Requirements
 * ------------
 * A thread pool supporting a user defined number of threads. Inserting jobs
 * must be thread safe and fast.
 * 
 * In order for jobs to be inserted and erased, an underlying thread safe
 * container must be created in order to hold pending job objects.
 * 
 * Job objects hold a function pointer and a data pointer, making it possible
 * for workers to call said functions with a single, user-defined argument.
 * 
 * 
 * 2. Design Decisions
 * ----------------
 * Worker threads with nothing to do shall be suspended, freeing CPU resources.
 * This will be achieved with condition variables.
 * 
 * The most optimal container to use for storing job objects is a ring buffer,
 * since it fulfills the requirement of an ordered container, and it doesn't
 * require for elements to be shifted around when inserting or deleting.
 * 
 * The ring buffer is especially suited for this job, as it can utilise atomic
 * operations for incrementing and decrementing the read/write positions.
 * 
 * 
 * 3. Problems and Solutions
 * -------------------------
 * 
 */

#include "thread_pool/thread_pool.h"
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

/* this constant can be configured in CMakeLists.txt */
static uint32_t g_max_buf_size = RING_BUFFER_MAX_SIZE;

#ifdef DATA_POINTER_TYPE
#   undef DATA_POINTER_TYPE
#endif
#define DATA_POINTER_TYPE unsigned char

/*!
 * @brief Ring buffer object.
 * 
 * 
 */
struct ring_buffer_t
{
    intptr_t element_size;
    intptr_t flg_buffer_size_in_bytes;
    intptr_t write_pos;
    intptr_t read_pos;
    DATA_POINTER_TYPE* obj_buffer;
    DATA_POINTER_TYPE* flg_buffer;
};

/*!
 * @brief Flag buffer values.
 * 
 * Meaning:
 * + FLAG_FREE              : The corresponding read/write position is empty.
 *                            It is ready to accept data. Before writing data,
 *                            the flag must be changed to FLAG_WRITE_IN_PROGRESS.
 * + FLAG_WRITE_IN_PROGRESS : The corresponding read/write position is in the
 *                            process of being written to. Once writing is
 *                            finished, the flag must be changed to
 *                            FLAG_READ_ME.
 * + FLAG_READ_ME           : The corresponding read/write position contains
 *                            data ready for reading. Before reading data, the
 *                            flag must be changed to FLAG_READ_IN_PROGRESS.
 * + FLAG_READ_IN_PROGRESS  : The corresponding read/write position is currently
 *                            being read from. Once reading is complete, the
 *                            flag must be changed back to FLAG_FREE so it can
 *                            be written to again.
 */
typedef enum ring_buffer_flags_e
{
    FLAG_FREE = 0,
    FLAG_WRITE_IN_PROGRESS = 1,
    FLAG_READ_ME = 2,
    FLAG_READ_IN_PROGRESS = 3
} ring_buffer_flags_e;

struct thread_pool_t
{
    int                     num_threads;        /* number of worker threads to spawn on resume */
    int                     num_jobs;           /* number of jobs queued or actively being executed - use atomics to modify */
    char                    active;             /* whether or not the pool is active - use atomics to modify */
    pthread_t*              worker_threads;     /* vector of worker thread handles */
    pthread_cond_t          worker_wakeup_cv;   /* condition variable for waking up a worker thread - lock worker_mutex for access */
    pthread_mutex_t         worker_mutex;       /* lock for worker_wakeup_cv, num_active_threads, and active */
    pthread_cond_t          job_finished_cv;    /* condition variable for waking up threads waiting on finished jobs - lock job_finished_mutex for access */
    struct ring_buffer_t    rb;                 /* ring buffer for storing jobs */
};

struct thread_pool_job_t
{
    thread_pool_job_func func;
    void* data;
};

/*!
 * @brief Initialises the specified ring buffer object.
 * @param rb The ring buffer object to initialise.
 * @param element_size The size of the elements this ring buffer will be
 * storing in bytes. One can pass the return value of sizeof().
 * @param buffer_size_in_bytes The initial size of the ring buffer in bytes.
 * If a value of 0 is specified, then the size of the buffer shall be
 * initialised to RING_BUFFER_FIXED_SIZE. This value can be configured in
 * CMakeLists.txt.
 */
static void
ring_buffer_init_buffer(struct ring_buffer_t* rb, intptr_t element_size, uint32_t buffer_size_in_bytes);

/*!
 * @brief De-initialises the specified ring buffer object.
 * @note Threads shall not access the ring buffer during or after this call.
 * @param rb The ring buffer object to de-initialise.
 */
static void
ring_buffer_deinit_buffer(struct ring_buffer_t* rb);

/*!
 * @brief Resizes the ring buffer to the specified size.
 * @note Threads shall not access the ring buffer during this call.
 * @param rb The ring buffer to resize.
 * @param new_size Size in bytes to resize the buffer to.
 */
static void
ring_buffer_resize(struct ring_buffer_t* rb, intptr_t new_size);

/*!
 * @brief This is the entry point for worker threads.
 * @param pool The pool the launched thread should work for.
 */
static void*
thread_pool_worker(struct thread_pool_t* pool);

/*!
 * @brief Initialises a thread pool object.
 * 
 * @note This includes launching all of the worker threads.
 * @param pool The pool object to initialise.
 * @param num_threads The number of worker threads to launch.
 * @param buffer_size_in_bytes The initial size of the ring buffer in bytes.
 * If a value of 0 is specified, then the size of the buffer shall be
 * initialised to RING_BUFFER_FIXED_SIZE. This value can be configured in
 * CMakeLists.txt.
 */
static void
thread_pool_init_pool(struct thread_pool_t* pool, uint32_t num_threads, uint32_t buffer_size_in_bytes);

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
void
thread_pool_set_max_buffer_size(uint32_t maximum_buffer_size)
{
    g_max_buf_size = maximum_buffer_size;
}

/* ------------------------------------------------------------------------- */
struct thread_pool_t*
thread_pool_create(uint32_t num_threads, uint32_t buffer_size_in_bytes)
{
    /* create and init thread pool object */
    struct thread_pool_t* pool = (struct thread_pool_t*)MALLOC(sizeof(struct thread_pool_t));
    thread_pool_init_pool(pool, num_threads, buffer_size_in_bytes);
    return pool;
}

/* ------------------------------------------------------------------------- */
void
thread_pool_destroy(struct thread_pool_t* pool)
{
    /*
     * Shut down worker threads.
     */
    thread_pool_suspend(pool);

    /* clean up */
    ring_buffer_deinit_buffer(&pool->rb);
    pthread_cond_destroy(&pool->worker_wakeup_cv);
    pthread_mutex_destroy(&pool->worker_mutex);
    pthread_cond_destroy(&pool->job_finished_cv);
    FREE(pool->worker_threads);
    FREE(pool);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data)
{
    /*
     * WARNING: This function is not thread safe.
     */
    
    intptr_t write_pos;
    struct thread_pool_job_t* job;
    DATA_POINTER_TYPE* flag_buffer;
    
    /* fetch, increment and wrap write position */
    write_pos = __sync_fetch_and_add(&pool->rb.write_pos, 1) % pool->rb.flg_buffer_size_in_bytes;
    /* cache flag buffer, it's used multiple times */
    flag_buffer = pool->rb.flg_buffer + write_pos;
    
    /* 
     * Set flag to "write in progress" in flag buffer.
     * If the buffer overflows, spinlock until one of the worker threads
     * completes and frees a job.
     */
    while(!__sync_bool_compare_and_swap(flag_buffer, FLAG_FREE, FLAG_WRITE_IN_PROGRESS))
    {
    }
    
    /*
     * Flag has been set to "write in progress", so the job can now be safely
     * copied into the target buffer.
     */
    job = (struct thread_pool_job_t*)(pool->rb.obj_buffer + write_pos * pool->rb.element_size);
    job->func = func;
    job->data = data;
    
    /* buffer is ready for reading */
    __sync_add_and_fetch(&pool->num_jobs, 1);
    __sync_bool_compare_and_swap(flag_buffer, FLAG_WRITE_IN_PROGRESS, FLAG_READ_ME);
    
    /* wake up all worker threads */
    pthread_mutex_lock(&pool->worker_mutex);
    pthread_cond_broadcast(&pool->worker_wakeup_cv);
    pthread_mutex_unlock(&pool->worker_mutex);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_suspend(struct thread_pool_t* pool)
{
    int i;

    /* join all worker threads */
    if(!__sync_fetch_and_add(&pool->active, 0)) /* already suspended */
        return;
    __sync_and_and_fetch(&pool->active, 0); /* set to inactive */

    /* join worker threads */
    pthread_mutex_lock(&pool->worker_mutex);
    /* cv flag is pool->active, which has now been set to 0 atomically */
    pthread_cond_broadcast(&pool->worker_wakeup_cv);
    pthread_mutex_unlock(&pool->worker_mutex);
    for(i = 0; i != pool->num_threads; ++i)
        pthread_join(pool->worker_threads[i], NULL);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_resume(struct thread_pool_t* pool)
{
    pthread_attr_t attr;
    int i;
    
    /* set pool to active, so threads know to not exit */
    if(__sync_fetch_and_add(&pool->active, 0)) /* already running */
        return;
    __sync_or_and_fetch(&pool->active, 1); /* set to active */

    /* for portability, explicitely create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* launch worker threads */
    for(i = 0; i != pool->num_threads; ++i)
        pthread_create(&(pool->worker_threads[i]), &attr, (void*(*)(void*))thread_pool_worker, pool);

    /* clean up */
    pthread_attr_destroy(&attr);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_wait_for_jobs(struct thread_pool_t* pool)
{
    /* wait for number of active jobs to drop to 0 */
    pthread_mutex_lock(&pool->worker_mutex);
    while(__sync_fetch_and_add(&pool->num_jobs, 0))
        pthread_cond_wait(&pool->job_finished_cv, &pool->worker_mutex);
    pthread_mutex_unlock(&pool->worker_mutex);
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */
static void
thread_pool_init_pool(struct thread_pool_t* pool, uint32_t num_threads, uint32_t buffer_size_in_bytes)
{
    printf("Thread pool initialising on thread 0x%lx\n", (intptr_t)pthread_self());
    
    /* set struct memory to a defined state */
    memset(pool, 0, sizeof(struct thread_pool_t));
    
    /* init ring buffer (job queue) */
    ring_buffer_init_buffer(&pool->rb, sizeof(struct thread_pool_job_t), buffer_size_in_bytes);
    
    /* initialise mutexes and conditional variables */
    pthread_cond_init(&pool->worker_wakeup_cv, NULL);
    pthread_mutex_init(&pool->worker_mutex, NULL);
    pthread_cond_init(&pool->job_finished_cv, NULL);
    
    /* set number of threads to create - if num_threads is 0, set it to the number of CPU cores present */
    if(num_threads)
        pool->num_threads = num_threads;
    else
        pool->num_threads = get_number_of_cores();
    
    /* allocate buffer to store thread objects in */
    pool->worker_threads = (pthread_t*)MALLOC(pool->num_threads * sizeof(pthread_t));
    memset(pool->worker_threads, 0, pool->num_threads * sizeof(pthread_t));
    
    /* launches all worker threads */
    thread_pool_resume(pool);
}

/* ------------------------------------------------------------------------- */
/* this is the entry point for worker threads */
static void*
thread_pool_worker(struct thread_pool_t* pool)
{
    struct thread_pool_job_t* job;
    intptr_t read_pos;
    DATA_POINTER_TYPE* flag_ptr;
    
    while(__sync_fetch_and_add(&pool->active, 0))
    {
        /* fetch, increment, and wrap read position */
        read_pos = __sync_fetch_and_add(&pool->rb.read_pos, 1) % pool->rb.flg_buffer_size_in_bytes;
        
        /* cache flag pointer, as it's used multiple times */
        flag_ptr = pool->rb.flg_buffer + read_pos;
        
        /* if there is no more data left in the queue, we can wait until there is */
        if(!__sync_bool_compare_and_swap(flag_ptr, FLAG_READ_ME, FLAG_READ_IN_PROGRESS))
        {
            /* signal that a job has been finished */
            pthread_mutex_lock(&pool->worker_mutex);
            pthread_cond_broadcast(&pool->job_finished_cv);
            
            /* 
             * Wait for wakeup signal.
             * Wakeup should only occur if either the pool is shutting down,
             * or a job is available. Go back to sleep if a wakeup flag was
             * set by accident.
             */
            while(__sync_fetch_and_add(&pool->active, 0) && !__sync_bool_compare_and_swap(flag_ptr, FLAG_READ_ME, FLAG_READ_IN_PROGRESS))
            {
                pthread_cond_wait(&pool->worker_wakeup_cv, &pool->worker_mutex);
            }
            
            /* if the pool is no longer active, don't process the job */
            if(!__sync_fetch_and_add(&pool->active, 0))
            {
                /* 
                 * Restore unprocessed job - required for suspend/resume.
                 * Decrementing the read position doesn't mean the job was
                 * lost, it just means this thread is giving up ownership
                 * for this particular read position. When the pool is
                 * resumed, the pending jobs will be picked up again.
                 */
                __sync_fetch_and_sub(&pool->rb.read_pos, 1);
                
                pthread_mutex_unlock(&pool->worker_mutex);
                break;
            }
            
            pthread_mutex_unlock(&pool->worker_mutex);
        }
        
        /* exec job and set flag to free once done */
        job = (struct thread_pool_job_t*)(pool->rb.obj_buffer + read_pos * pool->rb.element_size);
        job->func(job->data);
        __sync_sub_and_fetch(&pool->num_jobs, 1);
        __sync_bool_compare_and_swap(flag_ptr, FLAG_READ_IN_PROGRESS, FLAG_FREE);
    }
    
    pthread_mutex_lock(&pool->worker_mutex);
    pthread_cond_broadcast(&pool->job_finished_cv);
    pthread_mutex_unlock(&pool->worker_mutex);
    pthread_exit(NULL);
}

/* ------------------------------------------------------------------------- */
/* LOCK FREE RING BUFFER IMPLEMENTATION */
/* ------------------------------------------------------------------------- */
void
ring_buffer_init_buffer(struct ring_buffer_t* rb, intptr_t element_size, uint32_t buffer_size_in_bytes)
{
    memset(rb, 0, sizeof(struct ring_buffer_t));
    rb->element_size = element_size;
    buffer_size_in_bytes = (buffer_size_in_bytes != 0 ? buffer_size_in_bytes : RING_BUFFER_FIXED_SIZE);
    ring_buffer_resize(rb, buffer_size_in_bytes);
}

/* ------------------------------------------------------------------------- */
void
ring_buffer_deinit_buffer(struct ring_buffer_t* rb)
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
    
    /* don't go over maximum buffer size */
    if(new_size > (intptr_t)g_max_buf_size)
    {
        if(rb->flg_buffer_size_in_bytes)
            return;
        new_size = g_max_buf_size;
    }
    
    /* log */
    printf("Ring buffer resize: 0x%lx\n", new_size);
    
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


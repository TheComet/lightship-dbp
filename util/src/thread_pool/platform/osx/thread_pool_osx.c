#include "thread_pool/thread_pool.h"
#include "util/unordered_vector.h"
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

struct thread_pool_t
{
    int num_threads;
    char active;
    pthread_t* thread;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    struct unordered_vector_t jobs;
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
    int i;

    /* exit all threads */
    pthread_mutex_lock(&pool->mutex);
    pool->active = 0;
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_broadcast(&pool->cv);
    for(i = 0; i != pool->num_threads; ++i)
        pthread_join(pool->thread[i], NULL);

    /* clean up */
    unordered_vector_clear_free(&pool->jobs);
    FREE(pool->thread);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cv);
    FREE(pool);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data)
{
    struct thread_pool_job_t* job;
    pthread_mutex_lock(&pool->mutex);
    job = unordered_vector_push_emplace(&pool->jobs);
    job->func = func;
    job->data = data;
    pthread_cond_signal(&pool->cv);
    pthread_mutex_unlock(&pool->mutex);
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */
static void
thread_pool_process_while_active(struct thread_pool_t* pool)
{
    struct thread_pool_job_t* pjob, job;
    
    /* 
     * mutex must be locked at this point
     */
    
    
    while(pool->active)
    {
        /* pop from work queue and do work until there is no more work */
        pjob = unordered_vector_pop(&pool->jobs);
        while(!pjob)
        {
            pthread_cond_wait(&pool->cv, &pool->mutex);
            if(!pool->active)
                return;
            pjob = unordered_vector_pop(&pool->jobs);
        }

        job = *pjob; /* copy job before unlocking, pointer is only valid as long
                        as we were the last one to manipulate the vector */
        pthread_mutex_unlock(&pool->mutex);
        job.func(job.data);
        pthread_mutex_lock(&pool->mutex);
    }
}

static void*
thread_pool_worker(struct thread_pool_t* pool)
{
    char thread_self_str[sizeof(int)*8+3];
    sprintf(thread_self_str, "0x%lx", (intptr_t)pthread_self());
    llog(LOG_INFO, NULL, 3, "Worker thread ", thread_self_str, " started");
    
    pthread_mutex_lock(&pool->mutex);
    thread_pool_process_while_active(pool);
    pthread_mutex_unlock(&pool->mutex);
    
    llog(LOG_INFO, NULL, 3, "Worker thread ", thread_self_str, " stopping");
    pthread_exit(NULL);
}

/* ------------------------------------------------------------------------- */
static void
thread_pool_init_pool(struct thread_pool_t* pool, int num_threads)
{
    pthread_attr_t attr;
    int i;
    char thread_self_str[sizeof(int)*8+3];
    
    sprintf(thread_self_str, "0x%lx", (intptr_t)pthread_self());
    llog(LOG_INFO, NULL, 2, "Thread pool initialising on thread ", thread_self_str);
    
    /* init jobs */
    unordered_vector_init_vector(&pool->jobs, sizeof(struct thread_pool_job_t));

    /* set pool to active, so threads know to not exit */
    pool->active = 1;

    /* for portability, explicitely create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* initialise mutex and conditional variables */
    pthread_cond_init(&pool->cv, NULL);
    pthread_mutex_init(&pool->mutex, NULL);

    /* set number of threads to create - if num_threads is 0, set it to the number of CPU cores present */
    if(num_threads)
        pool->num_threads = num_threads;
    else
        pool->num_threads = get_number_of_cores();

    /* launch threads and make them idle */
    pool->thread = (pthread_t*)MALLOC(pool->num_threads * sizeof(pthread_t));
    for(i = 0; i != pool->num_threads; ++i)
        pthread_create(&pool->thread[i], &attr, (void*(*)(void*))thread_pool_worker, pool);

    /* clean up */
    pthread_attr_destroy(&attr);
}


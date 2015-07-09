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

#include "thread_pool.h"
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
	intptr_t job_slots;
	intptr_t job_size_in_bytes;
	intptr_t write_pos;
	intptr_t read_pos;
	DATA_POINTER_TYPE* obj_buffer;
	DATA_POINTER_TYPE* flg_buffer;
};

/*!
 * @brief Flag buffer values.
 *
 * Meaning:
 * + FLAG_FREE_SLOT         : The corresponding read/write position is empty.
 *                            It is ready to accept data. Before writing data,
 *                            the flag must be changed to FLAG_WRITE_IN_PROGRESS.
 * + FLAG_WRITE_IN_PROGRESS : The corresponding read/write position is in the
 *                            process of being written to. Once writing is
 *                            finished, the flag must be changed to
 *                            FLAG_READ_ME.
 * + FLAG_FILLED_SLOT       : The corresponding read/write position contains
 *                            data ready for reading. Before reading data, the
 *                            flag must be changed to FLAG_READ_IN_PROGRESS.
 * + FLAG_READ_IN_PROGRESS  : The corresponding read/write position is currently
 *                            being read from. Once reading is complete, the
 *                            flag must be changed back to FLAG_FREE_SLOT so it
 *                            can be written to again.
 * + FLAG_INVALID_JOB       : The corresponding read/write position has been
 *                            skipped and the data that exists at this location
 *                            is garbage. This flag will be set when a ring
 *                            buffer overflows and the job is executed by the
 *                            inserting thread instead.
 */
typedef enum ring_buffer_flags_e
{
	FLAG_FREE_SLOT = 0,
	FLAG_WRITE_IN_PROGRESS = 1,
	FLAG_FILLED_SLOT = 2,
	FLAG_READ_IN_PROGRESS = 3,
	FLAG_INVALID_JOB = 4
} ring_buffer_flags_e;

struct thread_pool_worker_t
{
	int                  timer;       /* counters for each thread - used for work balancing - use atomics to modify */
	pthread_t            thread;      /* vector of worker thread handles */
	pthread_cond_t       wakeup_cv;   /* condition variables for waking up a worker thread - lock worker_mutex for access */
	pthread_mutex_t      mutex;       /* locks for worker_wakeup_cv */
	struct ring_buffer_t ring_buffer; /* ring buffers for storing jobs, 1 for each thread */
	struct thread_pool_t* pool;       /* the pool that owns this worker */
};

struct thread_pool_t
{
	int             num_threads;        /* number of worker threads to spawn on resume */
	int             num_jobs;           /* number of jobs queued or actively being executed - use atomics to modify */
	int             selected_worker;    /* index of the worker to give an incoming job - NOTE: doesn't wrap, use modulo - use atomics to modify */
	char            active;             /* whether or not the pool is active - use atomics to modify */

	struct thread_pool_worker_t* worker;/* vector of workers */

	pthread_mutex_t mutex;              /* pool mutex - used for num_jobs and active */
	pthread_cond_t  job_finished_cv;    /* condition variable for waking up threads waiting on finished jobs - lock mutex for access */
};

struct thread_pool_job_t
{
	thread_pool_job_func func;
	void* data;
};

/*!
 * @brief Initialises the specified ring buffer object.
 * @param rb The ring buffer object to initialise.
 * @param job_slots The number of jobs to allocate space for in the ring
 * buffer. If 0 is specified then the number of jobs will equal
 * RING_BUFFER_DEFAULT_SLOT_COUNT which can be configured in CMakeLists.txt.
 * @param job_size_in_bytes The size of the objects this ring buffer will be
 * storing in bytes. One can pass the return value of sizeof().
 */
static void
ring_buffer_init_buffer(struct ring_buffer_t* rb,
						uint32_t job_slots,
						intptr_t job_size_in_bytes);

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
thread_pool_worker(struct thread_pool_worker_t* worker);

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
thread_pool_init_pool(struct thread_pool_t* pool,
					  uint32_t num_threads,
					  uint32_t job_slots);

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
thread_pool_create(uint32_t num_threads, uint32_t buffer_size_in_bytes)
{
	/* create and init thread pool object */
	struct thread_pool_t* pool = (struct thread_pool_t*)MALLOC(sizeof(struct thread_pool_t));
	thread_pool_init_pool(pool, num_threads, buffer_size_in_bytes);
	return pool;
}

/* ------------------------------------------------------------------------- */
static void
thread_pool_init_worker(struct thread_pool_worker_t* worker,
						uint32_t job_slots)
{
	ring_buffer_init_buffer(&worker->ring_buffer,
							job_slots,
							sizeof worker->ring_buffer);

	pthread_mutex_init(&worker->mutex, NULL);
	pthread_cond_init(&worker->wakeup_cv, NULL);
}

/* ------------------------------------------------------------------------- */
static void
thread_pool_deinit_worker(struct thread_pool_worker_t* worker)
{
	pthread_cond_destroy(&worker->wakeup_cv);
	pthread_mutex_destroy(&worker->mutex);

	ring_buffer_deinit_buffer(&worker->ring_buffer);
}

/* ------------------------------------------------------------------------- */
static void
thread_pool_init_pool(struct thread_pool_t* pool,
					  uint32_t num_threads,
					  uint32_t job_slots)
{
	int i;

	printf("Thread pool initialising on thread 0x%lx\n", (intptr_t)pthread_self());

	/* set struct memory to a defined state */
	memset(pool, 0, sizeof(struct thread_pool_t));

	/*
	 * Set number of threads to create - if num_threads is 0, set it to the
	 * number of CPU cores present
	 */
	if(num_threads)
		pool->num_threads = num_threads;
	else
		pool->num_threads = get_number_of_cores();

	/* allocate num_threads workers */
	pool->worker = (struct thread_pool_worker_t*)
			MALLOC(sizeof(*pool->worker) * pool->num_threads);
	memset(pool->worker, 0, sizeof(*pool->worker) * pool->num_threads);

	/* initialise workers */
	for(i = 0; i != pool->num_threads; ++i)
	{
		thread_pool_init_worker(&pool->worker[i], job_slots);
		pool->worker[i].pool = pool;
	}

	/* conditional variable and mutex for when workers go to sleep */
	pthread_mutex_init(&pool->mutex, NULL);
	pthread_cond_init(&pool->job_finished_cv, NULL);

	/* launches all worker threads */
	thread_pool_resume(pool);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_destroy(struct thread_pool_t* pool)
{
	int i;

	/* Shut down worker threads. */
	thread_pool_suspend(pool);

	pthread_cond_destroy(&pool->job_finished_cv);
	pthread_mutex_destroy(&pool->mutex);

	for(i = 0; i != pool->num_threads; ++i)
	{
		thread_pool_deinit_worker(&pool->worker[i]);
	}

	FREE(pool);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_queue(struct thread_pool_t* pool, thread_pool_job_func func, void* data)
{
	int selected_worker;
	intptr_t write_pos;
	struct thread_pool_job_t* job;
	struct ring_buffer_t* ring_buffer;
	DATA_POINTER_TYPE* flag_ptr;

	/* Get target worker: This is some primitive load balancing but should work for now */
	selected_worker = __sync_fetch_and_add(&pool->selected_worker, 1) % pool->num_threads;

	/* get the worker thread's ring buffer */
	ring_buffer = &pool->worker[selected_worker].ring_buffer;

	/*
	 * Fetch, increment and wrap write position of target ring buffer. This
	 * operation will acquire an unused, unique position in the ring buffer in
	 * which to insert a new job.
	 */
	write_pos = __sync_fetch_and_add(&ring_buffer->write_pos, 1) %
			ring_buffer->job_slots;

	/* cache flag buffer pointer, it's used multiple times */
	flag_ptr = ring_buffer->flg_buffer + write_pos;

	/*
	 * Try to set the flag to "write in progress" in flag buffer. This
	 * operation will fail if the current flag is not FLAG_FREE_SLOT and will mean
	 * the buffer has overflown.
	 */
	if(!__sync_bool_compare_and_swap(flag_ptr, FLAG_FREE_SLOT, FLAG_WRITE_IN_PROGRESS))
	{
		/*
		 * The buffer has overflown.
		 *
		 * At this point there are two possible things that have happened.
		 *  1) The job at write_pos is pending to be executed by the thread
		 *     we're currently in. Spin-locking until the job is free would
		 *     result in a deadlock and is therefore not an option.
		 *  2) The job at write_pos is pending or currently being processed by
		 *     another thread, in which case it would be possible to wait
		 *     until the slot frees up.
		 *
		 * The easiest thing to do here is to execute this job directly.
		 */
		func(data);

		/*
		 * If in the meantime the job at write_pos has completed (and maybe
		 * there are even more jobs queued up after write_pos, which is
		 * entirely possible seeing as any proceeding calls to this function
		 * will acquire a write_pos after this one), the thread which processed
		 * the job will be dependent on the state of this job.
		 *
		 * We have an obligation to mark this slot as "invalid" - indicating
		 * that this job slot has already been executed - and informing the
		 * worker thread about it.
		 *
		 * If the job is still being processed then there is no need to set
		 * this job as invalid. The worker thread will have no knowledge of
		 * this job ever existing.
		 */
		__sync_bool_compare_and_swap(flag_ptr, FLAG_FREE_SLOT, FLAG_INVALID_JOB);
	}
	else
	{
		/*
		 * Flag has been set to "write in progress", so the job can now be safely
		 * copied into the target buffer.
		 */
		job = (struct thread_pool_job_t*)(ring_buffer->obj_buffer +
				write_pos * ring_buffer->job_size_in_bytes);

		job->func = func;
		job->data = data;

		/* buffer is ready for reading, update flag */
		__sync_bool_compare_and_swap(flag_ptr, FLAG_WRITE_IN_PROGRESS, FLAG_FILLED_SLOT);
	}

	/* wake up all worker threads TODO: Slow as shit, use dummy work loads */
	pthread_mutex_lock(&pool->worker[selected_worker].mutex);
	pthread_cond_broadcast(&pool->worker[selected_worker].wakeup_cv);
	pthread_mutex_unlock(&pool->worker[selected_worker].mutex);
}

/* ------------------------------------------------------------------------- */
/* this is the entry point for worker threads */
static void*
thread_pool_worker(struct thread_pool_worker_t* worker)
{
	struct thread_pool_job_t* job;
	intptr_t read_pos;
	DATA_POINTER_TYPE* flag_ptr;

	/* keep executing jobs until the pool becomes inactive */
	while(__sync_fetch_and_add(&worker->pool->active, 0))
	{
		/*
		 * Fetch, increment and wrap read position of target ring buffer. This
		 * operation will acquire a unique position in the ring buffer in which
		 * to insert a new job.
		 */
		read_pos = __sync_fetch_and_add(&worker->ring_buffer.read_pos, 1) %
				worker->ring_buffer.job_slots;

		/* cache flag pointer, as it's used multiple times */
		flag_ptr = worker->ring_buffer.flg_buffer + read_pos;

		/*
		 * If the job at read_pos is filled, swap the flag with a
		 * "read in progress". This operation will fail if there are no more
		 * jobs.
		 */
		if(!__sync_bool_compare_and_swap(flag_ptr, FLAG_FILLED_SLOT, FLAG_READ_IN_PROGRESS))
		{
			/* signal that a job has been finished */
			pthread_mutex_lock(&worker->pool->mutex);
			pthread_cond_broadcast(&worker->pool->job_finished_cv);
			pthread_mutex_unlock(&worker->pool->mutex);

			/*
			 * Wait for wakeup signal.
			 * Wakeup should only occur if either the pool is shutting down,
			 * or a job is available. Go back to sleep if a wakeup flag was
			 * set by accident.
			 */
			pthread_mutex_lock(&worker->mutex);
			while(__sync_fetch_and_add(&worker->pool->active, 0) &&
			     !__sync_bool_compare_and_swap(flag_ptr, FLAG_FILLED_SLOT, FLAG_READ_IN_PROGRESS))
			{
				pthread_cond_wait(&worker->wakeup_cv, &worker->mutex);
			}

			/* if the pool is no longer active, don't process the job */
			if(!__sync_fetch_and_add(&worker->pool->active, 0))
			{
				/*
				 * Restore unprocessed job - required for suspend/resume.
				 * Decrementing the read position doesn't mean the job was
				 * lost, it just means this thread is giving up ownership
				 * for this particular read position. When the pool is
				 * resumed, the pending jobs will be picked up again.
				 */
				__sync_fetch_and_sub(&worker->ring_buffer.read_pos, 1);

				pthread_mutex_unlock(&worker->mutex);
				break;
			}

			pthread_mutex_unlock(&worker->mutex);
		}

		/* exec job and set flag to free once done */
		job = (struct thread_pool_job_t*)(worker->ring_buffer.obj_buffer +
				read_pos * worker->ring_buffer.job_size_in_bytes);

		job->func(job->data);

		/* job done, flag as free slot */
		__sync_bool_compare_and_swap(flag_ptr, FLAG_READ_IN_PROGRESS, FLAG_FREE_SLOT);
	}

	pthread_mutex_lock(&worker->pool->mutex);
	pthread_cond_broadcast(&worker->pool->job_finished_cv);
	pthread_mutex_unlock(&worker->pool->mutex);
	pthread_exit(NULL);
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
	for(i = 0; i != pool->num_threads; ++i)
	{
		pthread_mutex_lock(&pool->worker[i].mutex);
		/* cv flag is pool->active, which has now been set to 0 atomically */
		pthread_cond_broadcast(&pool->worker[i].wakeup_cv);
		pthread_mutex_unlock(&pool->worker[i].mutex);
		pthread_join(pool->worker[i].thread, NULL);
	}
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

	/* for portability, explicitly create threads in a joinable state */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	/* launch worker threads */
	for(i = 0; i != pool->num_threads; ++i)
	{
		pthread_create(&(pool->worker[i].thread),
					   &attr,
					   (void*(*)(void*))thread_pool_worker,
					   &pool->worker[i]);
	}

	/* clean up */
	pthread_attr_destroy(&attr);
}

/* ------------------------------------------------------------------------- */
void
thread_pool_wait_for_jobs(struct thread_pool_t* pool)
{
	/* wait for number of active jobs to drop to 0 */
	pthread_mutex_lock(&pool->mutex);
	while(__sync_fetch_and_add(&pool->num_jobs, 0))
		pthread_cond_wait(&pool->job_finished_cv, &pool->mutex);
	pthread_mutex_unlock(&pool->mutex);
}

/* ------------------------------------------------------------------------- */
void
ring_buffer_init_buffer(struct ring_buffer_t* rb,
						uint32_t job_slots,
						intptr_t job_size_in_bytes)
{
	memset(rb, 0, sizeof(struct ring_buffer_t));

	rb->job_slots = (job_slots != 0 ? job_slots : RING_BUFFER_DEFAULT_SLOT_COUNT);
	rb->job_size_in_bytes = job_size_in_bytes;

	/*
	 * Allocate memory and set up pointers.
	 * Flag buffer stores job_slots number of flags of type DATA_POINTER_TYPE
	 * Object buffer stores job_slots number of jobs
	 */
	rb->flg_buffer = (DATA_POINTER_TYPE*)MALLOC(
			rb->job_slots * sizeof(DATA_POINTER_TYPE) +                         /* flg buffer */
			rb->job_slots * sizeof(DATA_POINTER_TYPE) * rb->job_size_in_bytes); /* obj buffer */
	rb->obj_buffer = rb->flg_buffer + rb->job_slots;
}

/* ------------------------------------------------------------------------- */
void
ring_buffer_deinit_buffer(struct ring_buffer_t* rb)
{
	FREE(rb->flg_buffer);
	memset(rb, 0, sizeof *rb);
}

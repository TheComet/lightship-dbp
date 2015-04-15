#include "lightship/thread_test.h"
#include "lightship/init.h"
#include "thread_pool/thread_pool.h"
#include "framework/log.h"
#include "framework/services.h"
#include "framework/game.h"
#include "util/time.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void work_empty(void* p)
{
}

void work1(void* p)
{
    volatile int i;
    for(i = 0; i != 10; ++i)
    {
    }
}

void work2(void* p)
{
    volatile int i;
    for(i = 0; i != 100; ++i)
    {
    }
}

void work3(void* p)
{
    volatile int i;
    for(i = 0; i != 1000; ++i)
    {
    }
}

void work4(void* p)
{
    volatile int i;
    for(i = 0; i != 10000; ++i)
    {
    }
}

void work5(void* p)
{
    volatile int i;
    for(i = 0; i != 100000; ++i)
    {
    }
}

void work_recurse(struct thread_pool_t* pool)
{
    volatile int i;
    for(i = 0; i != 1000; ++i)
        if(pool)
            thread_pool_queue(pool, (thread_pool_job_func)work_recurse, NULL);
}
#include <math.h>
struct pos_t
{
    int x;
    int y;
};
struct mandel_t
{
    struct pos_t pos;
    unsigned char* pixel_buffer;
    int w;
    int h;
    double zoom, moveX, moveY;
    int max_iterations;
};
void
work_mandel(struct mandel_t* m)
{
    double pr, pi;
    double newRe, newIm, oldRe, oldIm;
    int i;
    uint32_t* colourp;
    
    colourp = (uint32_t*)m->pixel_buffer + (m->pos.y * m->w + m->pos.x);

    /* 
     * calculate the initial real and imaginary part of z, based on
     * the pixel location and zoom and position values
     */
    pr = 1.5 * (m->pos.x - m->w / 2) / (0.5 * m->zoom * m->w) + m->moveX;
    pi = (m->pos.y - m->h / 2) / (0.5 * m->zoom * m->h) + m->moveY;
    newRe = newIm = oldRe = oldIm = 0; /* these should start at 0,0*/
    /*
     * "i" will represent the number of iterations
     * start the iteration process
     */
    for(i = 0; i < m->max_iterations; i++)
    {
        /* remember value of previous iteration */
        oldRe = newRe;
        oldIm = newIm;
        /* the actual iteration, the real and imaginary part are calculated */
        newRe = oldRe * oldRe - oldIm * oldIm + pr;
        newIm = 2 * oldRe * oldIm + pi;
        /* if the point is outside the circle with radius 2: stop */
        if((newRe * newRe + newIm * newIm) > 4) 
            break;
    }
    
    if(i == m->max_iterations)
        *colourp = 0xFF000000; /* black */
    else
    {
        double z = sqrt(newRe * newRe + newIm * newIm);
        int brightness = 255. * log2(1.75 + i*5 - log2(log2(z))) / log2((double)m->max_iterations);
        *colourp = 0xFF000000 | (brightness << 16) | (brightness << 8);
    }
    
    free(m);
}

void
do_thread_test(struct game_t* game)
{
    int i;
    struct thread_pool_t* pool;
    int64_t timer1, timer2;
    
    thread_pool_set_max_buffer_size(0x4000000); /* 1 GB */

    puts("=======================================");
    puts("EMPTY TEST (1,000,000 x 1)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 1000000; ++i)
        thread_pool_queue(pool, work_empty, NULL);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("LOAD TEST 1 (1,000,000 x 10)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 1000000; ++i)
        thread_pool_queue(pool, work1, NULL);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("LOAD TEST 2 (1,000,000 x 100)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 1000000; ++i)
        thread_pool_queue(pool, work2, NULL);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("LOAD TEST 3 (1,000,000 x 1,000)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 1000000; ++i)
        thread_pool_queue(pool, work3, NULL);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("LOAD TEST 4 (1,000,000 x 10,000)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 1000000; ++i)
        thread_pool_queue(pool, work4, NULL);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("LOAD TEST 5 (100,000 x 100,000)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 100000; ++i)
        thread_pool_queue(pool, work5, NULL);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("RECURSIVE INSERT (1000^2)");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    for(i = 0; i != 1000; ++i)
        thread_pool_queue(pool, (thread_pool_job_func)work_recurse, pool);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);
    
    puts("=======================================");
    puts("CALCULATING MANDELBROT (1536x1536, maxiter=10000");
    puts("=======================================");
    pool = thread_pool_create(0, 1536*1536);
    timer1 = timer2 = get_time_in_microseconds();
    {
        int x, y;
        struct mandel_t m_b;
        m_b.w = 1536;
        m_b.h = 1536;
        m_b.pixel_buffer = malloc(m_b.w * m_b.h * 4);
        m_b.zoom = 1;
        m_b.moveX = -0.5;
        m_b.moveY = 0;
        m_b.max_iterations = 10000;
        for(y = 0; y != m_b.h; ++y)
        {
            for(x = 0; x != m_b.w; ++x)
            {
                struct mandel_t* m = malloc(sizeof(struct mandel_t));
                memcpy(m, &m_b, sizeof(struct mandel_t));
                m->pos.x = x;
                m->pos.y = y;
                thread_pool_queue(pool, (thread_pool_job_func)work_mandel, m);
            }
        }
        printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
        thread_pool_wait_for_jobs(pool);
        printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
        
        {
            uint16_t one = 1;
            uint32_t sprite;
            uint32_t font_id, text_id;
            char text_centered = 1;
            uint32_t text_size = 9;
            float text_pos_x = 0;
            float text_pos_y = 0.8;
            SERVICE_CALL_NAME6(game,
                               "renderer_gl.sprite_create_from_memory",
                               &sprite,
                               PTR(m_b.pixel_buffer),
                               m_b.w, m_b.h,
                               one, one, one);
            SERVICE_CALL_NAME2(game,
                               "renderer_gl.text_group_create",
                               &font_id,
                               "../../plugins/core/menu/ttf/DejaVuSans.ttf",
                               text_size);
            SERVICE_CALL_NAME2(game,
                               "renderer_gl.text_group_load_character_set",
                               SERVICE_NO_RETURN,
                               font_id,
                               PTR(NULL));
            SERVICE_CALL_NAME5(game,
                               "renderer_gl.text_create",
                               &text_id,
                               font_id,
                               text_centered,
                               text_pos_x, text_pos_y,
                               L"Close Window to Exit");
        }
        games_run_all();
        deinit();
        init();
        
        free(m_b.pixel_buffer);
    }
    thread_pool_destroy(pool);
}
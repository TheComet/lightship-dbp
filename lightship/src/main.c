#include <stdio.h>
#include <string.h>
#include "plugin_manager/plugin_manager.h"
#include "plugin_manager/services.h"
#include "plugin_manager/events.h"
#include "plugin_manager/plugin.h"
#include "plugin_manager/game.h"
#include "util/unordered_vector.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include "util/pstdint.h"

#include "util/map.h"

#ifdef _DEBUG
static const char* yml_core_plugins = "../../lightship/cfg/core-plugins.yml";
static const char* yml_entry_point = "../../lightship/cfg/entry-point.yml";
#else
static const char* yml_core_plugins = "cfg/core-plugins.yml";
static const char* yml_entry_point = "cfg/entry-point.yml";
#endif

static struct plugin_t* g_plugin_yaml = NULL;
static struct game_t* g_local_game = NULL;

typedef void (*start_loop_func)(void);

void
init(void)
{
    struct plugin_info_t target;
    
    /*
     * Enable logging as soon as possible
     */
    llog_init();
    
    /*
     * Create the local game instance. This is the context that holds all
     * plugins, services, and events together.
     */
    g_local_game = game_create("localhost");
    if(!g_local_game)
        return;
    
    /*
     * Services and events should be initialised before anything else, as they
     * register built-in mechanics that are required throughout the rest of the
     * program (such as the log event).
     */
    services_init(g_local_game);
    events_init(g_local_game);
    
    /*!
     * Load the YAML plugin. This is required so the plugin manager can parse
     * the core plugins config file and load the core plugins. If the plugin
     * fails to load, bail.
     */
    target.name = "yaml";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    g_plugin_yaml = plugin_load(g_local_game, &target, PLUGIN_VERSION_MINIMUM);
    if(!g_plugin_yaml)
        return;
    if(plugin_start(g_local_game, g_plugin_yaml) == PLUGIN_FAILURE)
    {
        llog(LOG_FATAL, NULL, 1, "Failed to start YAML plugin");
        return;
    }

    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    if(!load_plugins_from_yaml(g_local_game, yml_core_plugins))
    {
        llog(LOG_FATAL, NULL, 1, "Couldn't start all core plugins");
        return;
    }

    /* 
     * Try to get the main loop service and start running the game
     */
    {
        char* start_service_name;
        uint32_t doc_ID;
        struct service_t* start;
        const char* entry_point_key = "service";

        /* load the yaml file */
        SERVICE_CALL_NAME1(g_local_game, "yaml.load", &doc_ID, PTR(yml_entry_point));
        if(!doc_ID)
        {
            llog(LOG_FATAL, NULL, 1, "Cannot get main loop service");
            return;
        }
        
        /* search for the entry point key and retrieve its value, which is the name of the service to start with */
        SERVICE_CALL_NAME2(g_local_game, "yaml.get_value", &start_service_name, doc_ID, PTR(entry_point_key));
        if(!start_service_name)
        {
            llog(LOG_FATAL, NULL, 3, "Cannot get value of \"service\" in \"", yml_entry_point ,"\"");
            SERVICE_CALL_NAME1(g_local_game, "yaml.destroy", SERVICE_NO_RETURN, doc_ID);
            return;
        }

        /* with the service name retrieved, try to call it */
        start = service_get(g_local_game, start_service_name);
        SERVICE_CALL_NAME1(g_local_game, "yaml.destroy", SERVICE_NO_RETURN, doc_ID);
        if(!start)
        {
            llog(LOG_FATAL, NULL, 1, "Cannot get main loop service");
            return;
        }
        
        {
            struct menu_t;
            struct menu_t* menu;
            struct service_t* menu_load_service = service_get(g_local_game, "menu.load");
            struct service_t* menu_destroy_service = service_get(g_local_game, "menu.destroy");
#ifdef _DEBUG
            const char* menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
            const char* menu_file_name = "cfg/menu.yml";
#endif
            SERVICE_CALL1(menu_load_service, &menu, PTR(menu_file_name));
            SERVICE_CALL0(start, SERVICE_NO_RETURN);
            SERVICE_CALL1(menu_destroy_service, SERVICE_NO_RETURN, PTR(menu));
        }
    
    }
}

void
deinit(void)
{
    plugin_manager_deinit(g_local_game);
    llog_deinit();
    events_deinit(g_local_game);
    services_deinit();
    game_destroy(g_local_game);
}

#include "thread_pool/thread_pool.h"
#include "util/time.h"
#include <stdlib.h>
#include <unistd.h>
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
            thread_pool_queue(pool, (void(*)(void*))work_recurse, NULL);
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
    
    colourp = (uint32_t*)(m->pixel_buffer + (m->pos.x * m->h + m->pos.y));

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
        *colourp = 0x00000000; /* black */
    else
    {
        double z = sqrt(newRe * newRe + newIm * newIm);
        int brightness = 255. * log2(1.75 + i - log2(log2(z))) / log2((double)m->max_iterations);
        *colourp = (brightness << 16) | (brightness << 8) | 0xFF;
    }
    
    free(m);
}

void
do_thread_test()
{
    int i;
    struct thread_pool_t* pool;
    int64_t timer1, timer2;
    
    thread_pool_set_max_buffer_size(0x4000000); /* 1 GB */
/*
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
        thread_pool_queue(pool, (void(*)(void*))work_recurse, pool);
    printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
    thread_pool_wait_for_jobs(pool);
    printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
    thread_pool_destroy(pool);*/
    
    puts("=======================================");
    puts("CALCULATING MANDELBROT (1920x1080, maxiter=10000");
    puts("=======================================");
    pool = thread_pool_create(0, 1000000);
    timer1 = timer2 = get_time_in_microseconds();
    {
        int x, y;
        struct mandel_t m_b;
        m_b.w = 1920;
        m_b.h = 1080;
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
                thread_pool_queue(pool, (void(*)(void*))work_mandel, m);
            }
        }
        printf("insertion time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer1) * 0.001));
        thread_pool_wait_for_jobs(pool);
        printf("job time (ms): %ld\n", (int64_t)((get_time_in_microseconds() - timer2) * 0.001));
        free(m_b.pixel_buffer);
    }
    thread_pool_destroy(pool);
}

int
main(int argc, char** argv)
{
    puts("=========================================");
    puts("Starting lightship");
    puts("=========================================");

    /* first thing - initialise memory management */
    memory_init();
    do_thread_test();
    memory_deinit();
    return 0;

    /* initialise everything else */
    init();

    /* clean up */
    deinit();

    /* de-init memory management and print report */
    memory_deinit();

    return 0;
}

#include "lightship/argv.h"
#include "util/memory.h"
#include "util/log.h"
#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
void
show_help(char* program_name)
{
    printf("Usage: %s [OPTIONS]...\n", program_name);
    puts("\nGeneral Options:");
    printf("  %-30sPrint this help message\n",          "-h, --help");
    printf("  %-30sRun in server mode. This causes the "
                  "game to run as a dedicated server, allowing "
                  "multiple game instances to be hosted "
                  "simultaniously\n",                   "-s, --server");
    printf("  %-30sThe startup config file to use\n",   "-c, --config <FILE>");
}

/* -------------------------------------------------------------------------- */
struct arg_obj_t*
argv_parse(int argc, char** argv)
{
    struct arg_obj_t* arg_obj;
    char* arg;
    int i;
    int switch_type;
    
    static const int NO_DASH = 0;
    static const int SINGLE_DASH = 1;
    static const int DOUBLE_DASH = 2;
    
    /* allocate argument object (return value) */
    arg_obj = (struct arg_obj_t*)MALLOC(sizeof(struct arg_obj_t));
    if(!arg_obj)
        OUT_OF_MEMORY("argv_parse()", NULL);
    memset(arg_obj, 0, sizeof(struct arg_obj_t));
    
    /* game should be run by default */
    arg_obj->run_game = 1;
    
    /* for every argument */
    for(i = 1; i != argc; ++i)
    {
        arg = argv[i]; /* point to beginning of current argument */
        
        /* determine switch type */
        switch_type = NO_DASH;
        if(argv[i][0] == '-')
        {
            switch_type = SINGLE_DASH;
            ++arg; /* ignore first dash */
            if(strlen(argv[i]) >= 2 && argv[i][1] == '-')
            {
                switch_type = DOUBLE_DASH;
                ++arg; /* ignore second dash */
            }
        }
        
        /* single dash? */
        if(switch_type == SINGLE_DASH)
        {
            /* For every character in argument */
            for(; *arg; ++arg)
            {
                if(*arg == 'h')
                    arg_obj->show_help = 1;
            }
        }
        
        /* double dash? */
        if(switch_type == DOUBLE_DASH)
        {
            if(strcmp(arg, "help") == 0)
                arg_obj->show_help = 1;
        }
    }
    
    /* option conflicts/dependencies */
    if(arg_obj->show_help)     /* if the user requested help, don't start the game */
        arg_obj->run_game = 0;
    
    /* we are responsible to show help */
    if(arg_obj->show_help)
        show_help(argv[0]);
    
    return arg_obj;
}

/* -------------------------------------------------------------------------- */
void
argv_free(struct arg_obj_t* arg)
{
    FREE(arg);
}

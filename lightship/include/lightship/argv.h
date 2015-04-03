struct arg_obj_t
{
    char run_game;
    char show_help;
    char is_server;
};

struct arg_obj_t*
argv_parse(int argc, char** argv);

void
argv_free(struct arg_obj_t* argv);

struct game_t;
extern struct game_t* g_localhost;

char
init(void);

char
init_game(char is_server);

void
run_game(void);

void
deinit(void);

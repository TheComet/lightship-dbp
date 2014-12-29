#include "util/event_api.h"

struct GLFWwindow;

struct window_t
{
    int width;
    int height;
    struct GLFWwindow* window;
};

char
window_init(void);

void
window_deinit(void);

EVENT_LISTENER(on_render);

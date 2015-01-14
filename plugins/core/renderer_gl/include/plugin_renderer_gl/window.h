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

int
window_width(void);

int
window_height(void);

EVENT_LISTENER0(on_render);

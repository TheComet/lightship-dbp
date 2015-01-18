#include "util/event_api.h"
#include "util/pstdint.h"

struct GLFWwindow;

struct window_t
{
    uint32_t width;
    uint32_t height;
    struct GLFWwindow* window;
};

char
window_init(void);

void
window_deinit(void);

uint32_t
window_width(void);

uint32_t
window_height(void);

EVENT_LISTENER0(on_render);

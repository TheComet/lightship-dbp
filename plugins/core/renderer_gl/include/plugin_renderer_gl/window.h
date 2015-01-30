#include "util/event_api.h"
#include "util/pstdint.h"
#include "util/service_api.h"

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

SERVICE(window_width_wrapper);
SERVICE(window_height_wrapper);

EVENT_LISTENER0(on_render);

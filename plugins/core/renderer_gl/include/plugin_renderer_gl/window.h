#include "framework/se_api.h"
#include "util/pstdint.h"

struct GLFWwindow;
struct context_t;

struct window_t
{
	uint32_t width;
	uint32_t height;
	struct GLFWwindow* window;
};

char
window_init(struct context_t* context);

void
window_deinit(void);

uint32_t
window_width(void);

uint32_t
window_height(void);

SERVICE(window_width_wrapper);
SERVICE(window_height_wrapper);

EVENT_LISTENER(on_render);

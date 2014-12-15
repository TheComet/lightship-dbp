struct GLFWWindow;

struct window_t
{
    int width;
    int height;
    struct GLFWWindow* window;
};

struct window_t* window_create(void);
void window_destroy(struct window_t* window);
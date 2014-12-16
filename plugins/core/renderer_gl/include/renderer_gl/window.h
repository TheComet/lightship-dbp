struct GLFWwindow;

struct window_t
{
    int width;
    int height;
    struct GLFWwindow* window;
};

struct window_t* window_create(void);
void window_destroy(struct window_t* window);
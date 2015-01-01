#include "util/pstdint.h"

#define BUTTON_COLOUR_NORMAL 0xFFFFFFFF

struct vec2_t
{
    float x;
    float y;
};

struct button_t
{
    uint32_t ID;
    uint32_t shapes_normal_ID;
    char* text;
    struct vec2_t pos;
    struct vec2_t size;
};

void button_init(void);
void button_deinit(void);
uint32_t button_create(const char* text, float x, float y, float width, float height);
void button_destroy(uint32_t ID);
void button_destroy_all(void);

#include "plugin_renderer_gl/glutils.h"
#include "util/pstdint.h"
#include "framework/service_api.h"

typedef enum sprite_animation_e
{
    SPRITE_ANIMATION_STOP,
    SPRITE_ANIMATION_PLAY,
    SPRITE_ANIMATION_LOOP
} sprite_animation_e;

struct sprite_animation_t
{
    uint16_t current_frame;
    uint16_t frame_a;
    uint16_t frame_b;
    uint16_t total_frame_count;
    sprite_animation_e state;
};

struct sprite_gl_t
{
    GLuint tex;
};

struct sprite_t
{
    uint32_t id;

    struct vec2_t pos;
    struct vec2_t size;
    struct vec2_t frame_size;
    struct sprite_animation_t animation;
    struct sprite_gl_t gl;

    float aspect_ratio;
    char is_visible;
};

char
sprite_init(void);

void
sprite_deinit(void);

struct sprite_t*
sprite_create(const char* file_name,
              uint16_t x_frame_count,
              uint16_t y_frame_count,
              uint16_t total_frame_count,
              uint32_t* id);

struct sprite_t*
sprite_create_from_memory(const unsigned char* pixel_buffer,
                          uint16_t img_width,
                          uint16_t img_height,
                          uint16_t x_frame_count,
                          uint16_t y_frame_count,
                          uint16_t total_frame_count,
                          uint32_t* id);

void
sprite_destroy(struct sprite_t* sprite);

void
sprite_set_loop_a_b(struct sprite_t* sprite, uint16_t a, uint16_t b);

void
sprite_play(struct sprite_t* sprite);

void
sprite_loop(struct sprite_t* sprite);

void
sprite_stop(struct sprite_t* sprite);

void
sprite_set_fps(struct sprite_t* sprite, float fps);

void
sprite_set_position(struct sprite_t* sprite, float x, float y);

void
sprite_set_size(struct sprite_t* sprite, float x, float y);

void
sprite_scale(struct sprite_t* sprite, float factor);

void
sprite_show_hide(struct sprite_t* sprite, char is_visible);

void
sprite_draw(void);

SERVICE(sprite_create_wrapper);
SERVICE(sprite_create_from_memory_wrapper);
SERVICE(sprite_destroy_wrapper);
SERVICE(sprite_position_wrapper);
SERVICE(sprite_scale_wrapper);

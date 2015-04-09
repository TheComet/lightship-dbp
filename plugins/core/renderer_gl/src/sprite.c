#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/sprite.h"
#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/glob.h"
#include "framework/game.h"
#include "framework/log.h"
#include "util/map.h"
#include "util/memory.h"
#include <assert.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static struct map_t g_sprites;
static uint32_t guid = 1;
static GLuint g_vao;
static GLuint g_vbo;
static GLuint g_ibo;
static GLuint g_sprite_shader_id;

static GLuint g_uniform_sprite_position_location;
static GLuint g_uniform_sprite_size_location;

static struct vertex_quad_t g_quad_vertex_data[4] = {
    {{-1, -1}, {0,  1}},
    {{-1,  1}, {0,  0}},
    {{ 1, -1}, {1,  1}},
    {{ 1,  1}, {1,  0}}
};

static INDEX_DATA_TYPE g_quad_index_data[6] = {
    0, 2, 1,
    1, 3, 2
};

#ifdef _DEBUG
static const char* sprite_shader_file = "../../plugins/core/renderer_gl/fx/sprite";
#else
static const char* sprite_shader_file = "fx/sprite";
#endif

/* ------------------------------------------------------------------------- */
char
sprite_init(struct glob_t* g)
{
    map_init_map(&g_sprites);
    
    g_sprite_shader_id = shader_load(g, sprite_shader_file);printOpenGLError();
    g_uniform_sprite_position_location = glGetUniformLocation(g_sprite_shader_id, "spritePosition");
    g_uniform_sprite_size_location     = glGetUniformLocation(g_sprite_shader_id, "spriteSize");
    
    glGenVertexArrays(1, &g_vao);printOpenGLError();
    glBindVertexArray(g_vao);
        glGenBuffers(1, &g_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_data), g_quad_vertex_data, GL_STATIC_DRAW);
            VERTEX_QUAD_SETUP_ATTRIBS
        glGenBuffers(1, &g_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_quad_index_data), g_quad_index_data, GL_STATIC_DRAW);
    glBindVertexArray(0);

    return 1;
}

/* ------------------------------------------------------------------------- */
void
sprite_deinit(void)
{
    while(g_sprites.vector.count)
    {
        sprite_destroy(
            (struct sprite_t*)((struct map_key_value_t*)g_sprites.vector.data)->value
        );
    }
    map_clear_free(&g_sprites);
}

/* ------------------------------------------------------------------------- */
struct sprite_t*
sprite_create(struct glob_t* g,
              const char* file_name,
              uint16_t x_frame_count,
              uint16_t y_frame_count,
              uint16_t total_frame_count,
              uint32_t* id)
{
    unsigned char* pixel_buffer;
    int x, y, n;
    struct sprite_t* sprite;
    
    assert(file_name);
    assert(x_frame_count >= 1);
    assert(y_frame_count >= 1);
    assert(total_frame_count >= 1);

    /* force 4 channel data for easy texture upload */
    pixel_buffer = stbi_load(file_name, &x, &y, &n, 4);
    if(!pixel_buffer)
    {
        llog(LOG_ERROR, g->game, PLUGIN_NAME, 3, "Failed to load image: \"", file_name, "\"");
        return NULL;
    }
    
    sprite = sprite_create_from_memory(pixel_buffer, x, y, x_frame_count, y_frame_count, total_frame_count, id);
    stbi_image_free(pixel_buffer);
    
    return sprite;
}

/* ------------------------------------------------------------------------- */
struct sprite_t*
sprite_create_from_memory(const unsigned char* pixel_buffer,
                          uint16_t img_width,
                          uint16_t img_height,
                          uint16_t x_frame_count,
                          uint16_t y_frame_count,
                          uint16_t total_frame_count,
                          uint32_t* id)
{
    struct sprite_t* sprite;
    
    assert(pixel_buffer);
    assert(x_frame_count >= 1);
    assert(y_frame_count >= 1);
    assert(total_frame_count >= 1);
    
    /* create and set up sprite object */
    sprite = (struct sprite_t*)MALLOC(sizeof(struct sprite_t));
    memset(sprite, 0, sizeof(struct sprite_t));
    *id = guid++;
    map_insert(&g_sprites, *id, sprite);

    sprite->animation.state = SPRITE_ANIMATION_STOP;
    sprite->animation.frame_b = total_frame_count;
    sprite->animation.total_frame_count = total_frame_count;
    sprite->aspect_ratio = (float)img_width / (float)img_height;
    if(img_width > img_height)
    {
        sprite->size.x = 1.0;
        sprite->size.y = sprite->size.x / sprite->aspect_ratio;
    }
    else
    {
        sprite->size.y = 1.0;
        sprite->size.x = sprite->size.y * sprite->aspect_ratio;
    }
    sprite->frame_size.x = sprite->size.x;
    sprite->frame_size.y = sprite->size.y;
    sprite->is_visible = 1;
    
    /* create GL texture and hand over pixel data */
    glGenTextures(1, &sprite->gl.tex);printOpenGLError();
    glBindTexture(GL_TEXTURE_2D, sprite->gl.tex);printOpenGLError();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_buffer);printOpenGLError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);printOpenGLError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);printOpenGLError();
    
    return sprite;
}

/* ------------------------------------------------------------------------- */
void
sprite_destroy(struct sprite_t* sprite)
{
    assert(sprite);

    glDeleteTextures(1, &sprite->gl.tex);
    map_erase_element(&g_sprites, sprite);
    FREE(sprite);
}

/* ------------------------------------------------------------------------- */
void
sprite_set_loop_a_b(struct sprite_t* sprite, uint16_t a, uint16_t b)
{
}

/* ------------------------------------------------------------------------- */
void
sprite_play(struct sprite_t* sprite)
{
}

/* ------------------------------------------------------------------------- */
void
sprite_loop(struct sprite_t* sprite)
{
}

/* ------------------------------------------------------------------------- */
void
sprite_stop(struct sprite_t* sprite)
{
}

/* ------------------------------------------------------------------------- */
void
sprite_set_fps(struct sprite_t* sprite, float fps)
{
}

/* ------------------------------------------------------------------------- */
void
sprite_set_position(struct sprite_t* sprite, float x, float y)
{
    sprite->pos.x = x;
    sprite->pos.y = y;
}

/* ------------------------------------------------------------------------- */
void
sprite_set_size(struct sprite_t* sprite, float x, float y)
{
    sprite->size.x = x;
    sprite->size.y = y;
    sprite->aspect_ratio = x / y;
}

/* ------------------------------------------------------------------------- */
void
sprite_scale(struct sprite_t* sprite, float factor)
{
    sprite->size.y *= factor;
    sprite->size.x = sprite->size.y * sprite->aspect_ratio;
}

/* ------------------------------------------------------------------------- */
void
sprite_show_hide(struct sprite_t* sprite, char is_visible)
{
}

/* ------------------------------------------------------------------------- */
void
sprite_draw(void)
{
    glEnable(GL_BLEND);printOpenGLError();
    glUseProgram(g_sprite_shader_id);printOpenGLError();
    glBindVertexArray(g_vao);printOpenGLError();
    { MAP_FOR_EACH(&g_sprites, struct sprite_t, key, sprite)
    {
        glBindTexture(GL_TEXTURE_2D, sprite->gl.tex);printOpenGLError();
        glUniform2f(g_uniform_sprite_position_location, sprite->pos.x, sprite->pos.y);
        glUniform2f(g_uniform_sprite_size_location, sprite->size.x, sprite->size.y);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);printOpenGLError();
    }}
    glBindVertexArray(0);printOpenGLError();
    glUseProgram(0);
    glDisable(GL_BLEND);printOpenGLError();
}

/* ------------------------------------------------------------------------- */
/* WRAPPERS */
/* ------------------------------------------------------------------------- */
SERVICE(sprite_create_wrapper)
{
    uint32_t id;
    SERVICE_EXTRACT_ARGUMENT_PTR(0, file_name, const char*);
    SERVICE_EXTRACT_ARGUMENT(1, x_frame_count, uint16_t, uint16_t);
    SERVICE_EXTRACT_ARGUMENT(2, y_frame_count, uint16_t, uint16_t);
    SERVICE_EXTRACT_ARGUMENT(3, total_frame_count, uint16_t, uint16_t);
    struct glob_t* g = get_global(service->game);
    
    if(sprite_create(g, file_name, x_frame_count, y_frame_count, total_frame_count, &id))
        SERVICE_RETURN(id, uint32_t);
    SERVICE_RETURN(0, uint32_t);
}

/* ------------------------------------------------------------------------- */
SERVICE(sprite_create_from_memory_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT_PTR(0, pixel_buffer, const unsigned char*);
    SERVICE_EXTRACT_ARGUMENT(1, img_width, uint16_t, uint16_t);
    SERVICE_EXTRACT_ARGUMENT(2, img_height, uint16_t, uint16_t);
    SERVICE_EXTRACT_ARGUMENT(3, x_frame_count, uint16_t, uint16_t);
    SERVICE_EXTRACT_ARGUMENT(4, y_frame_count, uint16_t, uint16_t);
    SERVICE_EXTRACT_ARGUMENT(5, total_frame_count, uint16_t, uint16_t);
    uint32_t id;
    
    if(sprite_create_from_memory(pixel_buffer, img_width, img_height, x_frame_count, y_frame_count, total_frame_count, &id))
        SERVICE_RETURN(id, uint32_t);
    SERVICE_RETURN(0, uint32_t);
}

/* ------------------------------------------------------------------------- */
SERVICE(sprite_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    struct sprite_t* sprite = map_find(&g_sprites, id);
    if(sprite)
        sprite_destroy(sprite);
}

/* ------------------------------------------------------------------------- */
SERVICE(sprite_position_wrapper)
{
}

/* ------------------------------------------------------------------------- */
SERVICE(sprite_scale_wrapper)
{
}

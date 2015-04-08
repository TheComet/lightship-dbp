#include "util/pstdint.h"
#include "framework/service_api.h"
#include "yaml/yaml.h"
#include <stdio.h>

struct yaml_parser_t;
struct yaml_event_t;
struct ptree_t;

struct yaml_doc_t
{
    uint32_t ID;
    struct ptree_t* dom;
};

void
parser_init(struct game_t* game);

void
parser_deinit(struct game_t* game);

uint32_t
yaml_load(struct game_t* game, const char* filename);

struct ptree_t*
yaml_get_dom(struct game_t* game, uint32_t ID);

const char*
yaml_get_value(struct game_t* game, const uint32_t ID, const char* key);

struct ptree_t*
yaml_get_node(struct game_t* game, const uint32_t ID, const char* key);

void
yaml_destroy(struct game_t* game, const uint32_t ID);

SERVICE(yaml_load_wrapper);
SERVICE(yaml_get_dom_wrapper);
SERVICE(yaml_get_value_wrapper);
SERVICE(yaml_get_node_wrapper);
SERVICE(yaml_destroy_wrapper);

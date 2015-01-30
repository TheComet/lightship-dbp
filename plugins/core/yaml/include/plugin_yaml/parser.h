#include "util/pstdint.h"
#include "util/service_api.h"
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
parser_init(void);

void
parser_deinit(void);

uint32_t
yaml_load(const char* filename);

struct ptree_t*
yaml_get_dom(uint32_t ID);

const char*
yaml_get_value(const uint32_t ID, const char* key);

void
yaml_destroy(const uint32_t ID);

SERVICE(yaml_load_wrapper);
SERVICE(yaml_get_dom_wrapper);
SERVICE(yaml_get_value_wrapper);
SERVICE(yaml_destroy_wrapper);

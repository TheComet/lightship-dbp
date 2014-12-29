#include "util/pstdint.h"
#include "yaml/yaml.h"
#include <stdio.h>

struct yaml_parser_t;
struct yaml_event_t;

struct yaml_doc_t
{
    uint32_t ID;
    struct ptree_t* dom;
};

void
parser_init(void);

uint32_t
yaml_load(const char* filename);

char*
yaml_get(const uint32_t ID, const char* key);

void
yaml_destroy(const uint32_t ID);

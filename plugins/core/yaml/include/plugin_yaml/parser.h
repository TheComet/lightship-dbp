#include "util/pstdint.h"
#include "yaml/yaml.h"
#include <stdio.h>

struct yaml_parser_t;
struct yaml_event_t;

struct yaml_doc_t
{
    uint32_t ID;
    uint32_t file_size;
    char* text;
    yaml_parser_t parser;
    yaml_event_t event;
};

void parser_init(void);
uint32_t yaml_open(const char* filename);
void yaml_close(const char ID);

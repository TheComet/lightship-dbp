#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

struct yaml_parser_t;
struct yaml_event_t;
struct ptree_t;

struct yaml_doc_t
{
    uint32_t ID;
    struct ptree_t* dom;
};

void
yaml_init(void);

void
yaml_deinit(void);

struct yaml_doc_t*
yaml_load(const char* filename);

const char*
yaml_get_value(struct yaml_doc_t* doc, const char* key);

const struct ptree_t*
yaml_get_node(struct yaml_doc_t* doc, const char* key);

void
yaml_destroy(struct yaml_doc_t* doc);

C_HEADER_END

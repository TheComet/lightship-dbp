#include "util/unordered_vector.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/ptree.h"
#include "util/string.h"
#include "util/hash.h"
#include "plugin_yaml/parser.h"

struct unordered_vector_t g_open_docs;
static uint32_t GUID_counter = 1;

void
parser_init(void)
{
    unordered_vector_init_vector(&g_open_docs, sizeof(struct yaml_doc_t));
}

void
parser_deinit(void)
{
    unordered_vector_clear_free(&g_open_docs);
}

char
yaml_load_into_ptree(struct ptree_t* tree, struct ptree_t* root_tree, yaml_parser_t* parser)
{
    yaml_event_t event;
    char* key;
    char finished = 0;
    const char FINISH_ERROR = 1;
    const char FINISH_SUCCESS = 2;

    if(!yaml_parser_parse(parser, &event))
    {
        char error[16];
        sprintf(error, "%d", parser->error);
        llog(LOG_ERROR, 2, "Parser error ", error);
        return 0;
    }

    key = NULL;
    while(event.type != YAML_STREAM_END_EVENT)
    {
        char result;

        switch(event.type)
        {
            case YAML_NO_EVENT:
                llog(LOG_ERROR, 1, "Syntax error in yaml document");
                finished = FINISH_ERROR;
                break;

            /* stream/document start/end */
            case YAML_STREAM_START_EVENT:
            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_START_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                break;

            /* begin of sequence of mapping */
            case YAML_SEQUENCE_START_EVENT:
            case YAML_MAPPING_START_EVENT:
                if(key)
                {
                    struct ptree_t* child = ptree_add_node(tree, key, NULL);
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    result = yaml_load_into_ptree(child, root_tree, parser);
                    FREE(key);
                    key = NULL;
                    if(!result)
                    {
                        llog(LOG_ERROR, 1, "Failed to copy ptree (yaml anchor failed)");
                        finished = FINISH_ERROR;
                    }
                }
                break;

            /* end of sequence or mapping */
            case YAML_SEQUENCE_END_EVENT:
            case YAML_MAPPING_END_EVENT:
                finished = FINISH_SUCCESS;
                break;
                
            /* alias */
            case YAML_ALIAS_EVENT:
                if(key)
                {
                    const struct ptree_t* source = ptree_find_by_key(root_tree, (char*)event.data.alias.anchor);
                    if(source)
                        ptree_duplicate_tree(tree, source, key);
                    FREE(key);
                    key = NULL;
                }
                break;

            /* scalar */
            case YAML_SCALAR_EVENT:
                if(key)
                {
                    struct ptree_t* child = ptree_add_node(tree, key, malloc_string((char*)event.data.scalar.value));
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    FREE(key);
                    key = NULL;
                }
                else
                {
                    key = malloc_string((char*)event.data.scalar.value);
                }
                break;
            
            default:
                llog(LOG_ERROR, 1, "Unknown error");
                finished = FINISH_ERROR;
                break;

        }
        
        if(finished)
            break;

        yaml_event_delete(&event);
        yaml_parser_parse(parser, &event);
    }
    
    /* clean up */
    yaml_event_delete(&event);
    if(key)
        FREE(key);
    
    if(finished == FINISH_ERROR)
        return 0;

    return 1;
}

static struct yaml_doc_t*
yaml_get_doc(uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t, doc)
    {
        if(doc->ID == ID)
            return doc;
    }
    return NULL;
}

uint32_t
yaml_load(const char* filename)
{
    FILE* fp;
    struct yaml_doc_t* doc;
    struct ptree_t* tree;
    yaml_parser_t parser;
    
    /* try to open the file */
    fp = fopen(filename, "rb");
    if(!fp)
    {
        llog(LOG_ERROR, 3, "Failed to open file \"", filename, "\"");
        return 0;
    }
    
    /* parse file and load into dom tree */
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, fp);
    tree = ptree_create("root", NULL);
    if(!yaml_load_into_ptree(tree, tree, &parser))
    {
        yaml_parser_delete(&parser);
        fclose(fp);
        ptree_destroy(tree);
        llog(LOG_ERROR, 3, "Syntax error: Failed to parse YAML file \"", filename, "\"");
        return 0;
    }
    
    /* create doc object and initialise parser */
    doc = (struct yaml_doc_t*)unordered_vector_push_emplace(&g_open_docs);
    doc->ID = GUID_counter++;
    doc->dom = tree;
    
    /* clean up */
    yaml_parser_delete(&parser);
    fclose(fp);
    
    return doc->ID;
}

struct ptree_t*
yaml_get_dom(uint32_t ID)
{
    struct yaml_doc_t* doc = yaml_get_doc(ID);
    if(!doc)
        return NULL;
    return doc->dom;
}

char*
yaml_get_value(const uint32_t ID, const char* key)
{
    struct yaml_doc_t* doc = yaml_get_doc(ID);
    if(!doc)
        return NULL;
    {
        const struct ptree_t* node = ptree_find_by_key(doc->dom, key);
        if(node)
            return (char*)node->value;
    }
    return NULL;
}

void
yaml_destroy(const uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t, doc)
    {
        if(doc->ID == ID)
        {
            ptree_destroy_free(doc->dom);
            unordered_vector_erase_element(&g_open_docs, doc);

            return;
        }
    }
}

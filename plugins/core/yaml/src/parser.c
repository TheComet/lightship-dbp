#include "plugin_yaml/config.h"
#include "plugin_yaml/parser.h"
#include "util/unordered_vector.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/ptree.h"
#include "util/string.h"
#include "util/hash.h"
#include <stdlib.h>

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
yaml_load_into_ptree(struct ptree_t* tree, struct ptree_t* root_tree, yaml_parser_t* parser, char is_sequence)
{
    yaml_event_t event;
    char* key;
    char finished = 0;
    char sequence_index = 0;
    const char FINISH_ERROR = 1;
    const char FINISH_SUCCESS = 2;

    if(!yaml_parser_parse(parser, &event))
    {
        char error[16];
        sprintf(error, "%d", parser->error);
        llog(LOG_ERROR, PLUGIN_NAME, 2, "Parser error ", error);
        return 0;
    }

    key = NULL;
    while(event.type != YAML_STREAM_END_EVENT)
    {
        char result;

        switch(event.type)
        {
            case YAML_NO_EVENT:
                llog(LOG_ERROR, PLUGIN_NAME, 1, "Syntax error in yaml document");
                finished = FINISH_ERROR;
                break;

            /* stream/document start/end */
            case YAML_STREAM_START_EVENT:
            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_START_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                break;

            /* begin of sequence (yaml list) */
            case YAML_SEQUENCE_START_EVENT:
                if(!key)
                {
                    llog(LOG_ERROR, PLUGIN_NAME, 1, "Received sequence start without a key");
                    finished = FINISH_ERROR;
                    break;
                }
                {
                    /* recurse, setting is_sequence to 1 */
                    struct ptree_t* child = ptree_add_node(tree, key, NULL);
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    result = yaml_load_into_ptree(child, root_tree, parser, 1);
                    free_string(key);
                    key = NULL;
                    if(!result)
                        finished = FINISH_ERROR;
                }
            case YAML_MAPPING_START_EVENT:
                
                /* 
                 * If this is a sequence, create index key as usual, but
                 * recurse with is_sequence set to 0
                 */
                if(is_sequence)
                {
                    struct ptree_t* child;
                    char index[sizeof(int)*8+1];
                    if(key)
                    {
                        llog(LOG_ERROR, PLUGIN_NAME, 1, "Received a key during a sequence");
                        finished = FINISH_ERROR;
                        break;
                    }
                    sprintf(index, "%d", sequence_index);
                    ++sequence_index;
                    
                    child = ptree_add_node(tree, index, NULL);
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    result = yaml_load_into_ptree(child, root_tree, parser, 0);
                    if(!result)
                        finished = FINISH_ERROR;
                }
                /*
                 * If this is not a sequence, then only recurse if a key
                 * exists.
                 */
                else if(key)
                {
                    struct ptree_t* child = ptree_add_node(tree, key, NULL);
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    result = yaml_load_into_ptree(child, root_tree, parser, 0);
                    free_string(key);
                    key = NULL;
                    if(!result)
                        finished = FINISH_ERROR;
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
                    else
                    {
                        llog(LOG_ERROR, PLUGIN_NAME, 1, "Failed to copy ptree \"", event.data.alias.anchor, "\" (yaml anchor failed)");
                        llog(LOG_ERROR, PLUGIN_NAME, 1, "Possible solution: References need to be defined before they are used.");
                        finished = FINISH_ERROR;
                        break;
                    }
                    free_string(key);
                    key = NULL;
                }
                break;

            /* scalar */
            case YAML_SCALAR_EVENT:
                
                /*
                 * If the scalar doesn't belong to a sequence, simply toggle
                 * back and forth between key and value, creating a new node 
                 * each time a value is received.
                 * If the scalar does belong to a sequence, use the current
                 * sequence index as the key instead.
                 */
                if(is_sequence)
                {
                    struct ptree_t* child;
                    char index[sizeof(int)*8+1];
                    if(key)
                    {
                        llog(LOG_ERROR, PLUGIN_NAME, 1, "Received a key during a sequence");
                        finished = FINISH_ERROR;
                        break;
                    }
                    sprintf(index, "%d", sequence_index);
                    child = ptree_add_node(tree, index, malloc_string((char*)event.data.scalar.value));
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    ++sequence_index;
                }
                else /* scalar doesn't belong to a sequence */
                {
                    if(key)
                    {
                        struct ptree_t* child = ptree_add_node(tree, key, malloc_string((char*)event.data.scalar.value));
                        ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                        free_string(key);
                        key = NULL;
                    }
                    else
                    {
                        key = malloc_string((char*)event.data.scalar.value);
                    }
                }
                break;
            
            default:
                llog(LOG_ERROR, PLUGIN_NAME, 1, "Unknown error");
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
        free_string(key);
    
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
        llog(LOG_ERROR, PLUGIN_NAME, 3, "Failed to open file \"", filename, "\"");
        return 0;
    }

    /* parse file and load into dom tree */
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, fp);
    tree = ptree_create("root", NULL);
    if(!yaml_load_into_ptree(tree, tree, &parser, 0))
    {
        yaml_parser_delete(&parser);
        fclose(fp);
        ptree_destroy(tree);
        llog(LOG_ERROR, PLUGIN_NAME, 3, "Syntax error: Failed to parse YAML file \"", filename, "\"");
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

const char*
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

SERVICE(yaml_load_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT_PTR(0, file_name, const char*);
    SERVICE_RETURN(yaml_load(file_name), uint32_t);
}

SERVICE(yaml_get_dom_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    SERVICE_RETURN(yaml_get_dom(id), struct ptree_t*);
}

SERVICE(yaml_get_value_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT_PTR(1, key, const char*);
    SERVICE_RETURN(yaml_get_value(id, key), const char*);
}

SERVICE(yaml_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    yaml_destroy(id);
}

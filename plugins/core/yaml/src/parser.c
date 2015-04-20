#include "plugin_yaml/config.h"
#include "plugin_yaml/parser.h"
#include "plugin_yaml/glob.h"
#include "framework/log.h"
#include "util/unordered_vector.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/hash.h"
#include <stdlib.h>

/* ------------------------------------------------------------------------- */
void
parser_init(struct game_t* game)
{
    unordered_vector_init_vector(&get_global(game)->parser.open_docs, sizeof(struct yaml_doc_t));
    get_global(game)->parser.doc_guid_counter = 1;
}

/* ------------------------------------------------------------------------- */
void
parser_deinit(struct game_t* game)
{
    unordered_vector_clear_free(&get_global(game)->parser.open_docs);
}

/* ------------------------------------------------------------------------- */
char
yaml_load_into_ptree(struct game_t* game,
                     struct ptree_t* tree,
                     struct ptree_t* root_node,
                     yaml_parser_t* parser,
                     char is_sequence)
{
    yaml_event_t event;
    char* key;
    char finished = 0;
    char sequence_index = 0; /* this is used to generate keys for when lists/
                              * sequences are read, as the ptree requires each
                              * node to have a key. */
    const char FINISH_ERROR = 1;
    const char FINISH_SUCCESS = 2;

    if(!yaml_parser_parse(parser, &event))
    {
        char error[16];
        sprintf(error, "%d", parser->error);
        llog(LOG_ERROR, game, PLUGIN_NAME, 2, "Parser error ", error);
        return 0;
    }

    key = NULL;
    while(event.type != YAML_STREAM_END_EVENT)
    {
        char result;

        switch(event.type)
        {
            case YAML_NO_EVENT:
                llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Syntax error in yaml document");
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
                    llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Received sequence start without a key");
                    finished = FINISH_ERROR;
                    break;
                }
                
                {
                    /* create child and recurse, setting is_sequence to 1 so
                     * the parser knows to generate sequence keys */
                    struct ptree_t* child = ptree_add_node(tree, key, NULL);
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    ptree_set_free_func(child, (ptree_free_func)free_string);
                    result = yaml_load_into_ptree(game, child, root_node, parser, 1);
                    free_string(key);
                    key = NULL;
                    if(!result)
                        finished = FINISH_ERROR;
                }
                
            case YAML_MAPPING_START_EVENT:
                
                /*
                 * If this is a sequence, create index key as usual, but
                 * recurse with is_sequence set to 0, since the child data
                 * isn't a sequence any more.
                 */
                if(is_sequence)
                {
                    struct ptree_t* child;
                    char index_str[sizeof(int)*8+1];
                    if(key)
                    {
                        llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Received a key during a sequence");
                        finished = FINISH_ERROR;
                        break;
                    }
                    sprintf(index_str, "%d", sequence_index);
                    ++sequence_index;
                    
                    child = ptree_add_node(tree, index_str, NULL);
                    ptree_set_dup_func(child, (ptree_dup_func)malloc_string);
                    result = yaml_load_into_ptree(game, child, root_node, parser, 0);
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
                    result = yaml_load_into_ptree(game, child, root_node, parser, 0);
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
                
            /* 
             * Aliases - Find the anchor name in the root node, and recursively
             * copy said node into the current tree.
             */
            case YAML_ALIAS_EVENT:
                if(key)
                {
                    const struct ptree_t* source = ptree_find_in_tree(root_node, (char*)event.data.alias.anchor);
                    if(source)
                    {
                        if(!ptree_duplicate_children_into_existing_node(tree, source))
                        {
                            fprintf(stderr, "[yaml] Failed to duplicate tree (anchor copy failed)\n");
                            finished = FINISH_ERROR;
                        }
                    }
                    else
                    {
                        llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Failed to copy ptree \"", event.data.alias.anchor, "\" (yaml anchor failed)");
                        llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Possible solution: References need to be defined before they are used.");
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
                        llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Received a key during a sequence");
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
                llog(LOG_ERROR, game, PLUGIN_NAME, 1, "Unknown error");
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

/* ------------------------------------------------------------------------- */
static struct yaml_doc_t*
yaml_get_doc(struct game_t* game, uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&get_global(game)->parser.open_docs, struct yaml_doc_t, doc)
    {
        if(doc->ID == ID)
            return doc;
    }
    return NULL;
}

/* ------------------------------------------------------------------------- */
uint32_t
yaml_load(struct game_t* game, const char* filename)
{
    FILE* fp;
    struct yaml_doc_t* doc;
    struct ptree_t* tree;
    yaml_parser_t parser;

    /* try to open the file */
    fp = fopen(filename, "rb");
    if(!fp)
    {
        llog(LOG_ERROR, game, PLUGIN_NAME, 3, "Failed to open file \"", filename, "\"");
        return 0;
    }

    /* parse file and load into dom tree */
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, fp);
    tree = ptree_create(NULL);
    if(!yaml_load_into_ptree(game, tree, tree, &parser, 0))
    {
        yaml_parser_delete(&parser);
        fclose(fp);
        ptree_destroy(tree, 0, 1);
        llog(LOG_ERROR, game, PLUGIN_NAME, 3, "Syntax error: Failed to parse YAML file \"", filename, "\"");
        return 0;
    }

    /* create doc object and initialise parser */
    doc = (struct yaml_doc_t*)unordered_vector_push_emplace(&get_global(game)->parser.open_docs);
    doc->ID = get_global(game)->parser.doc_guid_counter++;
    doc->dom = tree;
    
    /* clean up */
    yaml_parser_delete(&parser);
    fclose(fp);
    
    return doc->ID;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_get_dom(struct game_t* game, uint32_t ID)
{
    struct yaml_doc_t* doc = yaml_get_doc(game, ID);
    if(!doc)
        return NULL;
    return doc->dom;
}

/* ------------------------------------------------------------------------- */
const char*
yaml_get_value(struct game_t* game, const uint32_t ID, const char* key)
{
    struct ptree_t* node = yaml_get_node(game, ID, key);
    if(node)
        return (char*)node->value;
    return NULL;
}


/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_get_node(struct game_t* game, const uint32_t ID, const char* key)
{
    struct yaml_doc_t* doc = yaml_get_doc(game, ID);
    if(!doc)
        return NULL;
    return ptree_find_by_key(doc->dom, key);
}

/* ------------------------------------------------------------------------- */
void
yaml_destroy(struct game_t* game, const uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&get_global(game)->parser.open_docs, struct yaml_doc_t, doc)
    {
        if(doc->ID == ID)
        {
            ptree_destroy_free(doc->dom);
            unordered_vector_erase_element(&get_global(game)->parser.open_docs, doc);

            return;
        }
    }
}

/* ------------------------------------------------------------------------- */
/* WRAPPERS */
/* ------------------------------------------------------------------------- */
SERVICE(yaml_load_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT_PTR(0, file_name, const char*);
    SERVICE_RETURN(yaml_load(service->game, file_name), uint32_t);
}

SERVICE(yaml_get_dom_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    SERVICE_RETURN(yaml_get_dom(service->game, id), struct ptree_t*);
}

SERVICE(yaml_get_value_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT_PTR(1, key, const char*);
    SERVICE_RETURN(yaml_get_value(service->game, id, key), const char*);
}

SERVICE(yaml_get_node_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT_PTR(1, key, const char*);
    SERVICE_RETURN(yaml_get_node(service->game, id, key), struct ptree_t*);
}

SERVICE(yaml_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    yaml_destroy(service->game, id);
}

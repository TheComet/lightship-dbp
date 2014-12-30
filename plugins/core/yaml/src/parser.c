#include "util/unordered_vector.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/ptree.h"
#include "util/string.h"
#include "util/hash.h"
#include "plugin_yaml/parser.h"

struct unordered_vector_t g_open_docs;
static uint32_t GUID_counter = 1;

typedef enum yaml_parser_states_e
{
    YAML_PARSER_STATE_NONE,
    YAML_PARSER_STATE_GET_KEY,
    YAML_PARSER_STATE_GOT_KEY,
    YAML_PARSER_STATE_GET_VALUE,
    YAML_PARSER_STATE_PREPARE_NEW_DEPTH,
    YAML_PARSER_STATE_GET_KEY_FOR_NEW_DEPTH,
    YAML_PARSER_STATE_GOT_KEY_FOR_NEW_DEPTH,
    YAML_PARSER_STATE_ENTER_NEW_DEPTH,
    YAML_PARSER_STATE_RETURN,
    YAML_PARSER_STATE_ERROR
} yaml_parser_states_e;

void
parser_init(void)
{
    unordered_vector_init_vector(&g_open_docs, sizeof(struct yaml_doc_t*));
}

void
parser_deinit(void)
{
    unordered_vector_clear_free(&g_open_docs);
}

char
yaml_load_into_ptree(struct ptree_t* tree, yaml_parser_t* parser)
{
    yaml_token_t token;
    yaml_parser_states_e state = YAML_PARSER_STATE_NONE;
    char* key = NULL;

    yaml_parser_scan(parser, &token);
    while(token.type != YAML_STREAM_END_TOKEN)
    {
        switch(token.type)
        {
            /* Stream start/end */
            case YAML_STREAM_START_TOKEN:
                break;
            case YAML_STREAM_END_TOKEN:
                break;

            /* Token types (read before actual token) */
            case YAML_KEY_TOKEN:

                /* make sure last state slots in */
                if(state != YAML_PARSER_STATE_NONE &&
                   state != YAML_PARSER_STATE_PREPARE_NEW_DEPTH)
                {
                    llog(LOG_ERROR, 1, "got a new key token event before being able to match the last one with a value");
                    state = YAML_PARSER_STATE_ERROR;
                    break;
                }
                /* enter next state */
                if(state == YAML_PARSER_STATE_NONE)
                    state = YAML_PARSER_STATE_GET_KEY;
                if(state == YAML_PARSER_STATE_PREPARE_NEW_DEPTH)
                    state = YAML_PARSER_STATE_GET_KEY_FOR_NEW_DEPTH;
                break;

            case YAML_VALUE_TOKEN:

                /* make sure last state slots in */
                if(state != YAML_PARSER_STATE_GOT_KEY &&
                   state != YAML_PARSER_STATE_GOT_KEY_FOR_NEW_DEPTH)
                {
                    llog(LOG_ERROR, 1, "got a new value token event before being able to match the last one with a key");
                    state = YAML_PARSER_STATE_ERROR;
                    break;
                }
                /* enter next state */
                if(state == YAML_PARSER_STATE_GOT_KEY)
                    state = YAML_PARSER_STATE_GET_VALUE;
                if(state == YAML_PARSER_STATE_GOT_KEY_FOR_NEW_DEPTH)
                    state = YAML_PARSER_STATE_ENTER_NEW_DEPTH;
                break;

            /* Block delimeters */
            case YAML_BLOCK_SEQUENCE_START_TOKEN:
                break;
            case YAML_BLOCK_ENTRY_TOKEN:
                break;
            case YAML_BLOCK_END_TOKEN:
                /* make sure state slots in */
                if(state != YAML_PARSER_STATE_NONE)
                {
                    llog(LOG_ERROR, 1, "exiting block without getting a value");
                    state = YAML_PARSER_STATE_ERROR;
                    break;
                }
                /* new state */
                state = YAML_PARSER_STATE_RETURN;
                break;

            /* Data */
            case YAML_BLOCK_MAPPING_START_TOKEN:
                /* make sure last state slots in */
                if(state != YAML_PARSER_STATE_NONE &&
                   state != YAML_PARSER_STATE_GET_VALUE &&
                   state != YAML_PARSER_STATE_ENTER_NEW_DEPTH
                )
                {
                    llog(LOG_ERROR, 1, "entering a new block without a key");
                    state = YAML_PARSER_STATE_ERROR;
                    break;
                }
                
                /* check if time to enter new depth */
                if(state == YAML_PARSER_STATE_ENTER_NEW_DEPTH ||
                   state == YAML_PARSER_STATE_GET_VALUE)
                {
                    int result = yaml_load_into_ptree(ptree_add_node(tree, key, NULL), parser);
                    FREE(key); key = NULL;
                    if(result == 0)
                    {
                        state = YAML_PARSER_STATE_ERROR;
                        break;
                    }
                    state = YAML_PARSER_STATE_NONE;
                    break;
                }
                /* new state */
                state = YAML_PARSER_STATE_PREPARE_NEW_DEPTH;

                break;

            case YAML_SCALAR_TOKEN:

                /* make sure last state slots in */
                if(state != YAML_PARSER_STATE_GET_KEY &&
                   state != YAML_PARSER_STATE_GET_VALUE &&
                   state != YAML_PARSER_STATE_GET_KEY_FOR_NEW_DEPTH &&
                   state != YAML_PARSER_STATE_ENTER_NEW_DEPTH)
                {
                    llog(LOG_ERROR, 1, "got a scalar token event without any key or value");
                    state = YAML_PARSER_STATE_ERROR;
                    break;
                }
                
                if(state == YAML_PARSER_STATE_ENTER_NEW_DEPTH)
                {
                    int result = yaml_load_into_ptree(ptree_add_node(tree, key, malloc_string((char*)token.data.scalar.value)), parser);
                    FREE(key); key = NULL;
                    if(result == 0)
                    {
                        state = YAML_PARSER_STATE_ERROR;
                        break;
                    }
                    state = YAML_PARSER_STATE_NONE;
                    break;
                }

                /*
                 * The last event was a key token event...
                 */
                if(state == YAML_PARSER_STATE_GET_KEY ||
                   state == YAML_PARSER_STATE_GET_KEY_FOR_NEW_DEPTH)
                {
                    /* 
                     * If the tree exists, search at the current depth for the
                     * key. If the key exists, throw an error since duplicate
                     * keys on the same depth are illegal. If the key doesn't
                     * exist, create it.
                     */
                    if(tree)
                    {
                        if(ptree_find_local_by_key(tree, (char*)token.data.scalar.value))
                        {
                            llog(LOG_ERROR, 3, "Duplicate key found in same level of indentation: \"", token.data.scalar.value, "\"");
                            state = YAML_PARSER_STATE_ERROR;
                            break;
                        }
                    }
                    
                    /*
                     * Safe to create key. Cannot create tree (or tree node) yet
                     * because we haven't received the value token event from
                     * libyaml yet, so just malloc the string and save it
                     * for later.
                     */
                    key = malloc_string((char*)token.data.scalar.value);
                    
                    /* enter next state */
                    if(state == YAML_PARSER_STATE_GET_KEY)
                        state = YAML_PARSER_STATE_GOT_KEY;
                    if(state == YAML_PARSER_STATE_GET_KEY_FOR_NEW_DEPTH)
                        state = YAML_PARSER_STATE_GOT_KEY_FOR_NEW_DEPTH;
                }
                
                /*
                 * The last event was a key token event...
                 */
                if(state == YAML_PARSER_STATE_GET_VALUE)
                {
                    /*
                     * It is now safe to either create the tree if it doesn't
                     * exist yet, or add a new node to the tree.
                     */
                    ptree_add_node(tree, key, malloc_string((char*)token.data.scalar.value));

                    /*
                     * key is now no longer needed, free it for re-use.
                     */
                    FREE(key);
                    key = NULL;
                    
                    /* enter next state */
                    if(state == YAML_PARSER_STATE_GET_VALUE)
                        state = YAML_PARSER_STATE_NONE;
                }

                break;
            
            /* unexpected token */
            default:
                {
                    char type_str[sizeof(yaml_token_type_t)+1];
                    sprintf(type_str, "%d", token.type);
                    llog(LOG_ERROR, 2, "Got token of type ", type_str);
                }
                state = YAML_PARSER_STATE_ERROR;
                break;
        }
        
        /* error occurred, abort */
        if(state == YAML_PARSER_STATE_ERROR)
            break;
        
        /* return, break */
        if(state == YAML_PARSER_STATE_RETURN)
            break;

        yaml_token_delete(&token);
        yaml_parser_scan(parser, &token);
    }
    
    /* clean up */
    yaml_token_delete(&token);
    if(key)
        FREE(key);
    
    /* 
     * Make sure state machine has returned to initial state, otherwise there
     * has been an error
     */
    if((state != YAML_PARSER_STATE_NONE &&
        state != YAML_PARSER_STATE_RETURN) ||
        state == YAML_PARSER_STATE_ERROR)
        return 0;
    
    return 1;
}

static struct yaml_doc_t*
yaml_get_doc(uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t*, docp)
    {
        if((*docp)->ID == ID)
            return *docp;
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
    if(!yaml_load_into_ptree(tree, &parser))
    {
        yaml_parser_delete(&parser);
        fclose(fp);
        ptree_destroy(tree);
        llog(LOG_ERROR, 3, "Syntax error: Failed to parse YAML file \"", filename, "\"");
        return 0;
    }
    
    /* create doc object and initialise parser */
    doc = (struct yaml_doc_t*)MALLOC(sizeof(struct yaml_doc_t));
    doc->ID = GUID_counter++;
    doc->dom = tree;
    unordered_vector_push(&g_open_docs, &doc);
    
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
    return (char*)ptree_find_by_key(doc->dom, key);
}

void
yaml_destroy(const uint32_t ID)
{
    struct yaml_doc_t* doc;
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t*, docp)
    {
        doc = *docp;
        if(doc->ID == ID)
        {
            ptree_destroy(doc->dom);
            FREE(doc);
            unordered_vector_erase_index(&g_open_docs, (intptr_t)docp);

            return;
        }
    }
}

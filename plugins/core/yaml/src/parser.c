#include "util/unordered_vector.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/ptree.h"
#include "util/string.h"
#include "util/hash.h"
#include "plugin_yaml/parser.h"

typedef enum yaml_parser_states_e
{
    YAML_PARSER_STATE_NONE,
    YAML_PARSER_STATE_KEY,
    YAML_PARSER_STATE_VALUE,
    YAML_PARSER_STATE_ERROR
} yaml_parser_states_e;

struct unordered_vector_t g_open_docs;
static uint32_t GUID_counter = 1;

void
parser_init(void)
{
    unordered_vector_init_vector(&g_open_docs, sizeof(struct yaml_doc_t*));
}

struct ptree_t*
yaml_load_into_ptree(struct ptree_t* tree, yaml_parser_t* parser)
{
    yaml_token_t token;
    yaml_parser_states_e state;

    yaml_parser_scan(parser, &token);
    while(token.type != YAML_STREAM_END_TOKEN)
    {
        char* key = NULL;
        char state = YAML_PARSER_STATE_NONE;
        switch(token.type)
        {
            /* Stream start/end */
            case YAML_STREAM_START_TOKEN: /*puts("STREAM START");*/ break;
            case YAML_STREAM_END_TOKEN:   /*puts("STREAM END");  */ break;
            /* Token types (read before actual token) */
            case YAML_KEY_TOKEN:   state = YAML_PARSER_STATE_KEY; puts("(key token)"); break;
            case YAML_VALUE_TOKEN: state = YAML_PARSER_STATE_VALUE; puts("(value token)"); break;
            /* Block delimeters */
            case YAML_BLOCK_SEQUENCE_START_TOKEN: puts("<b>Start Block (Sequence)</b>"); break;
            case YAML_BLOCK_ENTRY_TOKEN:          puts("<b>Start Block (Entry)</b>");    break;
            case YAML_BLOCK_END_TOKEN:            puts("<b>End block</b>");              break;
            /* Data */
            case YAML_BLOCK_MAPPING_START_TOKEN:  /*puts("[Block mapping]");         */   break;
            case YAML_SCALAR_TOKEN: 
                printf("scalar %s \n", token.data.scalar.value);

                if(state == YAML_PARSER_STATE_KEY && key == NULL)
                {
                    /* 
                     * If the tree has not yet been created, create it. If the
                     * tree exists, search at the current depth for the key.
                     * If the key exists, throw an error since duplicate keys
                     * on the same depth are illegal. If the key doesn't exist,
                     * create it.
                     *
                    char key_already_exists = 0;
                    uint32_t hash = hash_jenkins_oaat(token.data.scalar.value, strlen(token.data.scalar.value));
                    if(tree)
                    {
                        UNORDERED_VECTOR_FOR_EACH(&tree->children, struct ptree_t*, child)
                        {
                            if(hash == child->hash)
                            {
                                key_already_exists = 1;
                                break;
                            }
                        }
                    }
                    else
                    {
                    }
                    key = malloc_string(token.data.scalar.value);*/
                }
                if(state == YAML_PARSER_STATE_VALUE && key != NULL)
                    ;
                break;
            /* Others */
            default:
                printf("Got token of type %d\n", token.type);
                state = YAML_PARSER_STATE_ERROR;
                break;
        }
        
        /* error occurred, abort */
        if(state == YAML_PARSER_STATE_ERROR)
        {
            if(tree)
                ptree_destroy(tree);
            yaml_token_delete(&token);
            return NULL;
        }

        yaml_token_delete(&token);
        yaml_parser_scan(parser, &token);
    }
    yaml_token_delete(&token);
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
    tree = yaml_load_into_ptree(NULL, &parser);
    if(!tree)
    {
        yaml_parser_delete(&parser);
        fclose(fp);
        return 0;
    }

    /* create doc object and initialise parser */
    doc = (struct yaml_doc_t*)MALLOC(sizeof(struct yaml_doc_t));
    doc->ID = GUID_counter++;
    unordered_vector_push(&g_open_docs, &doc);
    
    /* clean up */
    yaml_parser_delete(&parser);
    fclose(fp);
    
    return doc->ID;
}

char*
yaml_get(const uint32_t ID, const char* key)
{
    /* get document object */
    struct yaml_doc_t* doc;
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t*, docp)
    {
        if((*docp)->ID == ID)
        {
            doc = *docp;
            break;
        }
    }

    
}

void
yaml_destroy(const uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t*, docp)
    {
        struct yaml_doc_t* doc = *docp; /* remember, what is in the vector is a pointer to the pointer of the yaml doc object */
        if(doc->ID == ID)
        {
            FREE(doc);
            unordered_vector_erase_element(&g_open_docs, doc);
            break;
        }
    }

    /* 
     * If the vector is now empty, also free the data it is holding so we don't
     * have to explicitely clean up when the plugin unloads.
     */
    if(g_open_docs.count == 0)
        unordered_vector_clear_free(&g_open_docs);
}

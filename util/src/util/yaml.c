#include "yaml/yaml.h"
#include "util/yaml.h"
#include "util/linked_list.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/ptree.h"
#include "util/unordered_vector.h"
#include <assert.h>

static struct list_t g_open_docs;

static char
yaml_load_into_ptree(struct ptree_t* tree,
                     struct ptree_t* root_node,
                     yaml_parser_t* parser,
                     char is_sequence);

static struct unordered_vector_t*
yaml_dup_node_value_func(struct unordered_vector_t* vec);

static void
yaml_free_node_value_func(struct unordered_vector_t* vec);

static struct ptree_t*
yaml_init_node(struct ptree_t* node);

/* ------------------------------------------------------------------------- */
void
yaml_init(void)
{
    list_init_list(&g_open_docs);
}

/* ------------------------------------------------------------------------- */
void
yaml_deinit(void)
{
    list_clear(&g_open_docs);
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_load(const char* filename)
{
    FILE* fp;
    struct ptree_t* doc;

    assert(filename);

    /* try to open the file */
    fp = fopen(filename, "rb");
    if(!fp)
    {
        fprintf(stderr, "Failed to open file \"%s\"\n", filename);
        return NULL;
    }

    doc = yaml_load_from_stream(fp);
    fclose(fp);

    return doc;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_load_from_memory(const char* buffer)
{
    FILE* stream;
    struct ptree_t* doc;

    assert(buffer);

    stream = fmemopen((char*)buffer, strlen(buffer), "rb");
    if(!stream)
    {
        fprintf(stderr, "Failed to open buffer as stream");
        return NULL;
    }

    doc = yaml_load_from_stream(stream);
    fclose(stream);

    return doc;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_load_from_stream(FILE* stream)
{
    yaml_parser_t parser;
    struct ptree_t* doc;

    /* parse file and load into tree */
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, stream);
    doc = ptree_create(NULL);
    if(!yaml_init_node(doc) || !yaml_load_into_ptree(doc, doc, &parser, 0))
    {
        yaml_parser_delete(&parser);
        ptree_destroy(doc);
        fprintf(stderr, "Syntax error: Failed to parse YAML.");
        return 0;
    }

    /* add to open documents */
    list_push(&g_open_docs, doc);

    /* clean up */
    yaml_parser_delete(&parser);

    return doc;
}

/* ------------------------------------------------------------------------- */
void
yaml_destroy(struct ptree_t* doc)
{
    ptree_destroy(doc);
    list_erase_element(&g_open_docs, doc);
}

/* ------------------------------------------------------------------------- */
const char*
yaml_get_value(const struct ptree_t* doc, const char* key)
{
    struct ptree_t* node;
    if(!(node = yaml_get_node(doc, key)))
        return NULL;

    /* return the last item in the vector */
    return *(const char**)unordered_vector_back((struct unordered_vector_t*)node->value);
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_get_node(const struct ptree_t* node, const char* key)
{
    return ptree_get_node(node, key);
}

/* ------------------------------------------------------------------------- */
uint32_t
yaml_get_hash(const struct ptree_t* node)
{
    /*
     * The hash isn't stored in the node itself, but in the map of the parent
     * node.
     */
    if(!node->parent)
        return 0;

    return map_find_element(&node->parent->children, node);
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_set_value(struct ptree_t* doc, const char* key, const char* value)
{
    struct ptree_t* node;
    struct unordered_vector_t* vec;

    /* handle duplicate keys by storing a vector as the value */
    if((node = ptree_add_node(doc, key, NULL)))
    {
        if(!yaml_init_node(node))
            return NULL;
    }
    else /* duplicate key, get existing node */
    {
        if(!(node = ptree_get_node(doc, key)))
            return NULL;
    }
    vec = (struct unordered_vector_t*)node->value;

    /* add yaml value */
    unordered_vector_push(vec, &value);

    return node;
}

/* ------------------------------------------------------------------------- */
static struct ptree_t*
yaml_init_node(struct ptree_t* node)
{
    if(!(node->value = unordered_vector_create(sizeof(char*))))
        return NULL;

    /* init duplication and free functions */
    ptree_set_dup_func(node, (ptree_dup_func)yaml_dup_node_value_func);
    ptree_set_free_func(node, (ptree_free_func)yaml_free_node_value_func);

    return node;
}

/* ------------------------------------------------------------------------- */
/*
 * Takes the value of a yaml node (node->value) and duplicates it.
 * Each yaml node points to a vector container, which in turn contains all
 * strings of the yaml node.
 */
static struct unordered_vector_t*
yaml_dup_node_value_func(struct unordered_vector_t* vec)
{
    struct unordered_vector_t* new_vec;

    for(;;)
    {
        /* create new vector and copy all strings into it */
        if(!(new_vec = unordered_vector_create(sizeof(char*))))
            break;
        { UNORDERED_VECTOR_FOR_EACH(vec, char*, pstr)
        {
            char* new_str;
            if(!(new_str = malloc_string(*pstr)))
                break;
            unordered_vector_push(new_vec, &new_str);
        }}

        /* return the new vector */
        return new_vec;
    }

    if(new_vec)
        unordered_vector_destroy(new_vec);

    return NULL;
}

/* ------------------------------------------------------------------------- */
/*
 * Takes the value of a yaml node (node->value) and frees it.
 * Each yaml node points to a vector container, which in turn contains all
 * strings of the yaml node.
 */
static void
yaml_free_node_value_func(struct unordered_vector_t* vec)
{
    /* iterate all strings and free them */
    UNORDERED_VECTOR_FOR_EACH(vec, char*, pstr)
    {
        if(*pstr) /* strings can be NULL */
            free_string(*pstr);
    }
    unordered_vector_destroy(vec);
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
yaml_destroy_value(struct ptree_t* doc, const char* key)
{
}

/* ------------------------------------------------------------------------- */
static char
yaml_load_into_ptree(struct ptree_t* tree,
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
        fprintf(stderr, "[yaml] Parser error: %d\n", parser->error);
        return 0;
    }

    key = NULL;
    while(event.type != YAML_STREAM_END_EVENT)
    {
        char result;

        switch(event.type)
        {
            case YAML_NO_EVENT:
                fprintf(stderr, "[yaml] Syntax error in yaml document\n");
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
                    fprintf(stderr, "[yaml] Received sequence start without a key\n");
                    finished = FINISH_ERROR;
                    break;
                }

                {
                    /* create child and recurse, setting is_sequence to 1 so
                     * the parser knows to generate sequence keys */
                    struct ptree_t* child = yaml_set_value(tree, key, NULL);
                    result = yaml_load_into_ptree(child, root_node, parser, 1);
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
                        fprintf(stderr, "Received a key during a sequence\n");
                        finished = FINISH_ERROR;
                        break;
                    }
                    sprintf(index_str, "%d", sequence_index);
                    ++sequence_index;

                    child = yaml_set_value(tree, index_str, NULL);
                    result = yaml_load_into_ptree(child, root_node, parser, 0);
                    if(!result)
                        finished = FINISH_ERROR;
                }

                /*
                 * If this is not a sequence, then only recurse if a key
                 * exists.
                 */
                else if(key)
                {
                    struct ptree_t* child = yaml_set_value(tree, key, NULL);
                    result = yaml_load_into_ptree(child, root_node, parser, 0);
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
                    const struct ptree_t* source = yaml_get_node(root_node, (char*)event.data.alias.anchor);
                    if(source)
                    {
                        struct ptree_t* child = yaml_set_value(tree, key, NULL);
                        if(!ptree_duplicate_children_into_existing_node(child, source))
                        {
                            fprintf(stderr, "[yaml] Failed to duplicate tree (anchor copy failed)\n");
                            finished = FINISH_ERROR;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "[yaml] Failed to copy ptree \"%s\" (yaml anchor failed)\n", event.data.alias.anchor);
                        fprintf(stderr, "[yaml] Possible solution: References need to be defined before they are used.\n");
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
                    char index[sizeof(int)*8+1];
                    if(key)
                    {
                        fprintf(stderr, "[yaml] Received a key during a sequence\n");
                        finished = FINISH_ERROR;
                        break;
                    }
                    sprintf(index, "%d", sequence_index);
                    yaml_set_value(tree, index, malloc_string((char*)event.data.scalar.value));
                    ++sequence_index;
                }
                else /* scalar doesn't belong to a sequence */
                {
                    if(key)
                    {
                        yaml_set_value(tree, key, malloc_string((char*)event.data.scalar.value));
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
                fprintf(stderr, "[yaml] Unknown error\n");
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

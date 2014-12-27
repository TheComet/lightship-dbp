#include "util/unordered_vector.h"
#include "util/log.h"
#include "util/memory.h"
#include "plugin_yaml/parser.h"

struct unordered_vector_t g_open_docs;
static uint32_t GUID_counter = 1;

void parser_init(void)
{
    unordered_vector_init_vector(&g_open_docs, sizeof(struct yaml_doc_t*));
}

uint32_t yaml_open(const char* filename)
{
    FILE* fp;
    unsigned char* text;
    unsigned int file_size;
    struct yaml_doc_t* doc;
    
    /* try to open the file */
    fp = fopen(filename, "rb");
    if(!fp)
    {
        llog(LOG_ERROR, 3, "Failed to open file \"", filename, "\"");
        return 0;
    }
    
    /* get file size */
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    
    /* copy entire file into memory */
    text = (unsigned char*)MALLOC(file_size+1);
    fread(text, file_size, file_size, fp);
    text[file_size] = '\0';
    printf("yaml text:\n%s\n", text);
    
    /* create doc object and initialise parser */
    doc = (struct yaml_doc_t*)MALLOC(sizeof(struct yaml_doc_t));
    doc->file_size = file_size;
    doc->text = text;
    doc->ID = GUID_counter++;
    yaml_parser_initialize(&doc->parser);
    yaml_parser_set_input_string(&doc->parser, doc->text, doc->file_size);
    unordered_vector_push(&g_open_docs, &doc);
    
    /* clean up */
    fclose(fp);
    
    return doc->ID;
}

void yaml_close(const uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_open_docs, struct yaml_doc_t*, docp)
    {
        struct yaml_doc_t* doc = *docp; /* remember, what is in the vector is a pointer to the pointer of the yaml doc object */
        if(doc->ID == ID)
        {
            FREE(doc->text);
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

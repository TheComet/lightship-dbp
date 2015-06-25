#include "util/dynamic_call.h"
#include "util/memory.h"
#include "util/ordered_vector.h"
#include "util/string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * If the type being inserted into the argument vector is equal or smaller
 * than sizeof(void*), we can save some time by copying it directly into
 * the extra space at the end of the vector instead of allocating space on
 * the heap. The pointer to the argument's memory location is inserted
 * into the beginning of the vector as normal. This way, when extracting
 * the arguments, whether the argument is on the heap or on embedded into
 * the argument vector itself will be transparent.
 *
 * The argument vector is guaranteed to have double the amount of slots
 * than argc. The following example shows the memory layout of an
 * argument vector with argc = 3:
 *
 *        pointers go here
 *                 v-----v
 *                |0|1|2|3|4|5|6|7|
 *                         ^-----^
 *                     Extra space at the end.
 *                     args smaller than sizeof(void*) go here
 *
 * In the case of arguments being copied into the end of the vector, the
 * slots in the beginning of the vector are set to point to the end of the
 * vector, as illustrated here:
 *                  _______
 *                 |       v
 *                |0|1|2|3|4|5|6|7|
 *                   |_______^
 *
 * If the type being inserted into the argument vector is greater than
 * sizeof(void*) then it is malloc'd and the slot at the end of the
 * argument vector is unused.
 */
#define COPY_ARGUMENT_INTO_ARGV(value_t, extract_func)                      \
        /* extract type can/will be different than the actual type */       \
        value_t value = (value_t)extract_func                               \
                                                                            \
        /* point the beginning of the vector to the slot at the end */      \
        argv[i] = argv + type_info->argc + i;                               \
                                                                            \
        /* now copy the argument into the end of the vector (argv[i] points \
         * to the correct slot at the end) */                               \
        memcpy(argv[i], &value, sizeof value);

/*
 * This macro is used in the case when the type of the argument is larger
 * than sizeof(void*).
 */
#define MALLOC_ARGUMENT_INTO_ARGV(value_t, extract_t)                       \
        argv[i] = MALLOC(sizeof(value_t));                                  \
        *(value_t*)argv[i] = (value_t)va_arg(ap, extract_t);

/* ------------------------------------------------------------------------- */
struct type_info_t*
dynamic_call_create_type_info(const char* ret_type, int argc, const char** argv)
{
    struct type_info_t* type_info;

    assert(ret_type);
    if(argc)
        assert(argv);

    /* allocate and initialise type info object */
    if(!(type_info = (struct type_info_t*)MALLOC(sizeof(struct type_info_t))))
    {
        fprintf(stderr, "malloc() failed in dynamic_call_create_type_info() -- not enough memory\n");
        return NULL;
    }
    memset(type_info, 0, sizeof(struct type_info_t));

    for(;;) /* break from here if something goes wrong */
    {
        type_info->has_unknown_types = 0; /* will be set to 1 during parsing
                                           * if an unknown type is found */

        /*
         * The return type string is parsed to determine if the type is known
         * or not and the result is stored.
         */
        type_info->ret_type = dynamic_call_get_type_from_string(ret_type);
        if(type_info->ret_type == TYPE_UNKNOWN)
            type_info->has_unknown_types = 1;

        /*
         * Each argument type string is parsed and it is determined if the type
         * is known or not. The results are stored in an array.
         */
        if(!(type_info->argv_type = (type_e*)MALLOC(argc * sizeof(type_e))))
            break;
        memset(type_info->argv_type, 0, argc * sizeof(type_e));
        {   int i;
            for(i = 0; i != argc; ++i)
            {
                type_info->argv_type[i] = dynamic_call_get_type_from_string(argv[i]);
                if(type_info->argv_type[i] == TYPE_UNKNOWN)
                    type_info->has_unknown_types = 1;

                ++type_info->argc;
            }
        }

        /* success! */
        return type_info;
    }

    /* something went wrong, clean up everything */
    dynamic_call_destroy_type_info(type_info);

    return NULL;
}

/* ------------------------------------------------------------------------- */
void
dynamic_call_destroy_type_info(struct type_info_t* type_info)
{
    /* free argument type info array */
    if(type_info->argv_type)
        FREE(type_info->argv_type);

    FREE(type_info);
}

/* ------------------------------------------------------------------------- */
void**
dynamic_call_create_argument_vector_from_strings(
        const struct type_info_t* type_info,
        const struct ordered_vector_t* strings)
{
    void** ret;

    assert(type_info);
    assert(strings);

    /* check argument count */
    if(type_info->argc != strings->count)
    {
        fprintf(stderr, "Cannot create argument list: Wrong number of arguments\n");
        fprintf(stderr, "    Required: %d\n", type_info->argc);
        fprintf(stderr, "    Provided: %d\n", strings->count);
        return NULL;
    }

    /* create void** argument vector */
    /* reserve double the space for when values smaller than the size of void*
     * are copied into the argument vector directly */
    ret = (void**)MALLOC(type_info->argc * sizeof(void*) * 2);
    if(!ret)
    {
        fprintf(stderr, "malloc() failed in dynamic_call_create_argument_vector_from_strings() -- not enough memory\n");
        return NULL;
    }

    /* fill out argument vector with the arguments in the string vector */
    if(!dynamic_call_set_argument_vector_from_strings(type_info, ret, strings))
    {
        dynamic_call_destroy_argument_vector(type_info, ret);
        return NULL;
    }

    return ret;
}

/* ------------------------------------------------------------------------- */
char
dynamic_call_set_argument_vector_from_strings(
    const struct type_info_t* type_info,
    void** argv,
    const struct ordered_vector_t* strings)
{
    int i;

    assert(type_info);
    assert(strings);

    /* clear, so cleanup can detect which elements to free if something goes
     * wrong */
    memset(argv, 0, type_info->argc * sizeof(void*) * 2);

    /* parse argument vector */
    i = 0;
    { ORDERED_VECTOR_FOR_EACH(strings, const char*, str_p)
    {
        const char* str = *str_p;

        switch(type_info->argv_type[i])
        {
/* ------------------------------------------------------------------------- */
            /* string types */
            case TYPE_STRING:
                argv[i] = malloc_string(str);
                break;
            case TYPE_WSTRING:
                argv[i] = strtowcs(str);
                break;
/* ------------------------------------------------------------------------- */
            /* integer types */
            case TYPE_INT8:
            {   int8_t value = (int8_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT8:
            {   uint8_t value = (uint8_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#if SIZEOF_VOID_PTR >= 2
            case TYPE_INT16:
            {   int16_t value = (int16_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT16:
            {   uint16_t value = (uint16_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_INT16:
            {   argv[i] = MALLOC(sizeof(int16_t));
                *(int16_t*)argv[i] = (int16_t)atoi(str);
                break; }
            case TYPE_UINT16:
            {   argv[i] = MALLOC(sizeof(uint16_t));
                *(uint16_t*)argv[i] = (uint16_t)atoi(str);
                break; }
#endif
#if SIZEOF_VOID_PTR >= 4
            case TYPE_INT32:
            {   int32_t value = (int32_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT32:
            {   uint32_t value = (uint32_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_INT32:
            {   argv[i] = MALLOC(sizeof(int32_t));
                *(int32_t*)argv[i] = (int32_t)atoi(str);
                break; }
            case TYPE_UINT32:
            {   argv[i] = MALLOC(sizeof(uint32_t));
                *(uint32_t*)argv[i] = (uint32_t)atoi(str);
                break; }
#endif
#if SIZEOF_VOID_PTR >= 8
            case TYPE_INT64:
            {   int64_t value = (int64_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT64:
            {   uint64_t value = (uint64_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_INT64:
            {   argv[i] = MALLOC(sizeof(int64_t));
                *(int64_t*)argv[i] = (int64_t)atoi(str);
                break; }
            case TYPE_UINT64:
            {   argv[i] = MALLOC(sizeof(uint64_t));
                *(uint64_t*)argv[i] = (uint64_t)atoi(str);
                break; }
#endif
/* ------------------------------------------------------------------------- */
            /* intptr is by definition the same size as void* */
            case TYPE_INTPTR:
            {   intptr_t value = (intptr_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINTPTR:
            {   uintptr_t value = (uintptr_t)atoi(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
/* ------------------------------------------------------------------------- */
                /* floating point types */
#if SIZEOF_VOID_PTR >= SIZEOF_FLOAT
            case TYPE_FLOAT:
            {   float value = (float)atof(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_FLOAT:
            {   argv[i] = MALLOC(sizeof(float));
                *(float*)argv[i] = (float)atoi(str);
                break; }
#endif
#if SIZEOF_VOID_PTR >= SIZEOF_DOUBLE
            case TYPE_DOUBLE:
            {   double value = (double)atof(str);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_DOUBLE:
            {   argv[i] = MALLOC(sizeof(double));
                *(double*)argv[i] = (double)atoi(str);
                break; }
#endif
/* ------------------------------------------------------------------------- */
            /* none types can't be passed as an argument */
            case TYPE_NONE:
                fprintf(stderr, "Cannot create argument in vector: Invalid "
                        "type \"%s\"\n", str);
                return 0;
/* ------------------------------------------------------------------------- */
            default:
                fprintf(stderr, "Cannot create argument in vector: Unknown "
                        "type \"%s\"\n", str);
                return 0;
        }

        ++i;
    }}

    return 1;
}

/* ------------------------------------------------------------------------- */
void**
dynamic_call_create_argument_vector_from_varargs(
        const struct type_info_t* type_info, ...)
{
    va_list ap;
    void** ret;
    va_start(ap, type_info);
    ret = vdynamic_call_create_argument_vector_from_varargs(type_info, ap);
    va_end(ap);
    return ret;
}

void**
vdynamic_call_create_argument_vector_from_varargs(
        const struct type_info_t* type_info, va_list ap)
{
    void** ret;

    assert(type_info);

    /* create void** argument vector */
    /* reserve double the space for when values smaller than the size of void*
     * are copied into the argument vector directly */
    ret = (void**)MALLOC(type_info->argc * sizeof(void*) * 2);
    if(!ret)
    {
        fprintf(stderr, "malloc() failed in dynamic_call_create_argument_vector_from_strings() -- not enough memory\n");
        return NULL;
    }

    /* fill out argument vector with the arguments in the va_list */
    if(!vdynamic_call_set_argument_vector_from_varargs(type_info, ret, ap))
    {
        dynamic_call_destroy_argument_vector(type_info, ret);
        return NULL;
    }

    return ret;
}

/* ------------------------------------------------------------------------- */
char
dynamic_call_set_argument_vector_from_varargs(
        const struct type_info_t* type_info, void** argv,  ...)
{
    va_list ap;
    char ret;
    va_start(ap, argv);
    ret = vdynamic_call_set_argument_vector_from_varargs(type_info, argv, ap);
    va_end(ap);
    return ret;
}

char
vdynamic_call_set_argument_vector_from_varargs(
        const struct type_info_t* type_info, void** argv, va_list ap)
{
    uint32_t i;

    /* clear, so cleanup can detect which elements to free if something goes
     * wrong */
    memset(argv, 0, type_info->argc * sizeof(void*) * 2);

    /* parse argument vector */
    for(i = 0; i != type_info->argc; ++i)
    {
        switch(type_info->argv_type[i])
        {
/* ------------------------------------------------------------------------- */
            /* string types */
            case TYPE_STRING:
                argv[i] = malloc_string(va_arg(ap, char*));
                break;
            case TYPE_WSTRING:
                argv[i] = malloc_wstring(va_arg(ap, wchar_t*));
                break;
/* ------------------------------------------------------------------------- */
            /* integer types */
            case TYPE_INT8:
            {   int8_t value = (int8_t)va_arg(ap, int);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT8:
            {   uint8_t value = (uint8_t)va_arg(ap, int);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#if SIZEOF_VOID_PTR >= 2
            case TYPE_INT16:
            {   int16_t value = (int16_t)va_arg(ap, int);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT16:
            {   uint16_t value = (uint16_t)va_arg(ap, int);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_INT16:
            {   argv[i] = MALLOC(sizeof(int16_t));
                *(int16_t*)argv[i] = (int16_t)va_arg(ap, int);
                break; }
            case TYPE_UINT16:
            {   argv[i] = MALLOC(sizeof(uint16_t));
                *(uint16_t*)argv[i] = (uint16_t)va_arg(ap, int);
                break; }
#endif
#if SIZEOF_VOID_PTR >= 4
            case TYPE_INT32:
            {   int32_t value = va_arg(ap, int32_t);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT32:
            {   uint32_t value = va_arg(ap, uint32_t);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_INT32:
            {   argv[i] = MALLOC(sizeof(int32_t));
                *(int32_t*)argv[i] = va_arg(ap, int32_t);
                break; }
            case TYPE_UINT32:
            {   argv[i] = MALLOC(sizeof(uint32_t));
                *(uint32_t*)argv[i] = va_arg(ap, uint32_t);
                break; }
#endif
#if SIZEOF_VOID_PTR >= 8
            case TYPE_INT64:
            {   int64_t value = va_arg(ap, int64_t);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINT64:
            {   uint64_t value = va_arg(ap, uint64_t);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_INT64:
            {   argv[i] = MALLOC(sizeof(int64_t));
                *(int64_t*)argv[i] = (int64_t)va_arg(ap, int64_t);
                break; }
            case TYPE_UINT64:
            {   argv[i] = MALLOC(sizeof(uint64_t));
                *(uint64_t*)argv[i] = (uint64_t)va_arg(ap, uint64_t);
                break; }
#endif
/* ------------------------------------------------------------------------- */
            /* intptr is by definition the same size as void* */
            case TYPE_INTPTR:
            {   intptr_t value = (intptr_t)va_arg(ap, int);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
            case TYPE_UINTPTR:
            {   uintptr_t value = (uintptr_t)va_arg(ap, int);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
/* ------------------------------------------------------------------------- */
            /* floating point types */
#if SIZEOF_VOID_PTR >= SIZEOF_FLOAT
            case TYPE_FLOAT:
            {   float value = (float)va_arg(ap, double);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_FLOAT:
            {   argv[i] = MALLOC(sizeof(float));
                *(float*)argv[i] = (float)va_arg(ap, double);
                break; }
#endif
#if SIZEOF_VOID_PTR >= SIZEOF_DOUBLE
            case TYPE_DOUBLE:
            {   double value = va_arg(ap, double);
                argv[i] = argv + type_info->argc + i;
                memcpy(argv[i], &value, sizeof value);
                break; }
#else
            case TYPE_DOUBLE:
            {   argv[i] = MALLOC(sizeof(double));
                *(double*)argv[i] = va_arg(ap, double);
                break; }
#endif
/* ------------------------------------------------------------------------- */
            /* none types can't be passed as an argument */
            case TYPE_NONE:
                fprintf(stderr, "Cannot create argument in vector: Invalid type!\n");
                return 0;
/* ------------------------------------------------------------------------- */
            default:
                fprintf(stderr, "Cannot create argument in vector: Unknown type!\n");
                return 0;
        }
    }

    return 1;
}

/* ------------------------------------------------------------------------- */
void
dynamic_call_destroy_argument_vector(const struct type_info_t* type_info,
                                     void** argv)
{
    uint32_t i;
    for(i = 0; i != type_info->argc; ++i)
    {
        switch(type_info->argv_type[i])
        {
            /* free strings */
            case TYPE_STRING:
            case TYPE_WSTRING:
                free_string(argv[i]);
                break;

            /* nothing to free */
            case TYPE_INT8:
            case TYPE_UINT8:
                break;

            /* free 16-64 bit integers, floats, and doubles */
#if SIZEOF_VOID_PTR < 2
            case TYPE_INT16:
            case TYPE_UINT16:
#endif
#if SIZEOF_VOID_PTR < 4
            case TYPE_INT32:
            case TYPE_UINT32:
#endif
#if SIZEOF_VOID_PTR < 8
            case TYPE_INT64:
            case TYPE_UINT64:
#endif
#if SIZEOF_VOID_PTR < SIZEOF_FLOAT
            case TYPE_FLOAT:
#endif
#if SIZEOF_VOID_PTR < SIZEOF_DOUBLE
            case TYPE_DOUBLE:
#endif
                FREE(argv[i]);
                break;

            default:
                break;
        }
    }

    FREE(argv);
}

/* ------------------------------------------------------------------------- */
char
dynamic_call_do_typecheck(const struct type_info_t* type_info,
                          const char* ret_type,
                          uint32_t argc,
                          const char** argv)
{
    uint32_t i;

    /* verify argument count */
    if(argc != type_info->argc)
    {
        return 0;
    }

    /* verify return type */
    if(!ret_type ||
        dynamic_call_get_type_from_string(ret_type) != type_info->ret_type)
    {
        return 0;
    }

    /* verify argument types */
    for(i = 0; i != argc; ++i)
    {
        if(!argv[i] ||
            dynamic_call_get_type_from_string(argv[i]) != type_info->argv_type[i])
        {
            return 0;
        }
    }

    /* valid! */
    return 1;
}

/* ------------------------------------------------------------------------- */
/* Static functions */
/* ------------------------------------------------------------------------- */
type_e
dynamic_call_get_type_from_string(const char* type)
{
    /* integers */
    if(strstr(type, "int"))
    {
        /*
         * Here we inspect the string to be any of the following:
         * int, int8_t, int16_t, int32_t, int64_t, intptr_t and all of their
         * unsigned counterparts.
         *
         */

        /* default to an int32 */
        type_e ret = TYPE_INT32;

        /* reject pointer types */
        if(strstr(type, "*"))
            return TYPE_UNKNOWN;
        /* intptr_t */
        if(strstr(type, "intptr"))
            ret = TYPE_INTPTR;
        /* number of bits */
        else if(strstr(type, "8"))
            ret = TYPE_INT8;
        else if(strstr(type, "16"))
            ret = TYPE_INT16;
        else if(strstr(type, "32"))
            ret = TYPE_INT32;
        else if(strstr(type, "64"))
            ret = TYPE_INT64;

        /* sign */
        if(strchr(type, 'u'))
            ++ret;
        return ret;
    }

    /* strings (or an int8/uint8) */
    {
        /* any form of wchar_t* is acceptable */
        if(strstr(type, "wchar_t"))
        {
            /* make sure it's not actually a wchar_t** */
            int num_chrs = 0;
            const char* str = type;
            /* one-liner for counting the number of '*' characters in a string
             * NOTE: modifies "str" */
            for(num_chrs = 0; str[num_chrs]; str[num_chrs] == '*' ? num_chrs++ : *str++);
            /* make sure it is a wchar_t* */
            if(num_chrs == 1)
                return TYPE_WSTRING;
            return TYPE_UNKNOWN;
        }

        /* any form of char* is acceptable */
        if(strstr(type, "char"))
        {
            /* make sure it's not actually a char** */
            int num_chrs = 0;
            const char* str = type;
            /* one-liner for counting the number of '*' characters in a string
             * NOTE: modifies "str" */
            for(num_chrs = 0; str[num_chrs]; str[num_chrs] == '*' ? num_chrs++ : *str++);
            if(num_chrs >= 2)
                return TYPE_UNKNOWN;
            /* make sure it is a char* */
            if(num_chrs == 1)
                return TYPE_STRING;

            /* it's only a char - determine if signed or unsigned, and return
             * either int8 or uint8 */
            if(strstr(type, "u"))
                return TYPE_UINT8;
            return TYPE_INT8;
        }
    }

    /* floats */
    if(strstr(type, "float"))
    {
        /* make sure it's not actually a float* */
        if(strstr(type, "*"))
            return TYPE_UNKNOWN;
        return TYPE_FLOAT;
    }

    /* doubles */
    if(strstr(type, "double"))
    {
        /* make sure it's not actually a float* */
        if(strstr(type, "*"))
            return TYPE_UNKNOWN;
        return TYPE_DOUBLE;
    }

    /* none/void */
    if(strstr(type, "void"))
    {
        /* reject pointer types */
        if(strstr(type, "*"))
            return TYPE_UNKNOWN;
        return TYPE_NONE;
    }

    /* unknown */
    return TYPE_UNKNOWN;
}

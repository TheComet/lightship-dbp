#include "util/dynamic_call.h"
#include "util/memory.h"
#include "util/ordered_vector.h"
#include "util/string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
void**
dynamic_call_create_argument_vector_from_strings(const struct type_info_t* type_info,
                                                 const struct ordered_vector_t* argv)
{
    void** ret;

    assert(type_info);
    assert(argv);

    /* check argument count */
    if(type_info->argc != argv->count)
    {
        char argc_provided[sizeof(int)*8+1];
        char argc_required[sizeof(int)*8+1];
        sprintf(argc_provided, "%d", (int)argv->count);
        sprintf(argc_required, "%d", type_info->argc);
        fprintf(stderr, "Cannot create argument list: Wrong number of arguments\n");
        fprintf(stderr, "    Required: %d\n", argc_required);
        fprintf(stderr, "    Provided: %d\n", argc_provided);
        return NULL;
    }

    /* create void** argument vector */
    ret = (void**)MALLOC(type_info->argc * sizeof(void*));
    if(!ret)
        OUT_OF_MEMORY("dynamic_call_create_argument_vector_from_strings()", NULL);
    memset(ret, 0, type_info->argc * sizeof(void*));

    /* parse argument vector */
    {
        int i = 0;
        char failed = 0;
        ORDERED_VECTOR_FOR_EACH(argv, const char*, str_p)
        {
            const char* str = *str_p;

            switch(type_info->argv_type[i])
            {
                /* ------------------------------------------------------------------------- */
                /* string types */
                case TYPE_STRING:
                    ret[i] = malloc_string(str);
                    break;
                case TYPE_WSTRING:
                    ret[i] = malloc_wstring((wchar_t*)str);
                    break;
                /* ------------------------------------------------------------------------- */
                /* integer types */
                case TYPE_INT8:
                {   int8_t value = (int8_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
                case TYPE_UINT8:
                {   uint8_t value = (uint8_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
#if SIZEOF_VOID_PTR >= 16
                case TYPE_INT16:
                {   int16_t value = (int16_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
                case TYPE_UINT16:
                {   uint16_t value = (uint16_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
#else
                case TYPE_INT16:
                {   ret[i] = MALLOC(sizeof(int16_t));
                    *(int16_t*)ret[i] = (int16_t)atoi(str);
                    break; }
                case TYPE_UINT16:
                {   ret[i] = MALLOC(sizeof(uint16_t));
                    *(uint16_t*)ret[i] = (uint16_t)atoi(str);
                    break; }
#endif
#if SIZEOF_VOID_PTR >= 32
                case TYPE_INT32:
                {   int32_t value = (int32_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
                case TYPE_UINT32:
                {   uint32_t value = (uint32_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
#else
                case TYPE_INT32:
                {   ret[i] = MALLOC(sizeof(int32_t));
                    *(int32_t*)ret[i] = (int32_t)atoi(str);
                    break; }
                case TYPE_UINT32:
                {   ret[i] = MALLOC(sizeof(uint32_t));
                    *(uint32_t*)ret[i] = (uint32_t)atoi(str);
                    break; }
#endif
#if SIZEOF_VOID_PTR >= 64
                case TYPE_INT64:
                {   int64_t value = (int64_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
                case TYPE_UINT64:
                {   uint64_t value = (uint64_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
#else
                case TYPE_INT64:
                {   ret[i] = MALLOC(sizeof(int64_t));
                    *(int64_t*)ret[i] = (int64_t)atoi(str);
                    break; }
                case TYPE_UINT64:
                {   ret[i] = MALLOC(sizeof(uint64_t));
                    *(uint64_t*)ret[i] = (uint64_t)atoi(str);
                    break; }
#endif
                /* ------------------------------------------------------------------------- */
                /* intptr is by definition the same size as void* */
                case TYPE_INTPTR:
                {   intptr_t value = (intptr_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
                case TYPE_UINTPTR:
                {   uintptr_t value = (uintptr_t)atoi(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
                /* ------------------------------------------------------------------------- */
#if SIZEOF_VOID_PTR >= SIZEOF_FLOAT
                case TYPE_FLOAT:
                {   float value = (float)atof(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
#else
                case TYPE_FLOAT:
                {   ret[i] = MALLOC(sizeof(float));
                    *(float*)ret[i] = (float)atoi(str);
                    break; }
#endif
#if SIZEOF_VOID_PTR >= SIZEOF_DOUBLE
                case TYPE_DOUBLE:
                {   double value = (double)atof(str);
                    memcpy(ret + i, &value, sizeof value);
                    break; }
#else
                case TYPE_DOUBLE:
                {   ret[i] = MALLOC(sizeof(double));
                    *(double*)ret[i] = (double)atoi(str);
                    break; }
#endif
                /* ------------------------------------------------------------------------- */
                default:
                    fprintf(stderr, "Cannot create argument vector: Unknown type \"%s\"\n", str);
                    failed = 1;
                    break;
            }

            ++i;
        }

        /* if any of the conversions failed, free the list and return NULL */
        if(failed)
        {
            dynamic_call_destroy_argument_vector(type_info, ret);
            ret = NULL;
        }
    }

    return ret;
}

/* ------------------------------------------------------------------------- */
void
dynamic_call_destroy_argument_vector(const struct type_info_t* type_info,
                                     void** argv)
{
    uint32_t i;
    for(i = 0; i != type_info->argc; ++i)
        if(argv[i])
            FREE(argv[i]);

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
        return 0;
    /* verify return type */
    if(!ret_type || strcmp(ret_type, type_info->ret_type_str))
        return 0;
    /* verify argument types */
    for(i = 0; i != argc; ++i)
        if(!argv[i] || strcmp(argv[i], type_info->argv_type_str[i]))
            return 0;

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
        service_type_e ret = TYPE_INT32;

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

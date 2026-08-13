#pragma once

#define ARRAY(T, name) T *name; size_t name##_len;

/*
Doesn't need after use C23 standard instead C17 (selected as
default in IDE)
typedef enum {
    true = 1,
    false = 0,
} bool;
*/

#define nameof(any) #any
#define generic _Generic

typedef signed   char   sbyte;
typedef unsigned char   byte;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define KB 1024
#define MB (KB*KB)
#define GB (MB*MB)

#define DEF_RESULT(func_name, TValue) struct func_name##_Result { \
    enum func_name##_ResCode code;\
    TValue value;\
}\

// Set code only
#define ERR(func_name, code_) \
    (struct func_name##_Result) { \
        .code = func_name##_##code_ \
    }

// For union ResultValue-s with error data, like IOReadErrorData
#define EXTERR(func_name, code_, data) \
    (struct func_name##_Result) { \
        .code = func_name##_##code_, \
        .value = {.code_ = data} \
    }

#define OK(func_name, value_) \
    (struct func_name##_Result) { \
        .code = func_name##_Success, \
        .value = value_ \
    }


#define ROUND_UP_TO_8(x) (((x) + 7) &~ 7u)
#define MIN(x, y) ((x) < (y) ? x : y)

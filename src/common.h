#pragma once

#define ARRAY(T, name) T *name; size_t name##_len;


typedef enum {
    true = 1,
    false = 0,
} bool;

#define nameof(some) "some"

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

// Need specify full code (like "read_http_request_IOReadError" instead just "IOReadError")
// for allow IDE refactoring actions (like renaming)
#define ERR(func_name, code_) (struct func_name##_Result){.code = code_}
#define OK(func_name, value_) (struct func_name##_Result){.code = func_name##_Success, .value = value_};


#define ROUND_UP_TO_8(x) (((x) + 7) &~ 7u)
#define MIN(x, y) ((x) < (y) ? x : y)

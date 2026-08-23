#pragma once
#include <stdint.h>

#define ARRAY(T, name) T *name; size_t name##_len;

/*
Doesn't need after use C23 standard instead C17 (selected as
default in IDE)
typedef enum {
    true = 1,
    false = 0,
} bool;

Saved for history (it is pet-project, not production, i can just comment it
instead remove by git commit like in prod)
*/

#define nameof(any) #any
#define generic _Generic

/*
 * Important: short, int and long is not a "i16, i32, i64".
 * Need use stdint for strict-sized types.
 *
 * Sizes by standard:
 *     short >= 16,
 *     int   >= 16,
 *     long  >= 32;
 *     long long >= 64;
 * Sizes on Linux x64:
 *     short == 16,
 *     int   == 32,
 *     long  == 64;
 *     long long == 64;
 * Sizes on god damn WINDOWS x64:
 *     short == 16,
 *     int   == 32,
 *     long  == 32; <---
 *     long long == 64;
*/
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i8 sbyte;
typedef u8 byte;

#define KB 1024


// Maybe only ResCode + T *out param would be better.
#define DEF_RESULT(func_name, TValue) struct func_name##_Result { \
    enum func_name##_ResCode code;\
    TValue value;\
}

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

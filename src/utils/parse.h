#pragma once
#include "src/common.h"

// Implement parse_<T>
#define IMPL_PARSE_NULL_TERMINATED_STR(str_p, out_p, parse_n_func, len_p) \
    /* I knew about strlen, but would not to include string.h here */ \
    size_t len_p = 0; \
    { \
        const char *current = str_p; \
        while (*current != '\0') { \
            current++; \
            len_p++; \
        } \
    } \
    return parse_n_func(str_p, len_p, out_p);

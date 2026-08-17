/*
 * Maybe move it in some "parsing utils"? Or "parsing stuff"?
 * Here only:
 * - Code gen macro
*/

#pragma once
#include "src/common.h"

// Create body of parse_<T> impl
#define IMPL_PARSE_NULL_TERMINATED_STR(str_p, out_p, parse_n_func, len_p) \
    size_t len_p = 0; \
    { \
        char *current = str_p; \
        while (*current != '\0') { \
            current++; \
            len_p++; \
        } \
    } \
    return parse_n_func(str_p, len_p, out_p);

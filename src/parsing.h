#pragma once

#include <stdlib.h>
#include "src/common.h"

// TODO: Rewrite to simple wariant in C# style (parse(str, out) -> bool)
enum parse_num_ResCode {
    parse_num_Success,
    parse_num_IsNotANumber,
};
DEF_RESULT(parse_num, long);

// variant for not null-terminated str
struct parse_num_Result sparse_num(const char *str, size_t str_len);
struct parse_num_Result parse_num(const char *str);

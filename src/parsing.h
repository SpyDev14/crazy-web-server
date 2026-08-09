#pragma once

#include <stdlib.h>
#include "src/common.h"


DEF_RESULT(parse_num, long);
enum parse_num_ResCode {
    parse_num_Success,
    parse_num_IsNotANumber,
};

// variant for not null-terminated str
struct parse_num_Result sparse_num(const char *str, size_t str_len);
struct parse_num_Result parse_num(const char *str);

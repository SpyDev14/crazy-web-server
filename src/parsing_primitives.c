#include <stdlib.h>
#include <limits.h>

#include "src/common.h"
#include "src/generic/parse.h"
#include "./parsing_primitives.h"


bool parse_n_u64(const char *str, size_t len, u64 *out) {
    if (len == 0) return false;

    u64 num = 0;
    const char *curr = str;

    while (curr - str < len) {
        if (*curr < '0' || *curr > '9') return false;
        u8 digit = *curr - '0'; // 0-9

        if (num > (INT64_MAX - digit) / 10) {
            return false;
        }

        num = num * 10 + digit;
        curr++;
    }

    *out = num;
    return true;
}

bool parse_n_u64(const char *str, size_t len, u64 *out) {
    IMPL_PARSE_NULL_TERMINATED_STR(str, out, parse_n_u64, len)
}


bool parse_n_i64(const char *str, size_t len, i64 *out) {
    if (len == 0) return false;

    bool is_neg = *str == '-';

    i64 num;
    if (!parse_n_u64(
        is_neg ? str + 1: str,
        is_neg ? len - 1: len,
        &num
    )) return false;

    if (is_neg) {
        num = -num;
    }
    *out = num;
    return true;
}

bool parse_i64(const char *str, u64 *out) {
    IMPL_PARSE_NULL_TERMINATED_STR(str, out, parse_n_i64, len)
}

#include <stdlib.h>

#include "src/common.h"
#include "src/generic/parse.h"
#include "./parsing.h"


bool parse_n_ulong(const char *str, size_t len, long *out) {
    if (len == 0) return false;

    long num = 0;
    const char *curr = str;

    while (curr - str < len) {
        if (*curr < '0' || *curr > '9') return false;

        num = num * 10 + (*curr - '0');
        curr++;
    }

    *out = num;
    return true;
}

bool parse_n_long(const char *str, size_t len, long *out) {
    if (len == 0) return false;

    long num = 0;
    bool is_neg = false;
    const char *curr = str;

    if (*curr == '-') {
        is_neg = true;
        curr++;
    }

    while (curr - str < len) {
        if (*curr < '0' || *curr > '9') return false;

        num = num * 10 + (*curr - '0');
        curr++;
    }

    if (is_neg) num = -num;
    *out = num;
    return true;
}

bool parse_long(const char *str, long *out) {
    IMPL_PARSE_NULL_TERMINATED_STR(str, len, out, parse_n_long);
}

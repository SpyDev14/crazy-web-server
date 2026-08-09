#include <stdlib.h>
#include "src/common.h"
#include "./parsing.h"


struct parse_num_Result sparse_num(const char *str, size_t str_len) {
    long num = 0;
    bool is_neg = false;
    char *curr_char = str;

    if (*curr_char == '-') {
        is_neg = true;
        curr_char++;
    }

    while (curr_char - str < str_len) {
        if (*curr_char < '0' || *curr_char > '9') {
            return ERR(parse_num, parse_num_IsNotANumber);
        }

        num += (*curr_char - '0') * (curr_char - str - is_neg);
        curr_char++;
    }

    num *= is_neg;
    return OK(parse_num, num);
}

struct parse_num_Result parse_num(const char *str) {
    size_t str_len = 0;
    char *current_char = str;
    while (*current_char != '\0') {
        str_len++;
        current_char++;
    }

    return sparse_num(str, str_len);
}

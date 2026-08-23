#pragma once

#define IS_VISIBLE_ASCII(c) (33 <= c && c <= 126)
#define IS_ASCII_UPPER_CASE_LETTER(c) (65 <= c && c <= 90)
#define IS_ASCII_LOWER_CASE_LETTER(c) (97 <= c && c <= 122)
#define IS_ASCII_LETTER(c) (IS_ASCII_UPPER_CASE_LETTER(c) || IS_ASCII_LOWER_CASE_LETTER(c))

#define IS_DIGIT(c) (48 <= c && c <= 57)

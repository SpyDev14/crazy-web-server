#pragma once
#include <stdio.h>


#define DEF_REPR_HEAD(type_name, type_prefix) \
char *type_name##_repr(const type_prefix type_name *self)

#define DEF_REPR_IMPL(type_name, type_prefix) \
DEF_REPR_HEAD(type_name, type_prefix) { \
    char fmt[] = "<struct type_name at %p>"; \
    int len = 1 + snprintf(NULL, 0, fmt, self); \
    char *buff = malloc(len); \
    if (buff == NULL) { return NULL; } \
    snprintf(buff, len, fmt, self); \
    return buff; \
}\

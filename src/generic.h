#pragma once
#include <stdio.h>


// Format params:
// - separator: "" if type_prefix == "", else " "
// - pointer: void *
// defined as macro just for being near to REQUIRED_REPR_STR_LEN
#define __REPR_FMT_FOR(type_name, type_prefix) ("<" #type_prefix "%s" #type_name " at %p>")

// Return repr str len for given type WITHOUT NULL TERMINATOR. Runtime size can be lower
// than it because %p size not defined in standard and can change.
#define REPR_LEN(T) ( \
    sizeof('<')      +\
    sizeof(#T)-1     + /* - null-terminator */ \
    sizeof(" at ")-1 + /* - null-terminator */ \
    sizeof("0x")-1   + /* - null-terminator */ \
    (sizeof(void *) * 2) + /* 1111 1111 -> FF (1 byte of data -> 2 bytes of text) */ \
    sizeof('>') \
) \


// You can known need buffer length at comp-time by `REPR_LEN`
// macro (`REPR_LEN` + 1 for null-terminator)
#define DEF_REPR_HEAD(type_name, type_prefix) \
int type_name##_repr(const type_prefix type_name *self, char out[], size_t out_len)\
//     ^^^^^^^^^^^^^^^^
// Prefix need because need separated name for generate func-name (like "Vec_repr")

#define DEF_REPR_IMPL(type_name, type_prefix) \
DEF_REPR_HEAD(type_name, type_prefix) { \
    char fmt[] = __REPR_FMT_FOR(type_name, type_prefix); \
    char *sep = *#type_prefix == '\0' ? "" : " "; \
    return snprintf(out, out_len, fmt, sep, self); \
}\
//

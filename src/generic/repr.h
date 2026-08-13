#pragma once
#include <stdio.h>


// Format params:
// - separator: "" if type_tag == "", else " "
// - pointer: void *
// defined as macro just for being near to REQUIRED_REPR_STR_LEN
#define _REPR_FMT(type_name, type_tag) ("<" #type_tag "%s" #type_name " at %p>")

// Return repr str len for given type WITHOUT NULL TERMINATOR. Runtime size can be lower
// than it because %p size not defined in standard and can change.
#define REPR_LEN(T) ( \
    sizeof('<')      + \
    sizeof(#T)-1     + /* - null-terminator */ \
    sizeof(" at ")-1 + /* - null-terminator */ \
    sizeof("0x")-1   + /* - null-terminator */ \
    (sizeof(void *) * 2) + /* 1111 1111 -> FF (1 byte of data -> 2 bytes of text) */ \
    sizeof('>') \
) \


// Declare repr func for given type.
// About declared func:
// You can known need buffer length at comp-time by `REPR_LEN` macro
// (`REPR_LEN` + 1 for null-terminator)
#define DECLARE_REPR_FOR(type_name, type_tag) \
int type_name##_repr(const type_tag type_name *self, char out[], size_t out_len) \
//  ^^^^^^^^^^^^^^^^
// Prefix need because need separated name for generate func-name (like "Vec_repr")

#define DEFINE_REPR_FOR(type_name, type_tag) \
DECLARE_REPR_FOR(type_name, type_tag) { \
    char fmt[] = _REPR_FMT(type_name, type_tag); \
    char *sep = *#type_tag == '\0' ? "" : " "; \
    return snprintf(out, out_len, fmt, sep, self); \
} \

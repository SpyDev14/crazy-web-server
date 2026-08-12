#pragma once
#include "src/common.h"


#define _CONF_FIELDS \
    X(ushort, PORT)\
    X(ushort, CONNECTION_QUEUE_SIZE)\

struct Config {
    #define X(T, name) T name;
    _CONF_FIELDS
    #undef X
};

/* Special funcs */
// NOT THREAD SAFE
struct Config new_Config_from_env(const char *prefix);
struct Config new_Config_from_command_line(const int argc, const char *argv[]);

/* Read-only methods */
// Update values in copy of first config by values from other config if set.
// Return updated copy.
struct Config Config_merge(const struct Config *self, const struct Config other);

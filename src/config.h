#pragma once
#include "src/common.h"


#define _CONF_FIELDS \
    X(u16, PORT)\
    X(u16, CONNECTION_QUEUE_SIZE)\

struct Config {
    #define X(T, name) T name;
    _CONF_FIELDS
    #undef X
};

/* Special funcs */
// NO THREAD SAFE
struct Config new_Config_from_env(const char *prefix);
struct Config new_Config_from_command_line(int argc, const char *argv[]);

/* Read-only methods */
// Update values in copy of first config by values from other config if set.
// Return updated copy.
struct Config Config_merge(const struct Config *this, const struct Config *other);

/* Mutate methods */
// Merge with exists config mutation
void Config_update(struct Config *this, const struct Config *other);

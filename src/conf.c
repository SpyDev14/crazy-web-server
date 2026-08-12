#include <stdlib.h>
#include "./conf.h"


const char *_CONF_FIELD_NAMES[] = {
    #define X(T, name) #name,
    _CONF_FIELDS
    #undef X
};

const int _CONF_FIELDS_COUNT = (
    #define X(T, name) 1 +
    _CONF_FIELDS 0
    #undef X
);


/* Special funcs */
struct Config new_Config_from_env(const char *prefix) {
    struct Config conf = {0};
    for (int i = 0; i < _CONF_FIELDS_COUNT; i++) {
        _CONF_FIELD_NAMES[i];

    }
}
struct Config new_Config_from_command_line(const int argc, const char *argv[]);

/* Read-only methods */
struct Config Config_merge(const struct Config *self, const struct Config other);

#include <stdlib.h>

#include "src/logging.h"
#include "./config.h"


static const char *m_CONF_FIELD_NAMES[] = {
    #define X(T, name) #name,
    _CONF_FIELDS
    #undef X
};

static const size_t m_CONF_FIELDS_COUNT = (
    #define X(T, name) 1 +
    _CONF_FIELDS 0
    #undef X
);


/* Special funcs */
struct Config new_Config_from_env(const char *prefix) {
    struct Config conf = {0};
    log(L_ERROR, L_ARGS, "not implemented");
    return conf;
}

struct Config new_Config_from_command_line(int argc, const char *argv[]) {
    struct Config conf = {0};
    log(L_ERROR, L_ARGS, "not implemented");
    return conf;
}

/* Read-only methods */
struct Config Config_merge(const struct Config *this, const struct Config *other) {
    log(L_ERROR, L_ARGS, "not implemented");
    return *this;
}

/* Mutate methods */
void Config_update(struct Config *this, const struct Config *other) {
    log(L_ERROR, L_ARGS, "not implemented");
}

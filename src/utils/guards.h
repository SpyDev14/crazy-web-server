#pragma once
#include "src/logging.h"

#define NOT_ZERO_GUARD(param, return_value) \
    if (param == 0) { \
        log(L_ERROR, L_ARGS, #param " cannot be zero"); \
        return return_value; \
    }

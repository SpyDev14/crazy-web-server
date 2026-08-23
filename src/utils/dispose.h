#pragma once
#include "src/logging.h"

#define LOG_SUCCESSFUL_DISPOSED(repr_fmt, this_ptr) \
    log(L_DEBUG, L_ARGS, repr_fmt " disposed", this_ptr);

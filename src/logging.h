#pragma once
#include <stdarg.h>
#include "src/common.h"

enum LoggingLevel {
    L_DEBUG = 10,
    L_INFO = 20,
    L_WARNING = 30,
    L_ERROR = 40,
    L_FATAL = 50,
};

const char *LoggingLevel_to_string(enum LoggingLevel this);

struct LogArgs {
    const char *func_name; // __func__ (implicit function-local static array)
    const char *file_name; // __NAME__ (macro)
    int line; // __LINE__ (macro)
};

void vlog(enum LoggingLevel level, struct LogArgs log_args, const char *fmt, va_list args);
void log(enum LoggingLevel level, struct LogArgs log_args, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

#define L_ARGS (struct LogArgs) { \
    .func_name = __func__, \
    .file_name = __FILE__, \
    .line = __LINE__ \
}

// I make decision to not use LOG macro for save check fmt arg types by IDE
#define LOG(level, fmt, ...) log(level, L_ARGS, fmt __VA_OPT__(,) __VA_ARGS__)

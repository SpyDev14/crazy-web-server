#pragma once
#include <stdarg.h>

enum LoggingLevel {
    L_DEBUG = 10,
    L_INFO = 20,
    L_WARNING = 30,
    L_ERROR = 40,
    L_FATAL = 50,
};

const char *LoggingLevel_to_string(enum LoggingLevel this);

struct LogArgs {
    char *func_name;
    char *file_name;
    u32 line;
};

void vlog(enum LoggingLevel level, struct LogArgs log_args, const char *fmt, va_list args);
void log(enum LoggingLevel level, struct LogArgs log_args, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

#define L_ARGS (struct LogArgs) { \
    .func_name = __func__, \
    .file_name = __FILE__, \
    .line = __LINE__ \
}

#pragma once
#include <stdarg.h>

enum LoggingLevel {
    L_DEBUG = 10,
    L_INFO = 20,
    L_WARNING = 30,
    L_ERROR = 40,
    L_FATAL = 50,
};

const char *LoggingLevel_to_string(enum LoggingLevel self);
void vlog(enum LoggingLevel level, const char *fmt, va_list args);
void log(enum LoggingLevel level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

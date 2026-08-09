#include <stdio.h>
#include <stdarg.h>

#include "src/common.h"
#include "src/logging.h"


const char *LoggingLevel_to_string(enum LoggingLevel self) {
    switch (self) {
        case L_DEBUG:   return "DEBUG";
        case L_INFO:    return "INFO";
        case L_WARNING: return "WARNING";
        case L_ERROR:   return "ERROR";
        case L_FATAL:   return "FATAL";
        default:        return "UNKNOWN";
    }
}

void vlog(enum LoggingLevel level, char *fmt, va_list args) {
    fprintf(stderr, "[%s] ", LoggingLevel_to_string(level));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void log(enum LoggingLevel level, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(level, fmt, args);
    va_end(args);
}

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

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

void vlog(enum LoggingLevel level, const char *fmt, va_list args) {
    char time_str[64] = {0};
    {
        time_t time_now = time(NULL);
        struct tm tm = *localtime(&time_now);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S %z", &tm);
    }

    fprintf(stderr, "[%s] %s: ",
        LoggingLevel_to_string(level),
        time_str
    );
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void log(enum LoggingLevel level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(level, fmt, args);
    va_end(args);
}

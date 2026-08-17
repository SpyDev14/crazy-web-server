#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "src/console/ansi.h"
#include "src/common.h"
#include "src/logging.h"


const char *LoggingLevel_to_string(enum LoggingLevel this) {
    switch (this) {
        case L_DEBUG:   return "DEBUG";
        case L_INFO:    return "INFO";
        case L_WARNING: return "WARNING";
        case L_ERROR:   return "ERROR";
        case L_FATAL:   return "FATAL";
        default:        return "UNKNOWN";
    }
}
const char *LoggingLevel_to_color(enum LoggingLevel this) {
    switch (this) {
        case L_DEBUG:   return ANSI_FORE_BLUE;
        case L_INFO:    return ANSI_FORE_CYAN;
        case L_WARNING: return ANSI_FORE_YELLOW;
        case L_ERROR:   return ANSI_FORE_RED;
        case L_FATAL:   return ANSI_FORE_MAGENTA;
        default:        return "";
    }
}

void vlog(enum LoggingLevel level, struct LogArgs log_args, const char *fmt, va_list args) {
    char time_str[64] = {0};
    {
        time_t time_now = time(NULL);
        struct tm tm = *localtime(&time_now);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S %z", &tm);
    }

    fprintf(stderr, "[%s%s%s] %s %s:%s:%s: ",
        LoggingLevel_to_color(level),
        LoggingLevel_to_string(level),
        ANSI_FORE_RESET,
        time_str,
        log_args.file_name,
        log_args.func_name,
        log_args.line
    );
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void log(enum LoggingLevel level, struct LogArgs log_args, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(level, log_args, fmt, args);
    va_end(args);
}

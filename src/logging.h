enum LoggingLevel {
    LL_DEBUG = 10,
    LL_INFO = 20,
    LL_WARNING = 30,
    LL_ERROR = 40,
    LL_FATAL = 50,
};
char *to_string(enum LoggingLevel self);

void log(enum LoggingLevel level, char *msg);

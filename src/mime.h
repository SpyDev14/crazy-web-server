#pragma once

// Frozen to future
#if FALSE
#include "src/common.h"

// X - X(id, extensions, str)
#define MIME_TYPES \
    X(TEXT_HTTP, {"http", NULL} "text/http")X_SEP \
    X(APP_JSON, {"json", NULL}, "application/json")

enum MimeType : u16 {
    #define X(id, ext, str) MIME_##id
    #define X_SEP ,
        MIME_TYPES,
    #undef X
    #undef X_SEP

    MIME_UNKNOWN
};
#endif

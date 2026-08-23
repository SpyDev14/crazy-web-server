#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "src/generic/parse.h"
#include "src/utils/text.h"
#include "src/logging.h"
#include "src/parsing_primitives.h"
#include "./http.h"

// REQ - Request
#define REQ_READ_BUFF_SZ 16 * KB

const char *HTTPResponseCode_to_reason_phrase(enum HTTPResponseCode this) {
    switch (this) {
        // 1xx: Informational
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        case 103: return "Early Hints";
        case 104: return "Upload Resumption Supported";

        // 2xx: Success
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        case 207: return "Multi-Status";
        case 208: return "Already Reported";
        case 226: return "IM Used";

        // 3xx: Redirection
        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 305: return "Use Proxy";
        case 306: return "Unused";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";

        // 4xx: Client Error
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Content Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        case 418: return "I'm a teapot";
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Content";
        case 423: return "Locked";
        case 424: return "Failed Dependency";
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 451: return "Unavailable For Legal Reasons";

        // 5xx: Server Error
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 506: return "Variant Also Negotiates";
        case 507: return "Insufficient Storage";
        case 508: return "Loop Detected";
        case 510: return "Not Extended";
        case 511: return "Network Authentication Required";

        default: return "Unknown Status Code";
    }
}

/* HTTPHeader: Special functions */
enum parse_HTTPHeader_ResCode parse_n_HTTPHeader(const char *str, size_t len, struct HTTPHeader *out) {
    #define IS_ALLOWED_WHITESPACE(c) (c == ' ' || c == '\t')
    size_t name_len, value_len;
    char *curr = str;

    #define IN_BOUNDS (curr - str < len)
    #define IT_WAS_OUT_OF_BOUNDS (curr - str == len)

    while (IN_BOUNDS && *curr != ':') {
        if (!IS_VISIBLE_ASCII(*curr))
            return parse_HTTPHeader_InvalidString;
        curr++;
    }

    if (IT_WAS_OUT_OF_BOUNDS || curr == str)
        return parse_HTTPHeader_InvalidString;

    name_len = curr - str;
    curr++; // Skip ":"

    // Skipping whitespaces
    while(IN_BOUNDS && IS_ALLOWED_WHITESPACE(*curr))
        curr++;

    if (IT_WAS_OUT_OF_BOUNDS)
        return parse_HTTPHeader_InvalidString;

    char *value_start = curr;
    while (IN_BOUNDS) {
        if (!(IS_VISIBLE_ASCII(*curr) || IS_ALLOWED_WHITESPACE(*curr)))
            return parse_HTTPHeader_InvalidString;
        curr++;
    }

    if (value_len == value_start)
        return parse_HTTPHeader_InvalidString;

    value_len = curr - value_start;

    // Trimming right
    while (!IS_VISIBLE_ASCII(*curr)) {
        value_len--;
        curr--;
    }

    char *name = malloc(name_len + 1);
    if (name == NULL) {
        log(L_ERROR, L_ARGS, "malloc failed");
        return parse_HTTPHeader_MemoryOverflow;
    }
    name[name_len] = 0;

    char *value = malloc(value_len + 1);
    if (value == NULL) {
        log(L_ERROR, L_ARGS, "malloc failed");
        return parse_HTTPHeader_MemoryOverflow;
    }
    value[value_len] = 0;

    *out = (struct HTTPHeader) {
        .name = name,
        .value = value,
    };
    return parse_HTTPHeader_Success;
    #undef IS_ALLOWED_WHITESPACE
    #undef IN_BOUNDS
    #undef IT_WAS_OUT_OF_BOUNDS
}

enum parse_HTTPHeader_ResCode parse_HTTPHeader(const char *str, struct HTTPHeader *out) {
    IMPL_PARSE_NULL_TERMINATED_STR(str, out, parse_n_HTTPHeader, len)
}

void dispose_HTTPHeader(struct HTTPHeader *this) {
    free(this->name);
    free(this->value);

    this->name = NULL;
    this->value = NULL;

    log(L_DEBUG, L_ARGS, HTTP_HEADER_REPR_FMT, this);
}

/* Read-only methods */
size_t HTTPHeader_to_string(struct HTTPHeader *this, char *buff, size_t len) {
    return snprintf(buff, len, HTTP_HEADER_FMT, HTTP_HEADER_FMT_ARG(this));
}

/* HTTPVersion: Special functions */
bool parse_n_HTTPVersion(const char *str, size_t len, struct HTTPVersion *out) {
    // "HTTP/" literal lives in static memory
    // const here makes this pointer read-only,
    // but awaited_prefix can be changed (like awaited_prefix = NULL)
    const char *awaited_prefix = "HTTP/";
    const char *curr = str;

    // can change awaited_prefix by `c` without const
    for (const char *c = awaited_prefix; *c != '\0'; c++) {
        if (curr - str >= len) return false;
        if (*curr != *c) return false;

        curr++;
    }

    // Divide substrings
    // Needs initialization; by default it holds garbage, not 0x00.
    const char *major_start = NULL;
    const char *minor_start = NULL;
    u8 major_len = 0;
    u8 minor_len = 0;
    {
        // Reminder:
        // x = ++some; <=> some += 1; x = some;
        // x = some++; <=> x = some; some += 1;

        // Select major
        major_start = curr;
        while (curr - str < len && *curr != '.') {
            if (++major_len > 3) return false;
            curr++;
        }

        // Select minor if has
        // +1 for case: "HTTP/1.", else out of buffer
        if (curr + 1 - str < len && *curr == '.') {
            minor_start = ++curr;
            while (curr - str < len) {
                if (++minor_len > 3) return false;
                curr++;
            }
        }
    }

    if (major_len == 0) return false;
    if (major_len > 3) return false;
    if (minor_len > 3) return false;

    // u16 to prevent overflows
    u16 major = 0;
    u16 minor = 0;
    u64 num = 0;

    if (!parse_n_u64(major_start, major_len, &num)) return false;
    major = (u16)num;

    if (major > 255) return false;

    if (minor_start != NULL) {
        if (minor_len == 0) return false; // For example: "HTTP/1."
        if (!parse_n_u64(minor_start, minor_len, &num)) return false;
        minor = (u16)num;
    }

    if (minor > 255) return false;

    out->major = (u8)major;
    out->minor = (u8)minor;

    return true;
}

bool parse_HTTPVersion(const char *str, struct HTTPVersion *out) {
    IMPL_PARSE_NULL_TERMINATED_STR(str, out, parse_n_HTTPVersion, len)
}

/* HTTPMethod */
extern const HTTPMethod HTTP_METHOD_GET     = "GET";
extern const HTTPMethod HTTP_METHOD_POST    = "POST";
extern const HTTPMethod HTTP_METHOD_PUT     = "PUT";
extern const HTTPMethod HTTP_METHOD_PATCH   = "PATCH";
extern const HTTPMethod HTTP_METHOD_DELETE  = "DELETE";
extern const HTTPMethod HTTP_METHOD_HEAD    = "HEAD";
extern const HTTPMethod HTTP_METHOD_OPTIONS = "OPTIONS";

#define X(name) name,
static const HTTPMethod m_HTTP_METHODS_ARR = {
    HTTP_METHOD_GET,
};
#undef X

bool HTTPMethod_is_from_known(HTTPMethod this) {
}

bool HTTPMethod_is_known_n(HTTPMethod this, size_t n) {

}

bool HTTPMethod_is_valid_n(HTTPMethod this, size_t n) {

}

HTTPMethod HTTPMethod_get_from_known(char *str, size_t len) {

}

/* HTTPVersion: Read-only methods */
size_t HTTPVersion_to_string(struct HTTPVersion this, char *out, size_t out_len) {
    if (this.minor == 0)
        return snprintf(out, out_len, "HTTP/%u", this.major);
    return snprintf(out, out_len, "HTTP/%u.%u", this.major, this.minor);
}

enum m_ReadRequestState {
    READ_HEADERS,
    READ_BODY
};

struct read_http_request_Result read_http_request(
    const int client_fd,
    const struct sockaddr_in *client_addr,
    FreeablesVec *objects_to_free
) {
    struct HTTPRequest request = {
        .requester = {
            .ip = client_addr->sin_addr.s_addr,
            .port = client_addr->sin_port
        },
    };

    // 1. Read head part to buffer
    // 2. Read body if content-length was given

    enum m_ReadRequestState state = READ_HEADERS;
    size_t bytes_read_total = 0; // For reading result
    struct Vec headers = new_Vec(sizeof(struct HTTPHeader));
    size_t head_len = 0; // with \r\n\r\n

    char buff[REQ_READ_BUFF_SZ] = {0};
    size_t bytes_written = 0; // written in buff
    // Read head from socket (while not found \r\n\r\n)
    while (head_len == 0) {
        void *cursor = buff + bytes_written; //where to write
        size_t max_read = sizeof(buff - 1 - bytes_written); // -1 for null-term
        ssize_t bytes_read = read(client_fd, cursor, max_read);
        switch (bytes_read) {
            case -1: return EXTERR(read_http_request, IOReadError, {.error_code = errno});
            case 0:  return ERR(read_http_request, ClientCloseConnection);
        }
        bytes_read_total += bytes_read;
        bytes_written += bytes_read;
        if (bytes_written >= sizeof(buff)) {
            return ERR(read_http_request, RequestTooBig);
        }

        // cursor instead buff for optimization (no double scan)
        // small thing, but has matter
        char *end = strstr(cursor, "\r\n\r\n");
        if (end == NULL)
            continue; // while not found

        head_len = end - buff + 4;
        break;
    }

    size_t content_length = 0; // Getted from content-length header if has
    // Parse head
    // 1. Parse METHOD URI VERSION (first line)
    char *curr = buff;
    #define INVALID_REQUEST_ERR ERR(read_http_request, RequestInvalid)
    {
        char  *method_start, *uri_start, *version_start;
        size_t method_len,    uri_len,    version_len;

        method_start = curr;
        while (*curr != ' ') {
            if (*curr == 0) return INVALID_REQUEST_ERR;
            curr++;
        }
        method_len = curr - method_start;
        curr++;

        uri_start = curr;
        while (*curr != ' ') {
            if (*curr == 0) return INVALID_REQUEST_ERR;
            curr++;
        }
        uri_len = curr - uri_start;
        curr++;

        version_start = curr;
        while (*curr != '\r') {
            // to prevent full-scan if only LF instead CRLF (should be CRLF only)
            if (*curr == '\n' || *curr == 0) return INVALID_REQUEST_ERR;
            curr++;
        }
        version_len = curr - version_start;
        curr++;

        if (*curr != '\n') return INVALID_REQUEST_ERR;
        curr++;

        char *method, *uri;
        struct HTTPVersion version;
        if (!parse_n_HTTPVersion(version_start, version_len, &version))
            return INVALID_REQUEST_ERR;

        if (HTTPMethod_is_from_known())
    }

    // 2. Parse headers
    {

    }

    size_t content_read_total = 0; // For check what body has been read


    union read_http_request_ReturnValue value = {
        .Success = (struct read_http_request_SuccessData){
            .request = request,
            .bytes_read_total = bytes_read_total,
        }
    };
    return OK(read_http_request, value);
    #undef INVALID_REQUEST_ERR
}

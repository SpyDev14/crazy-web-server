#pragma once
#include <stdlib.h>
#include <netinet/in.h>

#include "src/collections/vec.h"
#include "src/generic/repr.h"
#include "src/freeable.h"
#include "src/common.h"

// Enum here is bad idea because status code is just a number
// But this project not parse Responses, so it isn't problem.
// Anyway, better way is `typedef u16 HTTPResponseCode` +
// `const static HTTPResponseCode HTTP_... = ...;`
enum HTTPResponseCode : u16 {
    /* 1xx: Informational */
    HTTP_100_CONTINUE = 100,
    HTTP_101_SWITCHING_PROTOCOLS = 101,
    HTTP_102_PROCESSING = 102,
    HTTP_103_EARLY_HINTS = 103,
    // TEMPORARY, Expires 2026-11-13. Now testing and probably be added to new standard.
    // Nginx & Apache already support this.
    HTTP_104_UPLOAD_RESUMPTION_SUPPORTED = 104,

    /* 2xx: Success */
    HTTP_200_OK = 200,
    HTTP_201_CREATED = 201,
    HTTP_202_ACCEPTED = 202,
    HTTP_203_NON_AUTHORITATIVE_INFORMATION = 203,
    HTTP_204_NO_CONTENT = 204,
    HTTP_205_RESET_CONTENT = 205,
    HTTP_206_PARTIAL_CONTENT = 206,
    HTTP_207_MULTI_STATUS = 207,
    HTTP_208_ALREADY_REPORTED = 208,
    HTTP_226_IM_USED = 226,

    /* 3xx: Redirection */
    HTTP_300_MULTIPLE_CHOICES = 300,
    HTTP_301_MOVED_PERMANENTLY = 301,
    HTTP_302_FOUND = 302,
    HTTP_303_SEE_OTHER = 303,
    HTTP_304_NOT_MODIFIED = 304,
    HTTP_305_USE_PROXY = 305,
    HTTP_306_UNUSED = 306,
    HTTP_307_TEMPORARY_REDIRECT = 307,
    HTTP_308_PERMANENT_REDIRECT = 308,

    /* 4xx: Client Error */
    HTTP_400_BAD_REQUEST = 400,
    HTTP_401_UNAUTHORIZED = 401,
    HTTP_402_PAYMENT_REQUIRED = 402,
    HTTP_403_FORBIDDEN = 403,
    HTTP_404_NOT_FOUND = 404,
    HTTP_405_METHOD_NOT_ALLOWED = 405,
    HTTP_406_NOT_ACCEPTABLE = 406,
    HTTP_407_PROXY_AUTHENTICATION_REQUIRED = 407,
    HTTP_408_REQUEST_TIMEOUT = 408,
    HTTP_409_CONFLICT = 409,
    HTTP_410_GONE = 410,
    HTTP_411_LENGTH_REQUIRED = 411,
    HTTP_412_PRECONDITION_FAILED = 412,
    HTTP_413_CONTENT_TOO_LARGE = 413,
    HTTP_414_URI_TOO_LONG = 414,
    HTTP_415_UNSUPPORTED_MEDIA_TYPE = 415,
    HTTP_416_RANGE_NOT_SATISFIABLE = 416,
    HTTP_417_EXPECTATION_FAILED = 417,
    // 1 april code from RFC 2324 at 1998 year, ~should~
    // be return to requests with "Coffee" header.
    HTTP_418_IM_A_TEAPOT = 418,
    HTTP_421_MISDIRECTED_REQUEST = 421,
    HTTP_422_UNPROCESSABLE_CONTENT = 422,
    HTTP_423_LOCKED = 423,
    HTTP_424_FAILED_DEPENDENCY = 424,
    HTTP_425_TOO_EARLY = 425,
    HTTP_426_UPGRADE_REQUIRED = 426,
    HTTP_428_PRECONDITION_REQUIRED = 428,
    HTTP_429_TOO_MANY_REQUESTS = 429,
    HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

    /* 5xx: Server Error */
    HTTP_500_INTERNAL_SERVER_ERROR = 500,
    HTTP_501_NOT_IMPLEMENTED = 501,
    HTTP_502_BAD_GATEWAY = 502,
    HTTP_503_SERVICE_UNAVAILABLE = 503,
    HTTP_504_GATEWAY_TIMEOUT = 504,
    HTTP_505_HTTP_VERSION_NOT_SUPPORTED = 505,
    HTTP_506_VARIANT_ALSO_NEGOTIATES = 506,
    HTTP_507_INSUFFICIENT_STORAGE = 507,
    HTTP_508_LOOP_DETECTED = 508,
    HTTP_510_NOT_EXTENDED = 510,
    HTTP_511_NETWORK_AUTHENTICATION_REQUIRED = 511
};

// Reminder: return null-terminated string in static memory
const char *HTTPResponseCode_to_reason_phrase(enum HTTPResponseCode this);

// DISPOSABLE
struct HTTPHeader {
    const char *name; // allocated
    const char *value; // allocated
};

#define HTTP_HEADER_REPR_FMT REPR_FMT(struct HTTPHeader)
#define HTTP_HEADER_FMT "%s: %s"
#define HTTP_HEADER_FMT_ARG(this) this->name, this->value

/* HTTPHeader: Special methods */
enum parse_HTTPHeader_ResCode {
    parse_HTTPHeader_Success,
    parse_HTTPHeader_InvalidString,
    parse_HTTPHeader_MemoryOverflow,
};

// Awaited format: "Name: Value"; allowed multiply whitespaces around Value
[[nodiscard]]
enum parse_HTTPHeader_ResCode parse_n_HTTPHeader(const char *str, size_t len, struct HTTPHeader *out);
[[nodiscard]]
enum parse_HTTPHeader_ResCode parse_HTTPHeader(const char *str, struct HTTPHeader *out);

void dispose_HTTPHeader(struct HTTPHeader *this);

/* HTTPHeader: Read-only methods */
size_t HTTPHeader_to_string(struct HTTPHeader *this, char *buff, size_t len);


struct HTTPRequesterInfo {
    in_addr_t ip;
    in_port_t port;
};

struct HTTPVersion {
    u8 major;
    u8 minor;
};

/* HTTPVersion: Special functions */
// Awaited format: "HTTP/<major: 1-255>[.<minor: 0-255>]"
[[nodiscard]] bool parse_n_HTTPVersion(const char *str, size_t len, struct HTTPVersion *out);
[[nodiscard]] bool parse_HTTPVersion(const char *str, struct HTTPVersion *out);

/* HTTPVersion: Read-only methods */
// Max len of HTTPVersion string
#define HTTP_VERSION_STR_LEN sizeof("HTTP/255.255")
// Call snprintf with prepared params
size_t HTTPVersion_to_string(struct HTTPVersion this, char *out, size_t out_len);

/* HTTPMethod_t */
typedef const char * HTTPMethod_t;

/* HTTPRequest */
#define HTTP_REQUEST_REPR_FMT REPR_FMT(struct HTTPRequest)

// DISPOSABLE
struct HTTPRequest {
    // double const needs to compatibility with in-memory method-strings
    // any can be const, but i too lazy for mark all fields const everytime
    // (i can mark a struct as const, but it is not a point, okay?)
    HTTPMethod_t method; // allocated
    // Defined as URI by HTTP protocol.
    // URI: Uniform Resource Identifier - URL-s + URN-s
    // URL: Uniform Resource Locator.
    //      Format: <scheme>://[userinfo@]<host>[:port]/[/path][?query]
    //      Examples:
    //      - https://john.doe:pass123@api.example.com/items/10/?param1=value1&...
    //      - tel:+1234567890
    //      - file:///etc/hosts
    //      - postgresql://user:password@host:port/dbname?param1=value1&...
    // URN: Uniform Resource Name. Is not commonly used. Used for persistent
    //      links to some stuff. Format: urn:<namespace>:<number or UUID>.
    //      Example: urn:isbn:9780141036144 - 1984 G. Orwell book. Can be
    //      resolved to actual URL by resolvers. For example, 1984 URN can be
    //      opened by https://isbnsearch.org/isbn/9780141036144 URL. But main
    //      idea - URN always identifier the same resource, concrete URL can
    //      changes, URN - never. No limits for URN resolvers count. Commonly
    //      used for links to science articles ("doi" namespace).
    const char *uri; // allocated
    struct HTTPVersion http_version;
    ARRAY(const struct HTTPHeader, headers); // allocated
    ARRAY(const byte, body); // allocated
    struct HTTPRequesterInfo requester;
};

void dispose_HTTPRequest(struct HTTPRequest *this);

/* reading request from socket */
struct read_http_request_SuccessData {
    struct HTTPRequest request;
    size_t bytes_read_total;
};

struct read_http_request_IOReadErrorData {
    int error_code;
};

union read_http_request_ReturnValue {
    struct read_http_request_SuccessData Success;
    struct read_http_request_IOReadErrorData IOReadError;
};

enum read_http_request_ResCode {
    read_http_request_Success,
    // read return 0 (EOF)
    read_http_request_ClientCloseConnection,
    // read return -1
    read_http_request_IOReadError,
    // Request HTTP version is not supported
    read_http_request_HTTPVersionNotSupported,
    // any alloc return NULL
    read_http_request_MemoryOverflow,
    // Request violate HTTP protocol
    read_http_request_RequestInvalid,
    // Request too big
    read_http_request_RequestTooBig,
    // Request URI too long
    read_http_request_RequestURITooLong,
    // Content Length is not a number
    read_http_request_ContentLengthIsNotANumber,
};
DEF_RESULT(read_http_request, union read_http_request_ReturnValue);

[[nodiscard]]
struct read_http_request_Result read_http_request(
    int client_fd,
    const struct HTTPRequesterInfo *client_info
);


/* HTTPResponse */
struct HTTPResponse {
    struct HTTPVersion http_version;
    enum HTTPResponseCode code;
    ARRAY(const struct HTTPHeader, headers);
    ARRAY(const byte, body);
};

#include <errno.h>
#include <unistd.h>
#include "./http.h"

#define READ_REQUEST_BUFFER_SIZE 4 * KB

const char *HTTPResponseCode_to_reason_phrase(enum HTTPResponseCode self) {
    switch (self) {
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

int HTTPVersion_to_string(struct HTTPVersion self, char *out, size_t out_len) {
    if (self.minor == 0)
        return snprintf(out, out_len, "HTTP/%u", self.major);
    return snprintf(out, out_len, "HTTP/%u.%u", self.major, self.minor);
}

enum _ReadRequestState {
    READ_HEAD,
    READ_HEADERS,
    READ_BODY
};

struct read_http_request_Result read_http_request(
    const int client_fd,
    const struct sockaddr_in *client_addr,
    struct Vec *objects_to_free
) {
    size_t bytes_read_total = 0;
    struct HTTPRequest request = {
        .requester = {
            .ip = client_addr->sin_addr.s_addr,
            .port = client_addr->sin_port
        },
    };

    enum _ReadRequestState state = READ_HEAD;

    // how much bytes need to skip for get access to first body byte
    // in bytes with "\r\n\r\n";
    // 0 == is not initted;
    size_t head_part_size = 0;

    // 1. Receive and parse head (method, uri, check http version)
    // 2. Get Content-Length from then
    // 3. Receive body by CL
    struct Vec headers = new_Vec(sizeof(struct HTTPHeader));
    struct Vec buffer = new_Vec(1);
    while(true) {
        ssize_t bytes_read;
        char buff[READ_REQUEST_BUFFER_SIZE] = {0};

        bytes_read = read(client_fd, buff, sizeof(buff));
        switch (bytes_read) {
            case -1: return EXTERR(read_http_request, IOReadError, {.error_code = errno});
            case 0:  return ERR(read_http_request, ClientCloseConnection);
        }

        bytes_read_total += bytes_read;
        {
            enum Vec_push_ResCode res_code = Vec_push_several(&buffer, &buff, bytes_read);
            if (res_code == Vec_push_MemoryOverflow) {
                return ERR(read_http_request, MemoryOverflow);
            }
            assert(res_code == Vec_push_Success);
        }
    }

    union read_http_request_ReturnValue value = {
        .Success = (struct read_http_request_SuccessReturnValue){
            .request = request,
            .bytes_read_total = bytes_read_total,
        }
    };
    return OK(read_http_request, value);
}

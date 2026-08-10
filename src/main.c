#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#include "src/collections.h"
#include "src/logging.h"
#include "src/errors.h"
#include "src/common.h"


#define PORT 8080
#define CONN_QUEUE_SZ 10
#define REQ_READ_BUFF_SIZE 4 * KB
#define RESP_BUFF_SZ  4 * KB


struct RunParams {
    ushort port;
};

int main(int argc, char *argv[]) {
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        // network await numbers in Big-Endian, x86 works with Little-Endian
        // hton -> Host to Network
        .sin_port = htons(PORT)
    };

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("server socket creation failed\n");
        return 1;
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("binding socket to address failed\n");
        return 1;
    }

    if (listen(server_fd, CONN_QUEUE_SZ) == -1) {
        perror("listening enable failed\n");
        return 1;
    }

    while (true) {
        // `<Item = void *>`
        struct Vec objects_to_free = new_Vec(sizeof(void *));

        struct sockaddr_in client_addr;
        int client_fd;
        {
            size_t addr_size = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        }

        struct HTTPResponse response;
        struct HTTPRequest request;
        {
            bool need_continue = false;
            struct read_http_request_Result res = read_http_request(client_fd, &client_addr, &objects_to_free);
            switch (res.code) {
                case read_http_request_ClientCloseConnection:
                    log(L_INFO, "Client close connection before response was read");
                    close(client_fd);
                    need_continue = true;
                    break;
                case read_http_request_IOReadError:
                    log(L_ERROR, "read(%s) return error %d", nameof(client_fd), res.value.IOReadError.error_code);
                    // TODO: return response with 500 error
                    close(client_fd);
                    need_continue = true;
                    break;
                case read_http_request_HTTPVersionNotSupported:
                    log(L_WARNING, "Received request HTTP version is not supported");
                    close(client_fd);
                    need_continue = true;
                    break;
            }

            // Max len of ip as str is: 15 + null-terminator
            char ip_as_str[16] = {0};
            inet_ntop(AF_INET, &client_addr.sin_addr, ip_as_str, sizeof(ip_as_str));
            log(L_INFO, "%s %s %s from %s:%d (%d bytes)",
                request.method,
                request.uri,
                res.value.Success.bytes_read_total,
                ip_as_str,
                ntohs(client_addr.sin_port) // Network to Host (Big Endian -> Little Endian)
            );

            if (need_continue) {
                cleanup_allocated_memory(&objects_to_free);
                continue;
            }
        }

        cleanup_allocated_memory(&objects_to_free);
        // OTF destructed now
        // In Rust this get ownership on this Vec and we cant get access to them here
    }

    return 0;
}

// Also destruct given Vec
// Vec: `<Item = void *>`
void cleanup_allocated_memory(struct Vec *objects_to_free) {
    for (size_t i = 0; i < get_Vec_count(&objects_to_free); i++) {
        void *item;
        {
            struct Vec_get_Result res = Vec_get(&objects_to_free, i);
            assert(res.code == Vec_get_Success);
            item = res.value;
        }
        free(item);
    }
    destruct_Vec(&objects_to_free);
}


struct HTTPHeader {
    const char *name;
    const char *value;
};

struct HTTPRequesterInfo {
    in_addr_t ip;
    in_port_t port;
};

struct HTTPRequest {
    const char *method;
    const char *uri; // Uri is Url-s + Urn-s, Urn commonly not used (Urn - Unique Resource Number, like UUID)
    const ARRAY(struct HTTPHeader, headers);
    const ARRAY(void, body);
    const struct HTTPRequesterInfo requester;
};

struct HTTPResponse {
    byte code;
    const ARRAY(struct HTTPHeader, headers);
    const ARRAY(void, body);
};

struct read_http_request_SuccessReturnValue {
    struct HTTPRequest request;
    size_t bytes_read_total;
};
union read_http_request_ReturnValue {
    struct read_http_request_SuccessReturnValue Success;
    struct IOReadErrorData IOReadError;
};

DEF_RESULT(read_http_request, union read_http_request_ReturnValue);
enum read_http_request_ResCode {
    read_http_request_Success,
    // read return 0 (EOF)
    read_http_request_ClientCloseConnection,
    // read return -1
    read_http_request_IOReadError,
    // Request valid by HTTP protocol, but not contain Client-Length header
    read_http_request_RequestNotContainsContentLength,
    // Request HTTP version is not supported
    read_http_request_HTTPVersionNotSupported,
    // any alloc return NULL
    read_http_request_MemoryOverflow,
    // Request violate HTTP protocol
    read_http_request_RequestInvalid,
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

    // how much bytes need to skip for get access to first body byte
    // in bytes with "\r\n\r\n";
    // 0 == is not initted;
    size_t head_part_size = 0;

    // 1. Receive and parse head (method, uri, check http version)
    // 2. Get Content-Length from then
    // 3. Receive body by CL
    struct Vec headers = new_Vec(sizeof(struct HTTPHeader));
    byte __vec_buff[REQ_READ_BUFF_SIZE];
    struct Vec buffer = new_Vec_with_buffer(1, __vec_buff, REQ_READ_BUFF_SIZE, false);
    while(true) {
        ssize_t bytes_read;
        char buff[REQ_READ_BUFF_SIZE] = {0};

        bytes_read = read(client_fd, buff, REQ_READ_BUFF_SIZE);
        switch (bytes_read) {
            case -1: return EXTERR(read_http_request, IOReadError, {.error_code = errno});
            case 0:  return ERR(read_http_request, ClientCloseConnection);
        }

        bytes_read_total += bytes_read;
        Vec_push_several(&buffer, buff, bytes_read);
    }

    union read_http_request_ReturnValue value = {
        .Success = (struct read_http_request_SuccessReturnValue){
            .request = request,
            .bytes_read_total = bytes_read_total,
        }
    };
    return OK(read_http_request, value);
}

struct HTTPResponse get_response(const struct HTTPRequest* request) {
    ;
}

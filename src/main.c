#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "src/collections.h"
#include "src/logging.h"
#include "src/common.h"

#define PORT 8080
#define CONN_QUEUE_SZ 10
#define REQ_BUFF_SIZE 4 * KB
#define RESP_BUFF_SZ  4 * KB


int main() {
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
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
        struct sockaddr_in client_addr;
        int client_fd;
        {
            size_t addr_size = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        }

        struct HTTPRequest request;
        {
            struct read_http_request_Result res = read_http_request(client_fd, &client_addr);
            switch (res.code) {
                case read_http_request_Success:
                    break;
                case read_http_request_ClientCloseConnection:
                    log(L_WARNING, "Receive empty request");
                    // NOTE: maybe return response with an error?
                    close(client_fd);
                    continue;
                case read_http_request_IOReadError:
                    log(L_ERROR, "read %s return error", nameof(client_fd));
                    // NOTE: maybe return response with an error?
                    close(client_fd);
                    continue;
                default:
                    break;
            }
        }

        printf("Receive %d bytes of request from %s:%d",
            0,
            inet_ntoa(client_addr.sin_addr),
            ntons(client_addr.sin_port)
        );
    }

    return 0;
}



struct HTTPHeader {
    char *name;
    char *value;
};

struct HTTPRequesterInfo {
    in_addr_t ip;
    in_port_t port;
};

struct HTTPRequest {
    char *method;
    char *url;
    const ARRAY(struct HTTPHeader, headers);
    const ARRAY(void, body);
    struct HTTPRequesterInfo requester;
};

struct HTTPResponse {
    byte code;
    const ARRAY(struct HTTPHeader, headers);
    const ARRAY(void, body);
};


struct read_http_request_ReturnValue {
    struct HTTPRequest request;
    struct Vec objects_to_free;
    size_t bytes_read_total;
};

DEF_RESULT(read_http_request, struct read_http_request_ReturnValue);
enum read_http_request_ResCode {
    read_http_request_Success,
    // read return 0 (EOF)
    read_http_request_ClientCloseConnection,
    // read return -1
    read_http_request_IOReadError,
    // Request not contain Client-Length header
    read_http_request_RequestWithoutContentLength,
    // Request HTTP version is not supported
    read_http_request_HTTPVersionNotSupported,
    // alloc return NULL
    read_http_request_MemoryOverflow,
};

struct read_http_request_Result read_http_request(const int client_fd, const struct sockaddr_in *client_addr) {
    struct Vec objects_to_free = new_Vec(sizeof(void *));
    size_t bytes_read_total = 0;
    struct HTTPRequest request;
    // WARNING: ip в request должен не быть строкой, я дурак :(
    // А для thread-safe преобразования есть ntop
    {
        // 1 for null-terminator
        ssize_t ip_str_len = 1 + strlen(inet_ntoa(client_addr->sin_addr));
        char *ip_str = malloc(ip_str_len);
        if (ip_str == NULL) {
            return ERR(read_http_request, read_http_request_MemoryOverflow);
        }
        Vec_push(&objects_to_free, ip_str);

        // not use previous result because inet_ntoa not thread-safe
        memcpy(ip_str, inet_ntoa(client_addr->sin_addr), ip_str_len);
    }

    // how much bytes need to skip for get access to first body byte
    // in bytes with "\r\n\r\n";
    // 0 == is not initted;
    size_t head_part_size = 0;

    // 1. Receive and parse head (method, url, check http version)
    // 2. Get Content-Length from then
    // 3. Receive body by CL
    struct Vec headers = new_Vec(sizeof(struct HTTPHeader));
    while(true) {
        ssize_t bytes_read;
        char buff[REQ_BUFF_SIZE] = {0};

        bytes_read = read(client_fd, buff, REQ_BUFF_SIZE);
        switch (bytes_read) {
            case -1: return ERR(read_http_request, read_http_request_IOReadError);
            case 0:  return ERR(read_http_request, read_http_request_ClientCloseConnection);
        }

        bytes_read_total += bytes_read;

        // Parsing
        if (head_part_size == 0) { // Head part is not parsed already

        }
    }

    struct read_http_request_ReturnValue v = (struct read_http_request_ReturnValue){
        .request = request,
        .objects_to_free = objects_to_free,
        .bytes_read_total = bytes_read_total,
    };
    return OK(read_http_request, v);
}

struct HTTPResponse get_response(const struct HTTPRequest* request) {
    ;
}

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "src/collections.h"
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
        struct sockaddr_in req_addr;
        int client_fd;
        {
            size_t addr_size = sizeof(req_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&req_addr, &addr_size);
        }

        printf("Receive %d bytes of request from %s:%d",
            0,
            inet_ntoa(req_addr.sin_addr),
            ntons(req_addr.sin_port)
        );

        /*
        struct HTTPRequest request = {
            .requester = {
                .ip = req_addr.sin_addr.s_addr,
                .port = htons(req_addr.sin_port),
            },
            .
        };
        */
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
    ARRAY(struct HTTPHeader, headers);
    ARRAY(void, body);
    struct HTTPRequesterInfo requester;
};

struct HTTPResponse {
    ARRAY(struct HTTPHeader, headers);
    ARRAY(void, body);
};


struct read_http_request_ReturnValue {
    struct HTTPRequest request;
    struct Vec objects_to_free;
    size_t bytes_read_total;
};

DEF_RESULT(read_http_request, struct read_http_request_ReturnValue);
enum read_http_request_ResCode {
    read_http_request_Success,
    read_http_request_EmptyRequest,
    read_http_request_IOReadError,
    read_http_request_RequestHaveNotContentLength,
};

struct read_http_request_Result read_http_request(const int client_fd) {
    struct Vec objects_to_free = new_Vec(sizeof(void *));
    struct HTTPRequest request;
    size_t bytes_read_total = 0;

    struct Vec headers = new_Vec(sizeof(struct HTTPHeader));
    struct Vec buffer = new_Vec(1);
    while(true) {
        // 1. Receive and parse headers
        // 2. Get Content-Length from then
        // 3. Receive
        ssize_t bytes_read = 0;
        char buff[REQ_BUFF_SIZE] = {0};
        bytes_read = read(client_fd, buff, REQ_BUFF_SIZE);
        bytes_read_total += bytes_read;

        if (bytes_read == -1) {
            perror("request read failed\n");
            close(client_fd);
            return ERR(read_http_request, read_http_request_IOReadError);
        }

        Vec_push_several(&buffer, buff, bytes_read);
    }

    if (bytes_read == 0) {
        perror("receive empty request");
        close(client_fd);
        return ERR(read_http_request, read_http_request_EmptyRequest);
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

DEF_RESULT(parse_num, long);
enum parse_num_ResCode {
    parse_num_Success,
    parse_num_IsNotANumber,
};

// variant for not null-terminated str
struct parse_num_Result sparse_num(const char *str, size_t str_len) {
    long num = 0;
    bool is_neg = false;
    char *curr_char = str;

    if (*curr_char == '-') {
        is_neg = true;
        curr_char++;
    }

    while (curr_char - str < str_len) {
        if (*curr_char < '0' || *curr_char > '9') {
            return ERR(parse_num, parse_num_IsNotANumber);
        }

        num += (*curr_char - '0') * (curr_char - str - is_neg);
        curr_char++;
    }

    num *= is_neg;
    return OK(parse_num, num);
}

struct parse_num_Result parse_num(const char *str) {
    size_t str_len = 0;
    char *current_char = str;
    while (*current_char != '\0') {
        str_len++;
        current_char++;
    }

    return sparse_num(str, str_len);
}

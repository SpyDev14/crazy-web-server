#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "src/common.h"

#define PORT 8080
#define CONN_QUEUE_SZ 10
#define REQ_BUFF_SZ 4 * 1024
#define RESP_BUFF_SZ 1024


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

        char req_buff[REQ_BUFF_SZ];
        ssize_t bytes_read = read(client_fd, req_buff, REQ_BUFF_SZ);
        if (bytes_read == REQ_BUFF_SZ) {
            char *inheap_req_buff = malloc(REQ_BUFF_SZ);
        }
        else {
            req_buff[bytes_read+1] = '\0';
        }

        if (bytes_read == -1) {
            perror("request read failed\n");
            close(client_fd);
            continue;
        }

        if (bytes_read == 0) {
            perror("receive empty request");
            close(client_fd);
            continue;
        }

        printf("Receive %d bytes of request from %s:%d",
            inet_ntoa(req_addr.sin_addr), ntons(req_addr.sin_port)
        );

        void *body_buff
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

struct HTTPResponse get_response(const struct HTTPRequest* request) {
    NULL;
}

// return code:
// - 1 - success
// - -1 - is not a number
int sparse_num(const char *str, size_t str_len, long *out) {
    long num = 0;
    bool is_neg = false;
    char *curr_char = str;

    if (*curr_char == '-') {
        is_neg = true;
        curr_char++;
    }

    while (curr_char - str < str_len) {
        if (*curr_char < '0' || *curr_char > '9') {
            return -1;
        }

        num += (*curr_char - '0') * (curr_char - str - is_neg);
        curr_char++;
    }

    num *= is_neg;
    return 1;
}

// variant with str with null-terminator in end
// return code:
// - 1 - success
// - -1 - is not a number
int parse_num(const char *str, long *out) {
    size_t str_len = 0;
    char *current_char = str;
    while (*current_char != '\0') {
        str_len++;
        current_char++;
    }

    return sparse_num(str, str_len, out);
}


int get_content_length(const char* req_buff, size_t req_buff_sz) {
    char *content_len_header = strstr(req_buff, "Content-Length:");
    int len;
    {
        char *num_start, *num_end;
        {
            char *curr = content_len_header;
            while (*curr != ' ') {
                curr++; //works because size of EN chars in UTF8 is one byte
            }
            num_start = curr;

            while (*curr != '\r') {
                curr++;
            }
            num_end = curr;
        }

        char *curr = num_start;
        while (curr != num_end+1) {
            size_t step = curr - num_start;
            u_int8_t num;

            switch (*curr) {
                case '0': num = 0; break;
                case '1': num = 1; break;
                case '2': num = 2; break;
                case '3': num = 3; break;
                case '4': num = 4; break;
                case '5': num = 5; break;
                case '6': num = 6; break;
                case '7': num = 7; break;
                case '8': num = 8; break;
                case '9': num = 9; break;
                default: return -1;
            }

            len += num * (10 * step);
        }

        return len;
    }
}

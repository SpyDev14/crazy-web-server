#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#include "src/collections/vec.h"
#include "src/logging.h"
#include "src/common.h"
#include "src/http.h"
#include "src/config.h"
#include "src/freeable.h"


const struct Config DEFAULT_CONF = {
    .CONNECTION_QUEUE_SIZE = 10,
    .PORT = 8080
};

struct Config get_config(int argc, char *argv[]) {
    return DEFAULT_CONF;

    // This methods is not implemented
    struct Config conf_from_cmd_line = new_Config_from_command_line(argc, argv);
    struct Config conf_from_env = new_Config_from_env("CWS_");

    struct Config conf = Config_merge(&DEFAULT_CONF, &conf_from_env);
    return Config_merge(&conf, &conf_from_cmd_line);
}

int main(int argc, char *argv[]) {
    struct Config conf = get_config(argc, argv);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        // network await numbers in Big-Endian, x86 works with Little-Endian
        // hton -> Host to Network
        .sin_port = htons(conf.PORT)
    };

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        log(L_FATAL, L_ARGS, "server socket creation failed\n");
        return 1;
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        log(L_FATAL, L_ARGS, "binding socket to address failed\n");
        return 1;
    }

    if (listen(server_fd, conf.CONNECTION_QUEUE_SIZE) == -1) {
        log(L_FATAL, L_ARGS, "listening enable failed\n");
        return 1;
    }

    FreeablesVec objects_to_free = new_Vec(sizeof(struct Freeable));
    while (true) {
        struct sockaddr_in client_addr;
        int client_fd;
        {
            socklen_t addr_sz = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_sz);
        }

        struct HTTPResponse response;
        struct HTTPRequest request;
        {
            bool need_continue = false;
            struct read_http_request_Result res = read_http_request(client_fd, &client_addr, &objects_to_free);
            if (res.code != read_http_request_Success)
                close(client_fd);
            switch (res.code) {
                // TODO: return HTTP responses with an error
                case read_http_request_Success:
                    log(L_DEBUG, L_ARGS, "Request was successful read");
                    break;
                case read_http_request_ClientCloseConnection:
                    log(L_INFO, L_ARGS, "Client close connection before response has been read");
                    need_continue = true;
                    break;
                case read_http_request_IOReadError:
                    log(L_ERROR, L_ARGS, "read(%s) return error %d",
                        nameof(client_fd), res.value.IOReadError.error_code);
                    need_continue = true;
                    break;
                case read_http_request_HTTPVersionNotSupported:
                    log(L_WARNING, L_ARGS, "Received request HTTP version is not supported");
                    need_continue = true;
                    break;
                case read_http_request_RequestInvalid:
                    log(L_WARNING, L_ARGS, "Received request is not valid HTTP");
                    need_continue = true;
                    break;
                case read_http_request_MemoryOverflow:
                    // Return 500 here
                    log(L_ERROR, L_ARGS, "Memory overflow");
                    need_continue = true;
                    break;
            }
            assert(res.code == read_http_request_Success);

            char ip_as_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, ip_as_str, sizeof(ip_as_str));
            char http_version_as_str[HTTP_VERSION_STR_LEN];
            HTTPVersion_to_string(
                request.http_version,
                http_version_as_str,
                sizeof(http_version_as_str)
            );
            // <METHOD> <URI> <VERSION> from <IP>:<PORT> (<BYTES> bytes)
            log(L_INFO, L_ARGS, "%s %s %s from %s:%d (%zu bytes)",
                request.method, request.uri, http_version_as_str,
                res.value.Success.bytes_read_total, ip_as_str,
                // Network to Host (Big Endian -> Little Endian)
                ntohs(client_addr.sin_port)
            );

            if (need_continue) {
                cleanup_allocated_memory(&objects_to_free);
                continue;
            }
        }

        cleanup_allocated_memory(&objects_to_free);
    }

    return 0;
}

// Also clean given Vec
// NO THREAD SAFE
void cleanup_allocated_memory(FreeablesVec *objects_to_free) {
    for (size_t i = 0; i < get_Vec_count(objects_to_free); i++) {
        struct Vec_get_Result res = Vec_get(objects_to_free, i);
        assert(res.code == Vec_get_Success);
        struct Freeable *item = res.value;

        Freeable_free(item);
    }

    Vec_clear(objects_to_free);
}

struct HTTPResponse get_response(const struct HTTPRequest* request) {
    return (struct HTTPResponse) {
        .code = 200,
    };
}

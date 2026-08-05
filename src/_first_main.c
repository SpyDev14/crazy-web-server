#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define CONNECTIONS_QUEUE_LEN 10


int disable_terminal_echo() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    return 0;
}

int main() {
    disable_terminal_echo();

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed\n");
        exit(1);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed\n");
        exit(1);
    }

    if (listen(server_fd, CONNECTIONS_QUEUE_LEN) < 0) {
        perror("Listening failed\n");
        exit(1);
    }

    printf("Server start listen port %d\n", PORT);
    while (1) {
        int addrsize = sizeof(address);
        int client_fd = accept(
            server_fd,
            (struct sockaddr *)&address,
            (socklen_t*)&addrsize
        );

        if (client_fd < 0) {
            perror("Connection accept failed");
            continue;
        }

        char request[BUFFER_SIZE] = {0};
        ssize_t req_len = recv(client_fd, request, BUFFER_SIZE-1, 0);
        printf("Received new request from: %s:%d (%zu bytes)\n",
            inet_ntoa(address.sin_addr),
            ntohs(address.sin_port),
            req_len
        );
        if (req_len <= 0) {
            printf("Request was empty\n");
            close(client_fd);
            continue;
        }

        char resp_body[] = (
            "{\"ok\": true}"
        );

        char response[BUFFER_SIZE] = {0};
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\n"
            "Content-Length: %zu\n"
            "Content-Type: application/json\n"
            "\n"
            "%s\n",
            strlen(resp_body),
            resp_body
        );

        send(client_fd, response, strlen(response), 0);
        close(client_fd);
    }

    close(server_fd);
    printf("Server shutting down\n");
    return 0;
}

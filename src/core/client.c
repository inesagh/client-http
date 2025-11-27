#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "client.h"

int connect_to_server(const char* ip, int port) {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockFd < 0) {
        perror("Socket failed!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr.s_addr);

    if(connect(sockFd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("Connect failed!");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    return sockFd;
}

void close_socket(int fd) {
    if(fd >= 0) {
        close(fd);
    }
}
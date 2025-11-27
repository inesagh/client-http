#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/client.h"
#include "rest/http_client.h"
#include "rest/model/models.h"
#include "util/logger.c"

int handle_get(int, char*[]);
int handle_post(int, char*[]);
int handle_echo(int, char*[]);
void print_valid_requests();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_valid_requests();
        return 1;
    }

    const char* method = argv[1];
    if(strcmp(method, "GET") == 0) return handle_get(argc, argv);
    if(strcmp(method, "POST") == 0) return handle_post(argc, argv);
    if(strcmp(method, "ECHO") == 0) return handle_echo(argc, argv);

    printf("Invalid method! Exiting...\n");
    return 1;
}

int handle_get(int argc, char* argv[]) {
    if (argc != 5) {
        print_valid_requests();
        return 1;
    }

    const char *ip = argv[2];
    int port = atoi(argv[3]);
    const char *resource = argv[4];

    int fd = connect_to_server(ip, port);

    ResponseModel res;
    memset(&res, 0, sizeof(ResponseModel));

    if (http_get(fd, resource, &res) == -1) {
        log_error("GET request failed");
        close_socket(fd);
        return -1;
    }

    print_response(&res);
    free_response(&res);
    close_socket(fd);
    return 0;
}

int handle_post(int argc, char* argv[]) {
    if (argc != 6) {
        print_valid_requests();
        return 1;
    }

    const char *ip = argv[2];
    int port = atoi(argv[3]);
    const char *resource = argv[4];
    const char *data = argv[5];

    int fd = connect_to_server(ip, port);

    ResponseModel res;
    memset(&res, 0, sizeof(ResponseModel));

    if (http_post(fd, resource, data, &res) == -1) {
        log_error("POST request failed");
        close_socket(fd);
        return -1;
    }

    print_response(&res);
    free_response(&res);
    close_socket(fd);
    return 0;
}

int handle_echo(int argc, char* argv[]) {
    if (argc != 5) {
        print_valid_requests();
        return 1;
    }

    const char *ip = argv[2];
    int port = atoi(argv[3]);
    const char *data = argv[4];

    int fd = connect_to_server(ip, port);

    ResponseModel res;
    memset(&res, 0, sizeof(ResponseModel));

    if (http_echo(fd, data, &res) == -1) {
        log_error("ECHO request failed");
        close_socket(fd);
        return -1;
    }

    print_response(&res);
    free_response(&res);
    close_socket(fd);
    return 0;
}

void print_valid_requests() {
    printf("\nUsage:\n");
    printf("  client GET  <ip> <port> <resource>\n");
    printf("  client POST <ip> <port> <resource> <data>\n");
    printf("  client ECHO <ip> <port> <data>\n\n");
}
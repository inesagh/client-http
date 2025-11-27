#include "../util/files.h"
#include "../util/logger.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_client.h"

int http_send_request(int sockFd, RequestModel* request, ResponseModel* response) {
    print_request(request);

    if (send_request(sockFd, request) < 0) {
        return EXIT_FAILURE;
    }

    return receive_response(sockFd, response);
}

int http_get(int sockFd, const char* resource, ResponseModel* response) {
    RequestModel request;
    init_request(&request);

    request.line.method = GET;
    strcpy(request.line.resource, resource);

    strcpy(request.headers[0].name, "Body-Size");
    strcpy(request.headers[0].value, "0");
    request.headerCount = 1;

    int resp = http_send_request(sockFd, &request, response);
    free_request(&request);
    return resp;
}

int http_post(int sockFd, const char* resource, const char* data, ResponseModel* response) {
    RequestModel request;
    init_request(&request);

    request.line.method = POST;
    strcpy(request.line.resource, resource);

    size_t bodySize = strlen(data);
    sprintf(request.headers[0].value, "%zu", bodySize);
    strcpy(request.headers[0].name, "Body-Size");
    request.headerCount = 1;

    request.body = strdup(data);
    request.bodySize = bodySize;

    int resp = http_send_request(sockFd, &request, response);
    free_request(&request);
    return resp;
}

int http_echo(int sockFd, const char* data, ResponseModel* response) {
    RequestModel request;
    init_request(&request);

    request.line.method = ECHO;
    strcpy(request.line.resource, "/echo");

    size_t bodySize = strlen(data);
    sprintf(request.headers[0].value, "%zu", bodySize);
    strcpy(request.headers[0].name, "Body-Size");
    request.headerCount = 1;

    request.body = strdup(data);
    request.bodySize = bodySize;

    int resp = http_send_request(sockFd, &request, response);
    free_request(&request);
    return resp;
}

void init_request(RequestModel* req) {
    memset(req, 0, sizeof(RequestModel));
    req->body = NULL;
    req->bodySize = 0;
    strcpy(req->line.version, "CHLP/1.0");
}

void free_request(RequestModel* req) {
    if (req->body != NULL) {
        free(req->body);
    }
}

void free_response(ResponseModel* res) {
    if (res->body != NULL) {
        free(res->body);
    }
}

void print_response(ResponseModel *res) {
    printf("\n--------------------\n");
    printf("RESPONSE:\n");
    printf("Status: %s %d %s\n",
           res->status.version,
           res->status.statusCode,
           res->status.statusMsg);

    printf("\nHeaders:\n");
    for (size_t i = 0; i < res->headerCount; i++) {
        printf("  %s: %s\n",
               res->headers[i].name,
               res->headers[i].value);
    }

    if (res->bodySize > 0) {
        printf("\nBody (%zu bytes):\n", res->bodySize);
        fwrite(res->body, 1, res->bodySize, stdout);
        printf("\n");
    }

    printf("--------------------\n\n");
}

void print_request(RequestModel *req) {
    printf("\n--------------------\n");
    printf("REQUEST:\n");

    const char *methodStr = NULL;
    switch (req->line.method) {
        case GET: 
            methodStr = "GET"; 
            break;
        case POST:  
            methodStr = "POST"; 
            break;
        case ECHO:   
            methodStr = "ECHO"; 
            break;
        default:     
            methodStr = "UNKNOWN"; 
            break;
    }

    printf("Request-Line: %s %s %s\n",
           methodStr,
           req->line.resource,
           req->line.version);

    printf("\nHeaders:\n");
    for (size_t i = 0; i < req->headerCount; i++) {
        printf("  %s: %s\n",
               req->headers[i].name,
               req->headers[i].value);
    }

    if (req->bodySize > 0 && req->body != NULL) {
        printf("\nBody (%zu bytes):\n", req->bodySize);
        fwrite(req->body, 1, req->bodySize, stdout);
        printf("\n");
    }

    printf("--------------------\n\n");
}


//methods to send and receive data
int send_request(int sockFd, RequestModel* request) {
    char buffer[2048];
    int offset = 0;

    //Request line
    offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                       "%s %s %s\r\n",
                       method_to_str(request->line.method),
                       request->line.resource,
                       request->line.version);

    //Headers
    for (size_t i = 0; i < request->headerCount; i++) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                           "%s: %s\r\n",
                           request->headers[i].name,
                           request->headers[i].value);
    }

    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "\r\n");

    if (write_into(sockFd, buffer, offset) < 0) {
        return EXIT_FAILURE;
    }

    //Body
    if (request->bodySize > 0) {
        if (write_into(sockFd, request->body, request->bodySize) < 0) {
            return EXIT_FAILURE;
        }
    }

    return 0;
}

int receive_response(int sockFd, ResponseModel* response) {
    char line[4096];
    response->headerCount = 0;
    response->bodySize = 0;
    response->body = NULL;

    if (read_line(sockFd, line, sizeof(line)) <= 0) {
        return EXIT_FAILURE;
    }

    trim_newline(line);

    if (parse_status_line(line, response) < 0) {
        return EXIT_FAILURE;
    }

    //Headers
    size_t bodySize = 0;
    while(1) {
        ssize_t n = read_line(sockFd, line, sizeof(line));
        if (n <= 0) {
            return EXIT_FAILURE;
        }

        trim_newline(line);
        if (line[0] == '\0') break;

        Header *h = &response->headers[response->headerCount];
        if (parse_header(line, h) == 0) {
            if (strcasecmp(h->name, "Body-Size") == 0) {
                bodySize = atoi(h->value);
            }
            response->headerCount++;
        }
    }

    if (bodySize > 0) {
        response->body = malloc(bodySize + 1);
        if (!response->body) return EXIT_FAILURE;

        if (read_from(sockFd, response->body, bodySize) < 0)
            return EXIT_FAILURE;

        response->body[bodySize] = '\0';
        response->bodySize = bodySize;
    }
    return 0;
}

void trim_newline(char* line) {
    size_t len = strlen(line);
    while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
        line[len-1] = '\0';
        len--;
    }
}
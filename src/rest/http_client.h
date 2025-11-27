#ifndef HTTP_CLIENT
#define HTTP_CLIENT

#include "model/models.h"

int http_send_request(int, RequestModel*, ResponseModel*);
int http_get(int, const char*, ResponseModel*);
int http_post(int, const char*, const char*, ResponseModel*);
int http_echo(int, const char*, ResponseModel*);

void init_request(RequestModel*);
void free_request(RequestModel*);
void free_response(ResponseModel*);

void print_request(RequestModel*);
void print_response(ResponseModel*);

int send_request(int, RequestModel*);
int receive_response(int, ResponseModel*);

void trim_newline(char*);

#endif

#ifndef PARSER
#define PARSER

#include "model/models.h"

const char* method_to_str(HttpMethod);
int parse_status_line(char*, ResponseModel*);
int parse_header(char*, Header*);

#endif

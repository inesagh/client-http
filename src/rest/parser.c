#include "parser.h"
#include <stdio.h>
#include <string.h>

const char* method_to_str(HttpMethod m) {
    switch (m) {
        case GET:  return "GET";
        case POST: return "POST";
        case ECHO: return "ECHO";
        default:   return "UNKNOWN";
    }
}

int parse_status_line(char* line, ResponseModel* res) {
    char version[32];
    char message[128] = {0};
    int code = 0, consumed = 0;

    if (sscanf(line, "%31s %d %n", version, &code, &consumed) < 2) {
        return -1;
    }

    strcpy(res->status.version, version);
    res->status.statusCode = code;

    // The rest is status message
    if ((int)strlen(line) > consumed) {
        strncpy(message, line + consumed, sizeof(message) - 1);
    } else {
        strcpy(message, "OK");
    }

    strcpy(res->status.statusMsg, message);

    return 0;
}

int parse_header(char* line, Header* h) {
    char *colon = strchr(line, ':');
    if (!colon) return -1;

    *colon = 0;
    char *name = line;
    char *value = colon + 1;

    while (*value == ' ') value++;

    strcpy(h->name, name);
    strcpy(h->value, value);

    return 0;
}


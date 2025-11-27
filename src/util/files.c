#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

ssize_t read_line(int fd, char* buffer, size_t maxLength) {
    size_t i = 0;

    while (i < maxLength - 1) {
        char ch;
        ssize_t n = recv(fd, &ch, 1, 0);

        if (n == 0) {           
            if (i == 0) return 0;
            break;
        }

        if (n < 0) {
            return -1;
        }

        buffer[i++] = ch;
        if (ch == '\n') break;  
    }

    buffer[i] = '\0';
    return i;
}

int read_from(int fd, void* buffer, size_t length) {
    size_t total = 0;
    char *p = buffer;

    while (total < length) {
        ssize_t n = recv(fd, p + total, length - total, 0);
        if (n <= 0) return -1; 
        total += n;
    }

    return 0;
}

int write_into(int fd, const void* buffer, size_t length) {
    size_t total = 0;
    const char *p = buffer;

    while (total < length) {
        ssize_t n = send(fd, p + total, length - total, 0);
        if (n <= 0) return -1;
        total += n;
    }

    return 0;
}


# Client HTTP – C Client Application

This client sends **GET**, **POST**, and **ECHO** requests to a custom backend server using a simple http protocol.  
The project is split into logical layers and packaged as a static library for reuse.

---


### Layer Overview

| Layer        | Responsibility                            |
|--------------|-------------------------------------------|
| **core**     | Connect to server, socket lifecycle       |
| **rest**     | Build/send requests, parse responses      |
| **model**    | RequestLine, StatusLine, headers, models  |
| **util**     | Logging, I/O utilities                    |
| **main.c**   | CLI entrypoint                            |

---

## Building 
Note: Make sure you've installed GCC

### 1. Clean previous builds

```bash
rm -f *.o libclient.a
```

### 2. Compile all modules into .o files
```bash
gcc -c src/core/client.c -o client.o \
    -I./src -I./src/core -I./src/rest -I./src/rest/model -I./src/util

gcc -c src/rest/http_client.c -o http.o \
    -I./src -I./src/core -I./src/rest -I./src/rest/model -I./src/util

gcc -c src/rest/parser.c -o parser.o \
    -I./src -I./src/core -I./src/rest -I./src/rest/model -I./src/util

gcc -c src/util/files.c -o files.o \
    -I./src -I./src/core -I./src/rest -I./src/rest/model -I./src/util

gcc -c src/util/logger.c -o logger.o \
    -I./src -I./src/core -I./src/rest -I./src/rest/model -I./src/util
```

### 3. Archive object files into a static library
```bash
ar rcs libclient.a client.o http.o files.o logger.o parser.o
```

### 4. Compile main.c
```bash
gcc -c src/main.c -o main.o \
    -I./src -I./src/core -I./src/rest -I./src/rest/model -I./src/util
```

### 5. Link everything into the final executable
```bash
gcc main.o -L. -lclient -o client
```

### Your executable will appear as:
```bash
./client
```

## Running the Client
### GET request
```bash
./client GET 127.0.0.1 8080 /index.txt
```

### POST request
```bash
./client POST 127.0.0.1 8080 /upload "Hello backend"
```

### ECHO request
```bash
./client ECHO 127.0.0.1 8080 "Ping!"
```

#### Example Output
```bash
----- RESPONSE -----
Status: HTTP/1.0 200 OK

Headers:
  Body-Size: 12

Body (12 bytes):
Hello world!
--------------------
```
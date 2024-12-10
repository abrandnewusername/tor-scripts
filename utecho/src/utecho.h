#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <time.h>
#include <assert.h>

#define BUFFER_SIZE 2048
#define VLEN 10
#define TIMEOUT_SEC 1
#define TIMEOUT_NSEC 0

#define MODE_UDP 0
#define MODE_TCP 1

// #define DEBUG

void udpEcho(int port, int vlen, int buf_size);
// void tcpEcho(int port, char *buf[], int vlen, int buf_size);

static void fail(const char* msg) {
    perror(msg);
    exit(-1);
}

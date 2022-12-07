// Simple TCP echo - receives packets, and returns them.
// Matt Rossouw
// 07/12/2022

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define DEBUG 1
#define BUFFER_SIZE 5000
#define MAX_BACKLOG 50

static void usage() {
    printf("USAGE: tcp_echo [port]\n");
    exit(-1);
}

static void fail(const char* msg) {
    perror(msg);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) usage();

    #ifdef DEBUG
        printf("Beginning echo on port %d\n", atoi(argv[1]));
    #endif

    char buf[BUFFER_SIZE];
    struct sockaddr_in sockAddr;
    socklen_t sockAddrLen;
    ssize_t msgLen;

    // Prepare socket address
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(argv[1]));
    sockAddr.sin_addr.s_addr = INADDR_ANY;

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!sock) fail("Failed to create socket!");

    // Set buffer
    int bufsize = BUFFER_SIZE; // dummy variable to set socket options
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof bufsize);

    // Try bind
    if (bind(sock, (struct sockaddr *)&sockAddr, sizeof sockAddr)) 
        fail("Failed to bind.");

    // Begin listen
    if(listen(sock, MAX_BACKLOG))
        fail("Failed to begin listening.");

    // Server loop
    for (;;) {
        sockAddrLen = sizeof sockAddr;
        
        // Accept next client
        int client = accept(sock, (struct sockaddr *)&sockAddr, &sockAddrLen);
        size_t num_bytes = recv(client, buf, BUFFER_SIZE, 0x0);

        #ifdef DEBUG
            // Echo message content
            printf("msg: %.*s\n", num_bytes, buf);
        #endif

        // Reply
        send(client, buf, num_bytes, 0x0);

        // Terminate connection
        // close(client);
    }



}
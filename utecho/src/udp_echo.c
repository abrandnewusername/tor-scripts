// UDP echo, just Peter's program modified to fit utecho.
// Matt Rossouw
// 07/12/2022

#include "utecho.h"

static void setbufsize(int sock, int size)
{
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof size);
}

void udpEcho(int port, int vlen, int buf_size)
{

    #ifdef DEBUG
        printf("Beginning UDP echo on port %d\n", port);
    #endif

    char bufs[vlen][buf_size];
    struct iovec iovecs[vlen];
    struct mmsghdr msgs[vlen];
    struct sockaddr_in sas[vlen];
    struct timespec timeout;

    struct sockaddr_in sa;
    // socklen_t sa_len;
    // ssize_t msglen;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;
    int sock = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_UDP);
    setbufsize(sock, 2048*256);
    if (bind(sock, (struct sockaddr *)&sa, sizeof sa)) {
        fail("bind failure");
    }

    memset(msgs, 0, sizeof(msgs));
    for (size_t i = 0; i < vlen; i++) {
        iovecs[i].iov_base         = (void *)bufs[i];
        iovecs[i].iov_len          = buf_size;
        msgs[i].msg_hdr.msg_iov    = &iovecs[i];
        msgs[i].msg_hdr.msg_iovlen = 1;
        msgs[i].msg_hdr.msg_name   = &sas[i];
        msgs[i].msg_hdr.msg_namelen   = sizeof sas[i];
    }

    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_nsec = TIMEOUT_NSEC;

    // int counter = 0;
    // int max_rmsgcnt = -1;
    for (;;) { 
        // sa_len = sizeof sa;
        // msglen = recvfrom(sock, bufs[0], buf_size, 0, (struct sockaddr *)&sa, &sa_len);

        // #ifdef DEBUG
        //     printf("msg: %s\n", bufs[0]);
        // #endif

        int rmsgcnt = recvmmsg(sock, msgs, vlen, MSG_WAITFORONE, NULL);
        if (rmsgcnt == -1) {
            fail("recvmmsg failure");
        }
        
        #ifdef DEBUG
            printf("received %d msgs\n", rmsgcnt);
        #endif

        // sendto(sock, bufs[0], msglen, 0, (struct sockaddr *)&sa, sa_len);

        for (int i = 0; i < rmsgcnt; i++) {
        //     #ifdef DEBUG
        //         printf("echo msg %d, len: %u, addr: %lx, addr len: %d\n", i, msgs[i].msg_len, sas[i].sin_addr.s_addr, msgs[i].msg_hdr.msg_namelen);
        //     #endif
            // sendto(sock, bufs[i], msgs[i].msg_len, 0, msgs[i].msg_hdr.msg_name, msgs[i].msg_hdr.msg_namelen);
            // sendmsg(sock, &msgs[i].msg_hdr, 0);

            // ipbench doesn't like the packet length to be wrong
            iovecs[i].iov_len = msgs[i].msg_len;
        }

        int smsgcnt = sendmmsg(sock, msgs, rmsgcnt, 0);
        if (smsgcnt == -1) {
            fail("sendmmsg failure");
        }
        assert(smsgcnt == rmsgcnt);

        for (int i = 0; i < rmsgcnt; i++) {
            iovecs[i].iov_len = buf_size;
        }
        // counter++;
        // if (max_rmsgcnt < rmsgcnt) {
        //     max_rmsgcnt = rmsgcnt;
        // }

        // if (counter == 100) {
        //     printf("max rmsgcnt: %d\n", max_rmsgcnt);
        //     counter = 0;
        //     max_rmsgcnt = -1;
        // }
    }
}
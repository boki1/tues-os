#include <stdio.h>
#include <sys/socket.h>
#include <err.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

struct thread_args
{
    int sfd;
    struct sockaddr *addr;
    size_t addrlen;
};

void *send_thread(void *arg)
{
    struct thread_args *args = arg;
    char *buf = NULL;
    size_t n = 0;
    while (1)
    {
        if (getline(&buf, &n, stdin) < 0)
            err(1, "getline");
        char s[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &((struct sockaddr_in *)args->addr)->sin_addr, s, sizeof s))
            printf("sending to %s\n", s);
        else
            warn("inet_ntop");
        if (sendto(args->sfd, buf, n, 0, args->addr, args->addrlen) < 0)
            err(1, "sendto");
    }
}

void *receive_thread(void *arg)
{
    struct thread_args *args = arg;
    while (1)
    {
        char buf[4];
        ssize_t n = recv(args->sfd, buf, sizeof buf, 0);
        if (n < 0)
            err(1, "recv");
        if (n == 0)
            break;
        if (write(1, buf, n) < 0)
            err(1, "write");
    }
    return NULL;
}

int main()
{
    struct addrinfo *mcast, /* *remote, */ hints = {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_DGRAM,
    };
    int gai_err;
    if ((gai_err = getaddrinfo("224.0.12.34", "8888", &hints, &mcast)) < 0)
        errx(1, "getaddrinfo: %s", gai_strerror(gai_err));

    /* if ((gai_err = getaddrinfo("0.0.0.0", NULL, &hints, &remote)) < 0)
        errx(1, "getaddrinfo: %s", gai_strerror(gai_err)); */

    assert(mcast->ai_socktype == SOCK_DGRAM);
    //assert(remote->ai_socktype == SOCK_DGRAM);

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd < 0)
        err(1, "socket");

    /* if (bind(sfd, remote->ai_addr, remote->ai_addrlen) < 0)
        err(1, "bind"); */

    /* struct group_req mreq = {0};
    mreq.gr_group = *(struct sockaddr_storage *)mcast->ai_addr;
    mreq.gr_interface = 2;
    if (setsockopt(sfd, SOL_IP, MCAST_JOIN_GROUP, &mreq, sizeof mreq) < 0)
        err(1, "setsockopt"); */

    struct ip_mreq mreq = {0};
    mreq.imr_multiaddr = ((struct sockaddr_in *)mcast->ai_addr)->sin_addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof mreq) < 0)
        err(1, "setsockopt");

    pthread_t send_tid, recv_tid;
    struct thread_args send_args = {
            .sfd = sfd,
            .addr = mcast->ai_addr,
            .addrlen = mcast->ai_addrlen,
    };
    if (pthread_create(&send_tid, NULL, send_thread, &send_args) != 0)
        err(1, "pthread_create");

    struct thread_args recv_args = {
            .sfd = sfd,
            //.addr = remote->ai_addr,
            //.addrlen = remote->ai_addrlen,
    };
    if (pthread_create(&recv_tid, NULL, receive_thread, &recv_args) != 0)
        err(1, "pthread_create");

    if (pthread_join(send_tid, NULL) != 0)
        err(1, "pthread_join");

    if (pthread_join(recv_tid, NULL) != 0)
        err(1, "pthread_join");
    if (setsockopt(sfd, SOL_IP, MCAST_LEAVE_GROUP, &mreq, sizeof mreq) < 0)
        err(1, "setsockopt");
}


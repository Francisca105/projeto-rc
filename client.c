#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 500

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    
    hints.ai_socktype = SOCK_DGRAM; 
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            perror("socket");
            continue;
        }

        // For UDP, we don't strictly need to connect, but it can help
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  

        close(sfd);
    }

    if (rp == NULL) {               
        fprintf(stderr, "Could not connect: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           

    for (j = 3; j < argc; j++) {
        len = strlen(argv[j]) + 1;

        if (len + 1 > BUF_SIZE) {
            fprintf(stderr, "Ignoring long message in argument %d\n", j);
            continue;
        }

        if (send(sfd, argv[j], len, 0) != len) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        nread = recv(sfd, buf, BUF_SIZE, 0);
        if (nread == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        printf("Received %zd bytes: %s\n", nread, buf);
    }

    close(sfd);
    exit(EXIT_SUCCESS);
}
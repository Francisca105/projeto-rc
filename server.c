#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUF_SIZE 500

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];
    char server_ip[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    
    hints.ai_socktype = SOCK_DGRAM; 
    hints.ai_flags = AI_PASSIVE;    
    hints.ai_protocol = 0;          

    s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            // Convert IP to string
            void *addr;
            if (rp->ai_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
                addr = &(ipv4->sin_addr);
            } else {
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
                addr = &(ipv6->sin6_addr);
            }
            
            inet_ntop(rp->ai_family, addr, server_ip, sizeof(server_ip));
            printf("Server IP: %s\n", server_ip);
            break;
        }

        close(sfd);
    }

    if (rp == NULL) {               
        fprintf(stderr, "Could not bind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           

    printf("Server listening on port %s\n", argv[1]);

    for (;;) {
        peer_addr_len = sizeof(struct sockaddr_storage);
        nread = recvfrom(
            sfd, buf, BUF_SIZE, 0, 
            (struct sockaddr *) &peer_addr, &peer_addr_len
        );
        
        if (nread == -1) {
            perror("recvfrom");
            continue;
        }

        buf[nread] = '\0';  // Null-terminate received data

        char host[NI_MAXHOST], service[NI_MAXSERV];
        s = getnameinfo(
            (struct sockaddr *) &peer_addr,
            peer_addr_len,
            host,
            NI_MAXHOST,
            service,
            NI_MAXSERV,
            NI_NUMERICSERV
        );
        
        if (s == 0)
            printf("Received %zd bytes from %s:%s: %s\n", nread, host, service, buf);
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

        if (sendto(
            sfd, buf, nread, 0,
            (struct sockaddr *) &peer_addr, peer_addr_len
        ) != nread) {
            perror("sendto");
        }
    }
}
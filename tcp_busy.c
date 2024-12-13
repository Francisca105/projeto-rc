#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define max(A, B) ((A) >= (B) ? (A) : (B))

int main(void) {
    struct addrinfo hints, *res;
    int fd, newfd, errcode, afd = 0;
    ssize_t n, nw;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char *ptr, buffer[128];
    struct sigaction act;
    fd_set rfds;
    enum {idle, busy} state;
    int maxfd, counter;

    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;

    if (sigaction(SIGPIPE, &act, NULL) == -1) exit(1); // error

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1); // error

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, "58001", &hints, &res)) != 0) exit(1); // error

    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) exit(1); // error
    if (listen(fd, 5) == -1) exit(1); // error

    state = idle;
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        maxfd = fd;
        if (state == busy) {
            FD_SET(afd, &rfds);
            maxfd = max(maxfd, afd);
        }

        counter = select(maxfd+1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);
        if (counter <= 0) exit(1); // error

        if (FD_ISSET(fd, &rfds)) {
            addrlen = sizeof(addr);
            if ((newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) == -1) exit(1); // error
            switch (state) {
                case idle:
                    afd = newfd;
                    state = busy;
                    break;
                case busy:
                    ptr = strcpy(buffer, "busy\n");
                    n = 5;
                    while (n > 0) {
                        nw = write(newfd, ptr, n);
                        write(1, "busy\n", 5);
                        if (nw == -1) exit(1); // error
                        n -= nw;
                        ptr += nw;
                    }
                    close(newfd);
                    break;
            }
        }
        if (FD_ISSET(afd, &rfds)) {
            if ((n = read(afd, buffer, 128)) != 0) {
                sleep(1);
                if (n == -1) exit(1); // error
                ptr = &buffer[0];
                while (n > 0) {
                    if ((nw = write(afd, ptr, n)) <= 0) exit(1); // error
                    n -= nw;
                    ptr += nw;
                }
            } else {
                close(afd);
                state = idle; // connection closed by peer
            }
        }
    }
    close(fd);
    exit(0);
}
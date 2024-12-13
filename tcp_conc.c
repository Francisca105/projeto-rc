#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
extern int errno;

int main(void) {
    struct sigaction act;
    struct addrinfo hints, *res;
    int fd, newfd, ret;
    ssize_t n, nw;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char *ptr, buffer[128];
    pid_t pid;

    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &act, NULL) == -1) exit(1); // error

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1); // error
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;

    if ((ret = getaddrinfo(NULL, "58001", &hints, &res)) != 0) exit(1); // error
    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) exit(1); // error
    if (listen(fd, 5) == -1) exit(1); // error
    freeaddrinfo(res); // frees the memory allocated by getaddrinfo (no longer needed)

    while (1) {
        addrlen = sizeof(addr);
        do newfd = accept(fd, (struct sockaddr*)&addr, &addrlen); // wait for a connection
        while (newfd == -1 && errno == EINTR);
        if (newfd == -1) exit(1); // error

        if ((pid = fork()) == -1) exit(1); // error
        else if (pid == 0) { // child process
            memset(&act, 0, sizeof(act));
            act.sa_handler = SIG_IGN;
            if (sigaction(SIGPIPE, &act, NULL) == -1) exit(1); // error
            close(fd);
            while ((n = read(newfd, buffer, 128)) != 0) {
                if (n == -1) exit(1); // error
                ptr = &buffer[0];
                while (n > 0) {
                    if ((nw = write(newfd, ptr, n)) <= 0) exit(1); // error
                    n -= nw;
                    ptr += nw;
                }
                close(newfd);
                exit(0);
            }
        }
        // parent process
        do ret = close(newfd);
        while (ret == -1 && errno == EINTR);
        if (ret == -1) exit(1); // error
    }
    close(fd);
    exit(0);
}
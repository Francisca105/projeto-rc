#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)
        exit(1); // error

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    // errcode = getaddrinfo("192.168.1.83", "58050", &hints, &res);
    errcode = getaddrinfo("localhost", "58050", &hints, &res);
    if (errcode != 0)
        exit(1); // error
    char s[] = "SNG 111111 100\n";
    n = sendto(fd, s, sizeof(s)-1, 0, res->ai_addr, res->ai_addrlen);
    // n = sendto(fd, "SNG 111111 300\n", 14, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        exit(1); // error
    close(fd);
    freeaddrinfo(res);
    exit(0);
}

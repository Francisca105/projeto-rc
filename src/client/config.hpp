#ifndef CONFIG_H
#define CONFIG_H

#include <sys/socket.h>

#include "support.hpp"

#define GSIP "localhost"
#define GSPORT "58050"

// Client config
typedef struct {
	std::string ip = GSIP;
	std::string port = GSPORT;
	int udp_fd;
	struct addrinfo *server_addr;
} Config;

void initConfig(Config *config, int argc, char **argv);
bool validateIp(const char *ip);
bool validatePort(const char *port);
void setUdpSocket(std::string ip, std::string port, int *fd,
									struct addrinfo **addr);

#endif
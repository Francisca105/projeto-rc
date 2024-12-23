#ifndef CONFIG_H
#define CONFIG_H

#include <sys/socket.h>

#include <string>

#define GSPORT "58050"
#define MAX_TCP_CONNECTIONS 10

// Server config
typedef struct {
	std::string port = GSPORT;
	bool verbose = false;
	int udp_fd;
	int tcp_fd;
	int scoreboard = 1;
} Config;

void initConfig(Config *config, int argc, char **argv);
bool validatePort(const char *port);
void setUdpSocket(std::string port, int *fd);
void setTcpSocket(std::string port, int *fd);

#endif

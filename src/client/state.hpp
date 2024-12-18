#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <sys/socket.h>

#include "support.hpp"

void initConfig(Config *config, int argc, char **argv);
bool validateIp(const char *ip);
bool validatePort(const char *port);
void setUdpSocket(std::string ip, std::string port, int *fd,
									struct addrinfo **addr);

#endif
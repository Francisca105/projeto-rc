#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <netdb.h>

#include <string>

#include "../common/constants.hpp"

typedef struct {
	std::string ip = GSIP;
	std::string port = GSPORT;
	int fd;
	struct addrinfo *addr;
	std::string plid;
	int nT;
} ClientState;

int initClientState(ClientState *state, int argc, char **argv);
void setIp(const char *buf, std::string &ip);
void setPort(const char *buf, std::string &port);

#endif

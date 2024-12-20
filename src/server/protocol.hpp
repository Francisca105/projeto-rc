#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <netdb.h>

#include <iostream>

// Client address
typedef struct {
	sockaddr addr;
	socklen_t addrlen = sizeof(addr);
} Address;

bool receiveUdp(std::string &packet, int fd, Address *addr);
bool sendUdp(std::string packet, int fd, Address addr);
bool receiveTcp(std::string &packet, int fd);
bool readString(std::string &buf, int fd, size_t size);

#endif

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <arpa/inet.h>

#include <string>

typedef struct {
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
} Address;

int setUdpSocket(std::string ip, std::string port, struct addrinfo **addr,
								 bool bind);
void sendUdpPacket(int fd, std::string packet, struct addrinfo addr);
void receiveUdpPacket(int fd, std::string &buf, Address *addr);
bool waitPacket(int fd, std::string &buf, Address *addr);
bool sendUdpAndWait(int fd, std::string packet, std::string &reply,
										struct addrinfo addr, Address *sender);

#endif

#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <sys/socket.h>

#include <string>

#include "support.hpp"

#define UDP_MAX_TRIES 3
#define UDP_TIMEOUT 10
#define TCP_TIMEOUT 30

bool validateIp(const char *ip);
bool validatePort(const char *port);
void setUdpSocket(std::string ip, std::string port, int *fd,
									struct addrinfo **addr);

bool sendUdpPacket(int fd, std::string packet, struct addrinfo addr);
bool receiveUdpPacket(int fd, std::string &reply);
bool waitUdpPacket(int fd, std::string &buf);
Packet sendUdpAndWait(int fd, std::string packet, std::string &reply,
											struct addrinfo addr);

bool setTcpSocket(std::string ip, std::string port, int *fd);
bool sendTcpPacket(int fd, std::string packet);
bool receiveTcpPacket(int fd, std::string &reply);
bool waitTcpPacket(int fd, std::string &buf);
Packet sendTcpAndWait(std::string request, std::string &reply, std::string ip,
											std::string port);

#endif

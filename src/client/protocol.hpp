#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "support.hpp"

#define UDP_TIMEOUT 10
#define UDP_MAX_TRIES 3
#define TCP_TIMEOUT 10

bool sendUdp(std::string packet, int fd, struct addrinfo addr);
bool receiveUdp(std::string &packet, int fd);
bool waitUdp(std::string &packet, int fd);
bool sendUdpAndReceive(std::string request, std::string &reply, int fd,
											 struct addrinfo addr);

bool setTcpSocket(std::string ip, std::string port, int *fd);
bool sendTcp(std::string packet, int fd);
bool receiveTcp(std::string &packet, int fd);
bool sendTcpAndReceive(std::string request, std::string &reply, std::string ip,
											 std::string port);

bool readChar(std::string &buf, int fd);
bool readString(std::string &buf, int fd);
bool readString(std::string &buf, int fd, size_t size);
bool compareChar(std::string buf, char c);
bool readUint(std::string &buf, int fd, size_t *n);

bool waitTcpPacket(std::string &buf, int fd);

#endif

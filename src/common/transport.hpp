#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <string>

int setUdpSocket(std::string ip, std::string port, struct addrinfo **addr, bool bind);

#endif

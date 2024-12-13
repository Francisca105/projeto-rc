#include "transport.hpp"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>

int setUdpSocket(std::string ip, std::string port, struct addrinfo **addr, bool server) {
	const char *ip_cstr = ip.c_str(), *port_cstr = port.c_str();
	int fd;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		std::cerr << "Could not set the UDP socket.\n";
		std::perror("Error - socket");
		exit(1);
	}
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;
	if (server) hints.ai_flags = AI_PASSIVE;
	int errcode = getaddrinfo(ip_cstr, port_cstr, &hints, addr);
	if (errcode != 0) {
		std::cerr << "Could not get the address information.\n"
							<< "Error - getaddrinfo: " << gai_strerror(errcode) << "\n";
		exit(1);
	}
	if (server and bind(fd, (*addr)->ai_addr, (*addr)->ai_addrlen) == -1) {
		std::cerr << "Could not bind the UDP socket.\n";
		std::perror("bind");
		exit(1);
	}
	return fd;
}
